/*******************************************************************************
* File Name: Pedal_Timer_PM.c
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

#include "Pedal_Timer.h"

static Pedal_Timer_backupStruct Pedal_Timer_backup;


/*******************************************************************************
* Function Name: Pedal_Timer_SaveConfig
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
*  Pedal_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Pedal_Timer_SaveConfig(void) 
{
    #if (!Pedal_Timer_UsingFixedFunction)
        Pedal_Timer_backup.TimerUdb = Pedal_Timer_ReadCounter();
        Pedal_Timer_backup.InterruptMaskValue = Pedal_Timer_STATUS_MASK;
        #if (Pedal_Timer_UsingHWCaptureCounter)
            Pedal_Timer_backup.TimerCaptureCounter = Pedal_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Pedal_Timer_UDB_CONTROL_REG_REMOVED)
            Pedal_Timer_backup.TimerControlRegister = Pedal_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Pedal_Timer_RestoreConfig
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
*  Pedal_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Pedal_Timer_RestoreConfig(void) 
{   
    #if (!Pedal_Timer_UsingFixedFunction)

        Pedal_Timer_WriteCounter(Pedal_Timer_backup.TimerUdb);
        Pedal_Timer_STATUS_MASK =Pedal_Timer_backup.InterruptMaskValue;
        #if (Pedal_Timer_UsingHWCaptureCounter)
            Pedal_Timer_SetCaptureCount(Pedal_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Pedal_Timer_UDB_CONTROL_REG_REMOVED)
            Pedal_Timer_WriteControlRegister(Pedal_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Pedal_Timer_Sleep
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
*  Pedal_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Pedal_Timer_Sleep(void) 
{
    #if(!Pedal_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Pedal_Timer_CTRL_ENABLE == (Pedal_Timer_CONTROL & Pedal_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Pedal_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Pedal_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Pedal_Timer_Stop();
    Pedal_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Pedal_Timer_Wakeup
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
*  Pedal_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Pedal_Timer_Wakeup(void) 
{
    Pedal_Timer_RestoreConfig();
    #if(!Pedal_Timer_UDB_CONTROL_REG_REMOVED)
        if(Pedal_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Pedal_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
