/*******************************************************************************
* File Name: Tach.h  
* Version 2.10
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Tach_H) /* Pins Tach_H */
#define CY_PINS_Tach_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Tach_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Tach__PORT == 15 && ((Tach__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    Tach_Write(uint8 value) ;
void    Tach_SetDriveMode(uint8 mode) ;
uint8   Tach_ReadDataReg(void) ;
uint8   Tach_Read(void) ;
uint8   Tach_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define Tach_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define Tach_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define Tach_DM_RES_UP          PIN_DM_RES_UP
#define Tach_DM_RES_DWN         PIN_DM_RES_DWN
#define Tach_DM_OD_LO           PIN_DM_OD_LO
#define Tach_DM_OD_HI           PIN_DM_OD_HI
#define Tach_DM_STRONG          PIN_DM_STRONG
#define Tach_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define Tach_MASK               Tach__MASK
#define Tach_SHIFT              Tach__SHIFT
#define Tach_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Tach_PS                     (* (reg8 *) Tach__PS)
/* Data Register */
#define Tach_DR                     (* (reg8 *) Tach__DR)
/* Port Number */
#define Tach_PRT_NUM                (* (reg8 *) Tach__PRT) 
/* Connect to Analog Globals */                                                  
#define Tach_AG                     (* (reg8 *) Tach__AG)                       
/* Analog MUX bux enable */
#define Tach_AMUX                   (* (reg8 *) Tach__AMUX) 
/* Bidirectional Enable */                                                        
#define Tach_BIE                    (* (reg8 *) Tach__BIE)
/* Bit-mask for Aliased Register Access */
#define Tach_BIT_MASK               (* (reg8 *) Tach__BIT_MASK)
/* Bypass Enable */
#define Tach_BYP                    (* (reg8 *) Tach__BYP)
/* Port wide control signals */                                                   
#define Tach_CTL                    (* (reg8 *) Tach__CTL)
/* Drive Modes */
#define Tach_DM0                    (* (reg8 *) Tach__DM0) 
#define Tach_DM1                    (* (reg8 *) Tach__DM1)
#define Tach_DM2                    (* (reg8 *) Tach__DM2) 
/* Input Buffer Disable Override */
#define Tach_INP_DIS                (* (reg8 *) Tach__INP_DIS)
/* LCD Common or Segment Drive */
#define Tach_LCD_COM_SEG            (* (reg8 *) Tach__LCD_COM_SEG)
/* Enable Segment LCD */
#define Tach_LCD_EN                 (* (reg8 *) Tach__LCD_EN)
/* Slew Rate Control */
#define Tach_SLW                    (* (reg8 *) Tach__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Tach_PRTDSI__CAPS_SEL       (* (reg8 *) Tach__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Tach_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Tach__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Tach_PRTDSI__OE_SEL0        (* (reg8 *) Tach__PRTDSI__OE_SEL0) 
#define Tach_PRTDSI__OE_SEL1        (* (reg8 *) Tach__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Tach_PRTDSI__OUT_SEL0       (* (reg8 *) Tach__PRTDSI__OUT_SEL0) 
#define Tach_PRTDSI__OUT_SEL1       (* (reg8 *) Tach__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Tach_PRTDSI__SYNC_OUT       (* (reg8 *) Tach__PRTDSI__SYNC_OUT) 


#if defined(Tach__INTSTAT)  /* Interrupt Registers */

    #define Tach_INTSTAT                (* (reg8 *) Tach__INTSTAT)
    #define Tach_SNAP                   (* (reg8 *) Tach__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Tach_H */


/* [] END OF FILE */
