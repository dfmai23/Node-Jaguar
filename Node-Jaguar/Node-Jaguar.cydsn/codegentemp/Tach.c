/*******************************************************************************
* File Name: Tach.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Tach.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 Tach__PORT == 15 && ((Tach__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: Tach_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void Tach_Write(uint8 value) 
{
    uint8 staticBits = (Tach_DR & (uint8)(~Tach_MASK));
    Tach_DR = staticBits | ((uint8)(value << Tach_SHIFT) & Tach_MASK);
}


/*******************************************************************************
* Function Name: Tach_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  Tach_DM_STRONG     Strong Drive 
*  Tach_DM_OD_HI      Open Drain, Drives High 
*  Tach_DM_OD_LO      Open Drain, Drives Low 
*  Tach_DM_RES_UP     Resistive Pull Up 
*  Tach_DM_RES_DWN    Resistive Pull Down 
*  Tach_DM_RES_UPDWN  Resistive Pull Up/Down 
*  Tach_DM_DIG_HIZ    High Impedance Digital 
*  Tach_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void Tach_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(Tach_0, mode);
}


/*******************************************************************************
* Function Name: Tach_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro Tach_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Tach_Read(void) 
{
    return (Tach_PS & Tach_MASK) >> Tach_SHIFT;
}


/*******************************************************************************
* Function Name: Tach_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 Tach_ReadDataReg(void) 
{
    return (Tach_DR & Tach_MASK) >> Tach_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Tach_INTSTAT) 

    /*******************************************************************************
    * Function Name: Tach_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 Tach_ClearInterrupt(void) 
    {
        return (Tach_INTSTAT & Tach_MASK) >> Tach_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
