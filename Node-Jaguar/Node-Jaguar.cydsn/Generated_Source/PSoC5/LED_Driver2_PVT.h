/*******************************************************************************
* File Name: .h
* Version 3.50
*
* Description:
*  This file provides private constants and parameter values for the I2C
*  component.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_PVT_LED_Driver2_H)
#define CY_I2C_PVT_LED_Driver2_H

#include "LED_Driver2.h"

#define LED_Driver2_TIMEOUT_ENABLED_INC    (0u)
#if (0u != LED_Driver2_TIMEOUT_ENABLED_INC)
    #include "LED_Driver2_TMOUT.h"
#endif /* (0u != LED_Driver2_TIMEOUT_ENABLED_INC) */


/**********************************
*   Variables with external linkage
**********************************/

extern LED_Driver2_BACKUP_STRUCT LED_Driver2_backup;

extern volatile uint8 LED_Driver2_state;   /* Current state of I2C FSM */

/* Master variables */
#if (LED_Driver2_MODE_MASTER_ENABLED)
    extern volatile uint8 LED_Driver2_mstrStatus;   /* Master Status byte  */
    extern volatile uint8 LED_Driver2_mstrControl;  /* Master Control byte */

    /* Transmit buffer variables */
    extern volatile uint8 * LED_Driver2_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint8   LED_Driver2_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint8   LED_Driver2_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Receive buffer variables */
    extern volatile uint8 * LED_Driver2_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint8   LED_Driver2_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint8   LED_Driver2_mstrWrBufIndex; /* Master Write buffer Index      */

#endif /* (LED_Driver2_MODE_MASTER_ENABLED) */

/* Slave variables */
#if (LED_Driver2_MODE_SLAVE_ENABLED)
    extern volatile uint8 LED_Driver2_slStatus;         /* Slave Status  */

    /* Transmit buffer variables */
    extern volatile uint8 * LED_Driver2_slRdBufPtr;     /* Pointer to Transmit buffer  */
    extern volatile uint8   LED_Driver2_slRdBufSize;    /* Slave Transmit buffer size  */
    extern volatile uint8   LED_Driver2_slRdBufIndex;   /* Slave Transmit buffer Index */

    /* Receive buffer variables */
    extern volatile uint8 * LED_Driver2_slWrBufPtr;     /* Pointer to Receive buffer  */
    extern volatile uint8   LED_Driver2_slWrBufSize;    /* Slave Receive buffer size  */
    extern volatile uint8   LED_Driver2_slWrBufIndex;   /* Slave Receive buffer Index */

    #if (LED_Driver2_SW_ADRR_DECODE)
        extern volatile uint8 LED_Driver2_slAddress;     /* Software address variable */
    #endif   /* (LED_Driver2_SW_ADRR_DECODE) */

#endif /* (LED_Driver2_MODE_SLAVE_ENABLED) */

#if ((LED_Driver2_FF_IMPLEMENTED) && (LED_Driver2_WAKEUP_ENABLED))
    extern volatile uint8 LED_Driver2_wakeupSource;
#endif /* ((LED_Driver2_FF_IMPLEMENTED) && (LED_Driver2_WAKEUP_ENABLED)) */


#endif /* CY_I2C_PVT_LED_Driver2_H */


/* [] END OF FILE */
