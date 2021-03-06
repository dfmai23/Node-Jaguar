/*******************************************************************************
* File Name: LED_Driver1_INT.c
* Version 3.50
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LED_Driver1_PVT.h"
#include "cyapicallbacks.h"


/*******************************************************************************
*  Place your includes, defines and code here.
********************************************************************************/
/* `#START LED_Driver1_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: LED_Driver1_ISR
********************************************************************************
*
* Summary:
*  The handler for the I2C interrupt. The slave and master operations are
*  handled here.
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
CY_ISR(LED_Driver1_ISR)
{
#if (LED_Driver1_MODE_SLAVE_ENABLED)
   uint8  tmp8;
#endif  /* (LED_Driver1_MODE_SLAVE_ENABLED) */

    uint8  tmpCsr;
    
#ifdef LED_Driver1_ISR_ENTRY_CALLBACK
    LED_Driver1_ISR_EntryCallback();
#endif /* LED_Driver1_ISR_ENTRY_CALLBACK */
    

#if(LED_Driver1_TIMEOUT_FF_ENABLED)
    if(0u != LED_Driver1_TimeoutGetStatus())
    {
        LED_Driver1_TimeoutReset();
        LED_Driver1_state = LED_Driver1_SM_EXIT_IDLE;
        /* LED_Driver1_CSR_REG should be cleared after reset */
    }
#endif /* (LED_Driver1_TIMEOUT_FF_ENABLED) */


    tmpCsr = LED_Driver1_CSR_REG;      /* Make copy as interrupts clear */

#if(LED_Driver1_MODE_MULTI_MASTER_SLAVE_ENABLED)
    if(LED_Driver1_CHECK_START_GEN(LED_Driver1_MCSR_REG))
    {
        LED_Driver1_CLEAR_START_GEN;

        /* Set transfer complete and error flags */
        LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_ERR_XFER |
                                        LED_Driver1_GET_MSTAT_CMPLT);

        /* Slave was addressed */
        LED_Driver1_state = LED_Driver1_SM_SLAVE;
    }
#endif /* (LED_Driver1_MODE_MULTI_MASTER_SLAVE_ENABLED) */


#if(LED_Driver1_MODE_MULTI_MASTER_ENABLED)
    if(LED_Driver1_CHECK_LOST_ARB(tmpCsr))
    {
        /* Set errors */
        LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_ERR_XFER     |
                                        LED_Driver1_MSTAT_ERR_ARB_LOST |
                                        LED_Driver1_GET_MSTAT_CMPLT);

        LED_Driver1_DISABLE_INT_ON_STOP; /* Interrupt on Stop is enabled by write */

        #if(LED_Driver1_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(LED_Driver1_CHECK_ADDRESS_STS(tmpCsr))
            {
                /* Slave was addressed */
                LED_Driver1_state = LED_Driver1_SM_SLAVE;
            }
            else
            {
                LED_Driver1_BUS_RELEASE;

                LED_Driver1_state = LED_Driver1_SM_EXIT_IDLE;
            }
        #else
            LED_Driver1_BUS_RELEASE;

            LED_Driver1_state = LED_Driver1_SM_EXIT_IDLE;

        #endif /* (LED_Driver1_MODE_MULTI_MASTER_SLAVE_ENABLED) */
    }
#endif /* (LED_Driver1_MODE_MULTI_MASTER_ENABLED) */

    /* Check for master operation mode */
    if(LED_Driver1_CHECK_SM_MASTER)
    {
    #if(LED_Driver1_MODE_MASTER_ENABLED)
        if(LED_Driver1_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            switch (LED_Driver1_state)
            {
            case LED_Driver1_SM_MSTR_WR_ADDR:  /* After address is sent, write data */
            case LED_Driver1_SM_MSTR_RD_ADDR:  /* After address is sent, read data */

                tmpCsr &= ((uint8) ~LED_Driver1_CSR_STOP_STATUS); /* Clear Stop bit history on address phase */

                if(LED_Driver1_CHECK_ADDR_ACK(tmpCsr))
                {
                    /* Setup for transmit or receive of data */
                    if(LED_Driver1_state == LED_Driver1_SM_MSTR_WR_ADDR)   /* TRANSMIT data */
                    {
                        /* Check if at least one byte to transfer */
                        if(LED_Driver1_mstrWrBufSize > 0u)
                        {
                            /* Load the 1st data byte */
                            LED_Driver1_DATA_REG = LED_Driver1_mstrWrBufPtr[0u];
                            LED_Driver1_TRANSMIT_DATA;
                            LED_Driver1_mstrWrBufIndex = 1u;   /* Set index to 2nd element */

                            /* Set transmit state until done */
                            LED_Driver1_state = LED_Driver1_SM_MSTR_WR_DATA;
                        }
                        /* End of buffer: complete writing */
                        else if(LED_Driver1_CHECK_NO_STOP(LED_Driver1_mstrControl))
                        {
                            /* Set write complete and master halted */
                            LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_XFER_HALT |
                                                            LED_Driver1_MSTAT_WR_CMPLT);

                            LED_Driver1_state = LED_Driver1_SM_MSTR_HALT; /* Expect ReStart */
                            LED_Driver1_DisableInt();
                        }
                        else
                        {
                            LED_Driver1_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                            LED_Driver1_GENERATE_STOP;
                        }
                    }
                    else  /* Master receive data */
                    {
                        LED_Driver1_READY_TO_READ; /* Release bus to read data */

                        LED_Driver1_state  = LED_Driver1_SM_MSTR_RD_DATA;
                    }
                }
                /* Address is NACKed */
                else if(LED_Driver1_CHECK_ADDR_NAK(tmpCsr))
                {
                    /* Set Address NAK error */
                    LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_ERR_XFER |
                                                    LED_Driver1_MSTAT_ERR_ADDR_NAK);

                    if(LED_Driver1_CHECK_NO_STOP(LED_Driver1_mstrControl))
                    {
                        LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_XFER_HALT |
                                                        LED_Driver1_GET_MSTAT_CMPLT);

                        LED_Driver1_state = LED_Driver1_SM_MSTR_HALT; /* Expect RESTART */
                        LED_Driver1_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        LED_Driver1_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                        LED_Driver1_GENERATE_STOP;
                    }
                }
                else
                {
                    /* Address phase is not set for some reason: error */
                    #if(LED_Driver1_TIMEOUT_ENABLED)
                        /* Exit interrupt to take chance for timeout timer to handle this case */
                        LED_Driver1_DisableInt();
                        LED_Driver1_ClearPendingInt();
                    #else
                        /* Block execution flow: unexpected condition */
                        CYASSERT(0u != 0u);
                    #endif /* (LED_Driver1_TIMEOUT_ENABLED) */
                }
                break;

            case LED_Driver1_SM_MSTR_WR_DATA:

                if(LED_Driver1_CHECK_DATA_ACK(tmpCsr))
                {
                    /* Check if end of buffer */
                    if(LED_Driver1_mstrWrBufIndex  < LED_Driver1_mstrWrBufSize)
                    {
                        LED_Driver1_DATA_REG =
                                                 LED_Driver1_mstrWrBufPtr[LED_Driver1_mstrWrBufIndex];
                        LED_Driver1_TRANSMIT_DATA;
                        LED_Driver1_mstrWrBufIndex++;
                    }
                    /* End of buffer: complete writing */
                    else if(LED_Driver1_CHECK_NO_STOP(LED_Driver1_mstrControl))
                    {
                        /* Set write complete and master halted */
                        LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_XFER_HALT |
                                                        LED_Driver1_MSTAT_WR_CMPLT);

                        LED_Driver1_state = LED_Driver1_SM_MSTR_HALT;    /* Expect restart */
                        LED_Driver1_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        LED_Driver1_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                        LED_Driver1_GENERATE_STOP;
                    }
                }
                /* Last byte NAKed: end writing */
                else if(LED_Driver1_CHECK_NO_STOP(LED_Driver1_mstrControl))
                {
                    /* Set write complete, short transfer and master halted */
                    LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_ERR_XFER       |
                                                    LED_Driver1_MSTAT_ERR_SHORT_XFER |
                                                    LED_Driver1_MSTAT_XFER_HALT      |
                                                    LED_Driver1_MSTAT_WR_CMPLT);

                    LED_Driver1_state = LED_Driver1_SM_MSTR_HALT;    /* Expect ReStart */
                    LED_Driver1_DisableInt();
                }
                else  /* Do normal Stop */
                {
                    LED_Driver1_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                    LED_Driver1_GENERATE_STOP;

                    /* Set short transfer and error flag */
                    LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_ERR_SHORT_XFER |
                                                    LED_Driver1_MSTAT_ERR_XFER);
                }

                break;

            case LED_Driver1_SM_MSTR_RD_DATA:

                LED_Driver1_mstrRdBufPtr[LED_Driver1_mstrRdBufIndex] = LED_Driver1_DATA_REG;
                LED_Driver1_mstrRdBufIndex++;

                /* Check if end of buffer */
                if(LED_Driver1_mstrRdBufIndex < LED_Driver1_mstrRdBufSize)
                {
                    LED_Driver1_ACK_AND_RECEIVE;       /* ACK and receive byte */
                }
                /* End of buffer: complete reading */
                else if(LED_Driver1_CHECK_NO_STOP(LED_Driver1_mstrControl))
                {
                    /* Set read complete and master halted */
                    LED_Driver1_mstrStatus |= (LED_Driver1_MSTAT_XFER_HALT |
                                                    LED_Driver1_MSTAT_RD_CMPLT);

                    LED_Driver1_state = LED_Driver1_SM_MSTR_HALT;    /* Expect ReStart */
                    LED_Driver1_DisableInt();
                }
                else
                {
                    LED_Driver1_ENABLE_INT_ON_STOP;
                    LED_Driver1_NAK_AND_RECEIVE;       /* NACK and TRY to generate Stop */
                }
                break;

            default: /* This is an invalid state and should not occur */

            #if(LED_Driver1_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handles this case */
                LED_Driver1_DisableInt();
                LED_Driver1_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (LED_Driver1_TIMEOUT_ENABLED) */

                break;
            }
        }

        /* Catches Stop: end of transaction */
        if(LED_Driver1_CHECK_STOP_STS(tmpCsr))
        {
            LED_Driver1_mstrStatus |= LED_Driver1_GET_MSTAT_CMPLT;

            LED_Driver1_DISABLE_INT_ON_STOP;
            LED_Driver1_state = LED_Driver1_SM_IDLE;
        }
    #endif /* (LED_Driver1_MODE_MASTER_ENABLED) */
    }
    else if(LED_Driver1_CHECK_SM_SLAVE)
    {
    #if(LED_Driver1_MODE_SLAVE_ENABLED)

        if((LED_Driver1_CHECK_STOP_STS(tmpCsr)) || /* Stop || Restart */
           (LED_Driver1_CHECK_BYTE_COMPLETE(tmpCsr) && LED_Driver1_CHECK_ADDRESS_STS(tmpCsr)))
        {
            /* Catch end of master write transaction: use interrupt on Stop */
            /* The Stop bit history on address phase does not have correct state */
            if(LED_Driver1_SM_SL_WR_DATA == LED_Driver1_state)
            {
                LED_Driver1_DISABLE_INT_ON_STOP;

                LED_Driver1_slStatus &= ((uint8) ~LED_Driver1_SSTAT_WR_BUSY);
                LED_Driver1_slStatus |= ((uint8)  LED_Driver1_SSTAT_WR_CMPLT);

                LED_Driver1_state = LED_Driver1_SM_IDLE;
            }
        }

        if(LED_Driver1_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            /* The address only issued after Start or ReStart: so check the address
               to catch these events:
                FF : sets an address phase with a byte_complete interrupt trigger.
                UDB: sets an address phase immediately after Start or ReStart. */
            if(LED_Driver1_CHECK_ADDRESS_STS(tmpCsr))
            {
            /* Check for software address detection */
            #if(LED_Driver1_SW_ADRR_DECODE)
                tmp8 = LED_Driver1_GET_SLAVE_ADDR(LED_Driver1_DATA_REG);

                if(tmp8 == LED_Driver1_slAddress)   /* Check for address match */
                {
                    if(0u != (LED_Driver1_DATA_REG & LED_Driver1_READ_FLAG))
                    {
                        /* Place code to prepare read buffer here                  */
                        /* `#START LED_Driver1_SW_PREPARE_READ_BUF_interrupt` */

                        /* `#END` */

                    #ifdef LED_Driver1_SW_PREPARE_READ_BUF_CALLBACK
                        LED_Driver1_SwPrepareReadBuf_Callback();
                    #endif /* LED_Driver1_SW_PREPARE_READ_BUF_CALLBACK */
                        
                        /* Prepare next operation to read, get data and place in data register */
                        if(LED_Driver1_slRdBufIndex < LED_Driver1_slRdBufSize)
                        {
                            /* Load first data byte from array */
                            LED_Driver1_DATA_REG = LED_Driver1_slRdBufPtr[LED_Driver1_slRdBufIndex];
                            LED_Driver1_ACK_AND_TRANSMIT;
                            LED_Driver1_slRdBufIndex++;

                            LED_Driver1_slStatus |= LED_Driver1_SSTAT_RD_BUSY;
                        }
                        else    /* Overflow: provide 0xFF on bus */
                        {
                            LED_Driver1_DATA_REG = LED_Driver1_OVERFLOW_RETURN;
                            LED_Driver1_ACK_AND_TRANSMIT;

                            LED_Driver1_slStatus  |= (LED_Driver1_SSTAT_RD_BUSY |
                                                           LED_Driver1_SSTAT_RD_ERR_OVFL);
                        }

                        LED_Driver1_state = LED_Driver1_SM_SL_RD_DATA;
                    }
                    else  /* Write transaction: receive 1st byte */
                    {
                        LED_Driver1_ACK_AND_RECEIVE;
                        LED_Driver1_state = LED_Driver1_SM_SL_WR_DATA;

                        LED_Driver1_slStatus |= LED_Driver1_SSTAT_WR_BUSY;
                        LED_Driver1_ENABLE_INT_ON_STOP;
                    }
                }
                else
                {
                    /*     Place code to compare for additional address here    */
                    /* `#START LED_Driver1_SW_ADDR_COMPARE_interruptStart` */

                    /* `#END` */

                #ifdef LED_Driver1_SW_ADDR_COMPARE_ENTRY_CALLBACK
                    LED_Driver1_SwAddrCompare_EntryCallback();
                #endif /* LED_Driver1_SW_ADDR_COMPARE_ENTRY_CALLBACK */
                    
                    LED_Driver1_NAK_AND_RECEIVE;   /* NACK address */

                    /* Place code to end of condition for NACK generation here */
                    /* `#START LED_Driver1_SW_ADDR_COMPARE_interruptEnd`  */

                    /* `#END` */

                #ifdef LED_Driver1_SW_ADDR_COMPARE_EXIT_CALLBACK
                    LED_Driver1_SwAddrCompare_ExitCallback();
                #endif /* LED_Driver1_SW_ADDR_COMPARE_EXIT_CALLBACK */
                }

            #else /* (LED_Driver1_HW_ADRR_DECODE) */

                if(0u != (LED_Driver1_DATA_REG & LED_Driver1_READ_FLAG))
                {
                    /* Place code to prepare read buffer here                  */
                    /* `#START LED_Driver1_HW_PREPARE_READ_BUF_interrupt` */

                    /* `#END` */
                    
                #ifdef LED_Driver1_HW_PREPARE_READ_BUF_CALLBACK
                    LED_Driver1_HwPrepareReadBuf_Callback();
                #endif /* LED_Driver1_HW_PREPARE_READ_BUF_CALLBACK */

                    /* Prepare next operation to read, get data and place in data register */
                    if(LED_Driver1_slRdBufIndex < LED_Driver1_slRdBufSize)
                    {
                        /* Load first data byte from array */
                        LED_Driver1_DATA_REG = LED_Driver1_slRdBufPtr[LED_Driver1_slRdBufIndex];
                        LED_Driver1_ACK_AND_TRANSMIT;
                        LED_Driver1_slRdBufIndex++;

                        LED_Driver1_slStatus |= LED_Driver1_SSTAT_RD_BUSY;
                    }
                    else    /* Overflow: provide 0xFF on bus */
                    {
                        LED_Driver1_DATA_REG = LED_Driver1_OVERFLOW_RETURN;
                        LED_Driver1_ACK_AND_TRANSMIT;

                        LED_Driver1_slStatus  |= (LED_Driver1_SSTAT_RD_BUSY |
                                                       LED_Driver1_SSTAT_RD_ERR_OVFL);
                    }

                    LED_Driver1_state = LED_Driver1_SM_SL_RD_DATA;
                }
                else  /* Write transaction: receive 1st byte */
                {
                    LED_Driver1_ACK_AND_RECEIVE;
                    LED_Driver1_state = LED_Driver1_SM_SL_WR_DATA;

                    LED_Driver1_slStatus |= LED_Driver1_SSTAT_WR_BUSY;
                    LED_Driver1_ENABLE_INT_ON_STOP;
                }

            #endif /* (LED_Driver1_SW_ADRR_DECODE) */
            }
            /* Data states */
            /* Data master writes into slave */
            else if(LED_Driver1_state == LED_Driver1_SM_SL_WR_DATA)
            {
                if(LED_Driver1_slWrBufIndex < LED_Driver1_slWrBufSize)
                {
                    tmp8 = LED_Driver1_DATA_REG;
                    LED_Driver1_ACK_AND_RECEIVE;
                    LED_Driver1_slWrBufPtr[LED_Driver1_slWrBufIndex] = tmp8;
                    LED_Driver1_slWrBufIndex++;
                }
                else  /* of array: complete write, send NACK */
                {
                    LED_Driver1_NAK_AND_RECEIVE;

                    LED_Driver1_slStatus |= LED_Driver1_SSTAT_WR_ERR_OVFL;
                }
            }
            /* Data master reads from slave */
            else if(LED_Driver1_state == LED_Driver1_SM_SL_RD_DATA)
            {
                if(LED_Driver1_CHECK_DATA_ACK(tmpCsr))
                {
                    if(LED_Driver1_slRdBufIndex < LED_Driver1_slRdBufSize)
                    {
                         /* Get data from array */
                        LED_Driver1_DATA_REG = LED_Driver1_slRdBufPtr[LED_Driver1_slRdBufIndex];
                        LED_Driver1_TRANSMIT_DATA;
                        LED_Driver1_slRdBufIndex++;
                    }
                    else   /* Overflow: provide 0xFF on bus */
                    {
                        LED_Driver1_DATA_REG = LED_Driver1_OVERFLOW_RETURN;
                        LED_Driver1_TRANSMIT_DATA;

                        LED_Driver1_slStatus |= LED_Driver1_SSTAT_RD_ERR_OVFL;
                    }
                }
                else  /* Last byte was NACKed: read complete */
                {
                    /* Only NACK appears on bus */
                    LED_Driver1_DATA_REG = LED_Driver1_OVERFLOW_RETURN;
                    LED_Driver1_NAK_AND_TRANSMIT;

                    LED_Driver1_slStatus &= ((uint8) ~LED_Driver1_SSTAT_RD_BUSY);
                    LED_Driver1_slStatus |= ((uint8)  LED_Driver1_SSTAT_RD_CMPLT);

                    LED_Driver1_state = LED_Driver1_SM_IDLE;
                }
            }
            else
            {
            #if(LED_Driver1_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handle this case */
                LED_Driver1_DisableInt();
                LED_Driver1_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (LED_Driver1_TIMEOUT_ENABLED) */
            }
        }
    #endif /* (LED_Driver1_MODE_SLAVE_ENABLED) */
    }
    else
    {
        /* The FSM skips master and slave processing: return to IDLE */
        LED_Driver1_state = LED_Driver1_SM_IDLE;
    }

#ifdef LED_Driver1_ISR_EXIT_CALLBACK
    LED_Driver1_ISR_ExitCallback();
#endif /* LED_Driver1_ISR_EXIT_CALLBACK */    
}


#if ((LED_Driver1_FF_IMPLEMENTED) && (LED_Driver1_WAKEUP_ENABLED))
    /*******************************************************************************
    * Function Name: LED_Driver1_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(LED_Driver1_WAKEUP_ISR)
    {
    #ifdef LED_Driver1_WAKEUP_ISR_ENTRY_CALLBACK
        LED_Driver1_WAKEUP_ISR_EntryCallback();
    #endif /* LED_Driver1_WAKEUP_ISR_ENTRY_CALLBACK */
         
        /* Set flag to notify that matched address is received */
        LED_Driver1_wakeupSource = 1u;

        /* SCL is stretched until the I2C_Wake() is called */

    #ifdef LED_Driver1_WAKEUP_ISR_EXIT_CALLBACK
        LED_Driver1_WAKEUP_ISR_ExitCallback();
    #endif /* LED_Driver1_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* ((LED_Driver1_FF_IMPLEMENTED) && (LED_Driver1_WAKEUP_ENABLED)) */


/* [] END OF FILE */
