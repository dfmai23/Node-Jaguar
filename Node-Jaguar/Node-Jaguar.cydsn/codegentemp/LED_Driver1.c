/*******************************************************************************
* File Name: LED_Driver1.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  The actual protocol and operation code resides in the interrupt service
*  routine file.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LED_Driver1_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 LED_Driver1_initVar = 0u; /* Defines if component was initialized */

volatile uint8 LED_Driver1_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: LED_Driver1_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LED_Driver1_Init(void) 
{
#if (LED_Driver1_FF_IMPLEMENTED)
    /* Configure fixed function block */
    LED_Driver1_CFG_REG  = LED_Driver1_DEFAULT_CFG;
    LED_Driver1_XCFG_REG = LED_Driver1_DEFAULT_XCFG;
    LED_Driver1_ADDR_REG = LED_Driver1_DEFAULT_ADDR;
    LED_Driver1_CLKDIV1_REG = LO8(LED_Driver1_DEFAULT_DIVIDE_FACTOR);
    LED_Driver1_CLKDIV2_REG = HI8(LED_Driver1_DEFAULT_DIVIDE_FACTOR);

#else
    uint8 intState;

    /* Configure control and interrupt sources */
    LED_Driver1_CFG_REG      = LED_Driver1_DEFAULT_CFG;
    LED_Driver1_INT_MASK_REG = LED_Driver1_DEFAULT_INT_MASK;

    /* Enable interrupt generation in status */
    intState = CyEnterCriticalSection();
    LED_Driver1_INT_ENABLE_REG |= LED_Driver1_INTR_ENABLE;
    CyExitCriticalSection(intState);

    /* Configure bit counter */
    #if (LED_Driver1_MODE_SLAVE_ENABLED)
        LED_Driver1_PERIOD_REG = LED_Driver1_DEFAULT_PERIOD;
    #endif  /* (LED_Driver1_MODE_SLAVE_ENABLED) */

    /* Configure clock generator */
    #if (LED_Driver1_MODE_MASTER_ENABLED)
        LED_Driver1_MCLK_PRD_REG = LED_Driver1_DEFAULT_MCLK_PRD;
        LED_Driver1_MCLK_CMP_REG = LED_Driver1_DEFAULT_MCLK_CMP;
    #endif /* (LED_Driver1_MODE_MASTER_ENABLED) */
#endif /* (LED_Driver1_FF_IMPLEMENTED) */

#if (LED_Driver1_TIMEOUT_ENABLED)
    LED_Driver1_TimeoutInit();
#endif /* (LED_Driver1_TIMEOUT_ENABLED) */

    /* Configure internal interrupt */
    CyIntDisable    (LED_Driver1_ISR_NUMBER);
    CyIntSetPriority(LED_Driver1_ISR_NUMBER, LED_Driver1_ISR_PRIORITY);
    #if (LED_Driver1_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(LED_Driver1_ISR_NUMBER, &LED_Driver1_ISR);
    #endif /* (LED_Driver1_INTERN_I2C_INTR_HANDLER) */

    /* Set FSM to default state */
    LED_Driver1_state = LED_Driver1_SM_IDLE;

#if (LED_Driver1_MODE_SLAVE_ENABLED)
    /* Clear status and buffers index */
    LED_Driver1_slStatus = 0u;
    LED_Driver1_slRdBufIndex = 0u;
    LED_Driver1_slWrBufIndex = 0u;

    /* Configure matched address */
    LED_Driver1_SlaveSetAddress(LED_Driver1_DEFAULT_ADDR);
#endif /* (LED_Driver1_MODE_SLAVE_ENABLED) */

#if (LED_Driver1_MODE_MASTER_ENABLED)
    /* Clear status and buffers index */
    LED_Driver1_mstrStatus = 0u;
    LED_Driver1_mstrRdBufIndex = 0u;
    LED_Driver1_mstrWrBufIndex = 0u;
#endif /* (LED_Driver1_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: LED_Driver1_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
*******************************************************************************/
void LED_Driver1_Enable(void) 
{
#if (LED_Driver1_FF_IMPLEMENTED)
    uint8 intState;

    /* Enable power to block */
    intState = CyEnterCriticalSection();
    LED_Driver1_ACT_PWRMGR_REG  |= LED_Driver1_ACT_PWR_EN;
    LED_Driver1_STBY_PWRMGR_REG |= LED_Driver1_STBY_PWR_EN;
    CyExitCriticalSection(intState);
#else
    #if (LED_Driver1_MODE_SLAVE_ENABLED)
        /* Enable bit counter */
        uint8 intState = CyEnterCriticalSection();
        LED_Driver1_COUNTER_AUX_CTL_REG |= LED_Driver1_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    #endif /* (LED_Driver1_MODE_SLAVE_ENABLED) */

    /* Enable slave or master bits */
    LED_Driver1_CFG_REG |= LED_Driver1_ENABLE_MS;
#endif /* (LED_Driver1_FF_IMPLEMENTED) */

#if (LED_Driver1_TIMEOUT_ENABLED)
    LED_Driver1_TimeoutEnable();
#endif /* (LED_Driver1_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: LED_Driver1_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This component automatically enables its interrupt.  If I2C is enabled !
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  LED_Driver1_initVar - This variable is used to check the initial
*                             configuration, modified on the first
*                             function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LED_Driver1_Start(void) 
{
    if (0u == LED_Driver1_initVar)
    {
        LED_Driver1_Init();
        LED_Driver1_initVar = 1u; /* Component initialized */
    }

    LED_Driver1_Enable();
    LED_Driver1_EnableInt();
}


/*******************************************************************************
* Function Name: LED_Driver1_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LED_Driver1_Stop(void) 
{
    LED_Driver1_DisableInt();

#if (LED_Driver1_TIMEOUT_ENABLED)
    LED_Driver1_TimeoutStop();
#endif  /* End (LED_Driver1_TIMEOUT_ENABLED) */

#if (LED_Driver1_FF_IMPLEMENTED)
    {
        uint8 intState;
        uint16 blockResetCycles;

        /* Store registers effected by block disable */
        LED_Driver1_backup.addr    = LED_Driver1_ADDR_REG;
        LED_Driver1_backup.clkDiv1 = LED_Driver1_CLKDIV1_REG;
        LED_Driver1_backup.clkDiv2 = LED_Driver1_CLKDIV2_REG;

        /* Calculate number of cycles to reset block */
        blockResetCycles = ((uint16) ((uint16) LED_Driver1_CLKDIV2_REG << 8u) | LED_Driver1_CLKDIV1_REG) + 1u;

        /* Disable block */
        LED_Driver1_CFG_REG &= (uint8) ~LED_Driver1_CFG_EN_SLAVE;
        /* Wait for block reset before disable power */
        CyDelayCycles((uint32) blockResetCycles);

        /* Disable power to block */
        intState = CyEnterCriticalSection();
        LED_Driver1_ACT_PWRMGR_REG  &= (uint8) ~LED_Driver1_ACT_PWR_EN;
        LED_Driver1_STBY_PWRMGR_REG &= (uint8) ~LED_Driver1_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable block */
        LED_Driver1_CFG_REG |= (uint8) LED_Driver1_ENABLE_MS;

        /* Restore registers effected by block disable. Ticket ID#198004 */
        LED_Driver1_ADDR_REG    = LED_Driver1_backup.addr;
        LED_Driver1_ADDR_REG    = LED_Driver1_backup.addr;
        LED_Driver1_CLKDIV1_REG = LED_Driver1_backup.clkDiv1;
        LED_Driver1_CLKDIV2_REG = LED_Driver1_backup.clkDiv2;
    }
#else

    /* Disable slave or master bits */
    LED_Driver1_CFG_REG &= (uint8) ~LED_Driver1_ENABLE_MS;

#if (LED_Driver1_MODE_SLAVE_ENABLED)
    {
        /* Disable bit counter */
        uint8 intState = CyEnterCriticalSection();
        LED_Driver1_COUNTER_AUX_CTL_REG &= (uint8) ~LED_Driver1_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    }
#endif /* (LED_Driver1_MODE_SLAVE_ENABLED) */

    /* Clear interrupt source register */
    (void) LED_Driver1_CSR_REG;
#endif /* (LED_Driver1_FF_IMPLEMENTED) */

    /* Disable interrupt on stop (enabled by write transaction) */
    LED_Driver1_DISABLE_INT_ON_STOP;
    LED_Driver1_ClearPendingInt();

    /* Reset FSM to default state */
    LED_Driver1_state = LED_Driver1_SM_IDLE;

    /* Clear busy statuses */
#if (LED_Driver1_MODE_SLAVE_ENABLED)
    LED_Driver1_slStatus &= (uint8) ~(LED_Driver1_SSTAT_RD_BUSY | LED_Driver1_SSTAT_WR_BUSY);
#endif /* (LED_Driver1_MODE_SLAVE_ENABLED) */
}


/* [] END OF FILE */
