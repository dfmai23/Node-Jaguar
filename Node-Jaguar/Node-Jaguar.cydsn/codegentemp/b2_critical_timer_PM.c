/*******************************************************************************
* File Name: b2_critical_timer_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "b2_critical_timer.h"

static b2_critical_timer_backupStruct b2_critical_timer_backup;


/*******************************************************************************
* Function Name: b2_critical_timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  b2_critical_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void b2_critical_timer_SaveConfig(void) 
{
    #if (!b2_critical_timer_UsingFixedFunction)
        b2_critical_timer_backup.TimerUdb = b2_critical_timer_ReadCounter();
        b2_critical_timer_backup.InterruptMaskValue = b2_critical_timer_STATUS_MASK;
        #if (b2_critical_timer_UsingHWCaptureCounter)
            b2_critical_timer_backup.TimerCaptureCounter = b2_critical_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!b2_critical_timer_UDB_CONTROL_REG_REMOVED)
            b2_critical_timer_backup.TimerControlRegister = b2_critical_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: b2_critical_timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  b2_critical_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void b2_critical_timer_RestoreConfig(void) 
{   
    #if (!b2_critical_timer_UsingFixedFunction)

        b2_critical_timer_WriteCounter(b2_critical_timer_backup.TimerUdb);
        b2_critical_timer_STATUS_MASK =b2_critical_timer_backup.InterruptMaskValue;
        #if (b2_critical_timer_UsingHWCaptureCounter)
            b2_critical_timer_SetCaptureCount(b2_critical_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!b2_critical_timer_UDB_CONTROL_REG_REMOVED)
            b2_critical_timer_WriteControlRegister(b2_critical_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: b2_critical_timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  b2_critical_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void b2_critical_timer_Sleep(void) 
{
    #if(!b2_critical_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(b2_critical_timer_CTRL_ENABLE == (b2_critical_timer_CONTROL & b2_critical_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            b2_critical_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            b2_critical_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    b2_critical_timer_Stop();
    b2_critical_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: b2_critical_timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  b2_critical_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void b2_critical_timer_Wakeup(void) 
{
    b2_critical_timer_RestoreConfig();
    #if(!b2_critical_timer_UDB_CONTROL_REG_REMOVED)
        if(b2_critical_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                b2_critical_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
