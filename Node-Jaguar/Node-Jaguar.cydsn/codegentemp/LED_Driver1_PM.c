/*******************************************************************************
* File Name: LED_Driver1_PM.c
* Version 3.50
*
* Description:
*  This file provides low power mode APIs for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LED_Driver1_PVT.h"

LED_Driver1_BACKUP_STRUCT LED_Driver1_backup =
{
    LED_Driver1_DISABLE,

#if (LED_Driver1_FF_IMPLEMENTED)
    LED_Driver1_DEFAULT_XCFG,
    LED_Driver1_DEFAULT_CFG,
    LED_Driver1_DEFAULT_ADDR,
    LO8(LED_Driver1_DEFAULT_DIVIDE_FACTOR),
    HI8(LED_Driver1_DEFAULT_DIVIDE_FACTOR),
#else  /* (LED_Driver1_UDB_IMPLEMENTED) */
    LED_Driver1_DEFAULT_CFG,
#endif /* (LED_Driver1_FF_IMPLEMENTED) */

#if (LED_Driver1_TIMEOUT_ENABLED)
    LED_Driver1_DEFAULT_TMOUT_PERIOD,
    LED_Driver1_DEFAULT_TMOUT_INTR_MASK,
#endif /* (LED_Driver1_TIMEOUT_ENABLED) */
};

#if ((LED_Driver1_FF_IMPLEMENTED) && (LED_Driver1_WAKEUP_ENABLED))
    volatile uint8 LED_Driver1_wakeupSource;
#endif /* ((LED_Driver1_FF_IMPLEMENTED) && (LED_Driver1_WAKEUP_ENABLED)) */


/*******************************************************************************
* Function Name: LED_Driver1_SaveConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Stores the component non-retention configuration registers.
*   Checked:   Disables the master, if it was enabled before, and enables
*              backup regulator of the I2C hardware. If a transaction intended
*              for component executes during this function call, it waits until
*              the current transaction is completed and I2C hardware is ready
*              to enter sleep mode. All subsequent I2C traffic is NAKed until
*              the device is put into sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LED_Driver1_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            entering the sleep mode.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LED_Driver1_SaveConfig(void) 
{
#if (LED_Driver1_FF_IMPLEMENTED)
    #if (LED_Driver1_WAKEUP_ENABLED)
        uint8 intState;
    #endif /* (LED_Driver1_WAKEUP_ENABLED) */

    /* Store registers before enter low power mode */
    LED_Driver1_backup.cfg     = LED_Driver1_CFG_REG;
    LED_Driver1_backup.xcfg    = LED_Driver1_XCFG_REG;
    LED_Driver1_backup.addr    = LED_Driver1_ADDR_REG;
    LED_Driver1_backup.clkDiv1 = LED_Driver1_CLKDIV1_REG;
    LED_Driver1_backup.clkDiv2 = LED_Driver1_CLKDIV2_REG;

#if (LED_Driver1_WAKEUP_ENABLED)
    /* Disable master */
    LED_Driver1_CFG_REG &= (uint8) ~LED_Driver1_ENABLE_MASTER;

    /* Enable backup regulator to keep block powered in low power mode */
    intState = CyEnterCriticalSection();
    LED_Driver1_PWRSYS_CR1_REG |= LED_Driver1_PWRSYS_CR1_I2C_REG_BACKUP;
    CyExitCriticalSection(intState);

    /* 1) Set force NACK to ignore I2C transactions;
    *  2) Wait unti I2C is ready go to Sleep; !!
    *  3) These bits are cleared on wake up.
    */
    /* Wait when block is ready for sleep */
    LED_Driver1_XCFG_REG |= LED_Driver1_XCFG_FORCE_NACK;
    while (0u == (LED_Driver1_XCFG_REG & LED_Driver1_XCFG_RDY_TO_SLEEP))
    {
    }

    /* Setup wakeup interrupt */
    LED_Driver1_DisableInt();
    (void) CyIntSetVector(LED_Driver1_ISR_NUMBER, &LED_Driver1_WAKEUP_ISR);
    LED_Driver1_wakeupSource = 0u;
    LED_Driver1_EnableInt();
#endif /* (LED_Driver1_WAKEUP_ENABLED) */

#else
    /* Store only address match bit */
    LED_Driver1_backup.control = (LED_Driver1_CFG_REG & LED_Driver1_CTRL_ANY_ADDRESS_MASK);
#endif /* (LED_Driver1_FF_IMPLEMENTED) */

#if (LED_Driver1_TIMEOUT_ENABLED)
    LED_Driver1_TimeoutSaveConfig();
#endif /* (LED_Driver1_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: LED_Driver1_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component before device enters
*  sleep mode. The Enable wakeup from Sleep Mode selection influences this
*  function implementation:
*   Unchecked: Checks current I2C component state, saves it, and disables the
*              component by calling I2C_Stop() if it is currently enabled.
*              I2C_SaveConfig() is then called to save the component
*              non-retention configuration registers.
*   Checked:   If a transaction intended for component executes during this
*              function call, it waits until the current transaction is
*              completed. All subsequent I2C traffic intended for component
*              is NAKed until the device is put to sleep mode. The address
*              match event wakes up the device.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LED_Driver1_Sleep(void) 
{
#if (LED_Driver1_WAKEUP_ENABLED)
    /* Do not enable block after exit low power mode if it is wakeup source */
    LED_Driver1_backup.enableState = LED_Driver1_DISABLE;

    #if (LED_Driver1_TIMEOUT_ENABLED)
        LED_Driver1_TimeoutStop();
    #endif /* (LED_Driver1_TIMEOUT_ENABLED) */

#else
    /* Store enable state */
    LED_Driver1_backup.enableState = (uint8) LED_Driver1_IS_ENABLED;

    if (0u != LED_Driver1_backup.enableState)
    {
        LED_Driver1_Stop();
    }
#endif /* (LED_Driver1_WAKEUP_ENABLED) */

    LED_Driver1_SaveConfig();
}


/*******************************************************************************
* Function Name: LED_Driver1_RestoreConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              to the state they were in before I2C_Sleep() or I2C_SaveConfig()
*              was called.
*   Checked:   Disables the backup regulator of the I2C hardware. Sets up the
*              regular component interrupt handler and generates the component
*              interrupt if it was wake up source to release the bus and
*              continue in-coming I2C transaction.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LED_Driver1_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            exiting the sleep mode.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before LED_Driver1_SaveConfig() or
*  LED_Driver1_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void LED_Driver1_RestoreConfig(void) 
{
#if (LED_Driver1_FF_IMPLEMENTED)
    uint8 intState;

    if (LED_Driver1_CHECK_PWRSYS_I2C_BACKUP)
    /* Low power mode was Sleep - backup regulator is enabled */
    {
        /* Enable backup regulator in active mode */
        intState = CyEnterCriticalSection();
        LED_Driver1_PWRSYS_CR1_REG &= (uint8) ~LED_Driver1_PWRSYS_CR1_I2C_REG_BACKUP;
        CyExitCriticalSection(intState);

        /* Restore master */
        LED_Driver1_CFG_REG = LED_Driver1_backup.cfg;
    }
    else
    /* Low power mode was Hibernate - backup regulator is disabled. All registers are cleared */
    {
    #if (LED_Driver1_WAKEUP_ENABLED)
        /* Disable power to block before register restore */
        intState = CyEnterCriticalSection();
        LED_Driver1_ACT_PWRMGR_REG  &= (uint8) ~LED_Driver1_ACT_PWR_EN;
        LED_Driver1_STBY_PWRMGR_REG &= (uint8) ~LED_Driver1_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable component in I2C_Wakeup() after register restore */
        LED_Driver1_backup.enableState = LED_Driver1_ENABLE;
    #endif /* (LED_Driver1_WAKEUP_ENABLED) */

        /* Restore component registers after Hibernate */
        LED_Driver1_XCFG_REG    = LED_Driver1_backup.xcfg;
        LED_Driver1_CFG_REG     = LED_Driver1_backup.cfg;
        LED_Driver1_ADDR_REG    = LED_Driver1_backup.addr;
        LED_Driver1_CLKDIV1_REG = LED_Driver1_backup.clkDiv1;
        LED_Driver1_CLKDIV2_REG = LED_Driver1_backup.clkDiv2;
    }

#if (LED_Driver1_WAKEUP_ENABLED)
    LED_Driver1_DisableInt();
    (void) CyIntSetVector(LED_Driver1_ISR_NUMBER, &LED_Driver1_ISR);
    if (0u != LED_Driver1_wakeupSource)
    {
        /* Generate interrupt to process incoming transaction */
        LED_Driver1_SetPendingInt();
    }
    LED_Driver1_EnableInt();
#endif /* (LED_Driver1_WAKEUP_ENABLED) */

#else
    LED_Driver1_CFG_REG = LED_Driver1_backup.control;
#endif /* (LED_Driver1_FF_IMPLEMENTED) */

#if (LED_Driver1_TIMEOUT_ENABLED)
    LED_Driver1_TimeoutRestoreConfig();
#endif /* (LED_Driver1_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: LED_Driver1_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component for active mode
*  operation (when device exits sleep mode). The Enable wakeup from Sleep Mode
*  selection influences this function implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              by calling I2C_RestoreConfig(). If the component was enabled
*              before the I2C_Sleep() function was called, I2C_Wakeup()
*              re-enables it.
*   Checked:   Enables  master functionality if it was enabled before sleep,
*              and disables the backup regulator of the I2C hardware.
*              The incoming transaction continues as soon as the regular
*              I2C interrupt handler is set up (global interrupts has to be
*              enabled to service I2C component interrupt).
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before LED_Driver1_SaveConfig() or
*  LED_Driver1_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void LED_Driver1_Wakeup(void) 
{
    LED_Driver1_RestoreConfig();

    /* Restore component enable state */
    if (0u != LED_Driver1_backup.enableState)
    {
        LED_Driver1_Enable();
        LED_Driver1_EnableInt();
    }
    else
    {
    #if (LED_Driver1_TIMEOUT_ENABLED)
        LED_Driver1_TimeoutEnable();
    #endif /* (LED_Driver1_TIMEOUT_ENABLED) */
    }
}


/* [] END OF FILE */
