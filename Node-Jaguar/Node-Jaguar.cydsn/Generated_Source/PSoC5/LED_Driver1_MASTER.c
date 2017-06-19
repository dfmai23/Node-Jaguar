/*******************************************************************************
* File Name: LED_Driver1_MASTER.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component master mode.
*
*******************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LED_Driver1_PVT.h"

#if(LED_Driver1_MODE_MASTER_ENABLED)

/**********************************
*      System variables
**********************************/

volatile uint8 LED_Driver1_mstrStatus;     /* Master Status byte  */
volatile uint8 LED_Driver1_mstrControl;    /* Master Control byte */

/* Transmit buffer variables */
volatile uint8 * LED_Driver1_mstrRdBufPtr;     /* Pointer to Master Read buffer */
volatile uint8   LED_Driver1_mstrRdBufSize;    /* Master Read buffer size       */
volatile uint8   LED_Driver1_mstrRdBufIndex;   /* Master Read buffer Index      */

/* Receive buffer variables */
volatile uint8 * LED_Driver1_mstrWrBufPtr;     /* Pointer to Master Write buffer */
volatile uint8   LED_Driver1_mstrWrBufSize;    /* Master Write buffer size       */
volatile uint8   LED_Driver1_mstrWrBufIndex;   /* Master Write buffer Index      */


/*******************************************************************************
* Function Name: LED_Driver1_MasterWriteBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer of data to be sent.
*  cnt:       Size of buffer to send.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after a start or restart condition is
*  generated.
*
* Global variables:
*  LED_Driver1_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  LED_Driver1_state       - The global variable used to store a current
*                                 state of the software FSM.
*  LED_Driver1_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without Stop
*                                 generation.
*  LED_Driver1_mstrWrBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  LED_Driver1_mstrWrBufIndex - The global variable used to store current
*                                    index within the master write buffer.
*  LED_Driver1_mstrWrBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 LED_Driver1_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode)
      
{
    uint8 errStatus;

    errStatus = LED_Driver1_MSTR_NOT_READY;

    if(NULL != wrData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(LED_Driver1_SM_IDLE == LED_Driver1_state)
        {
            /* Master is ready for transaction: check if bus is free */
            if(LED_Driver1_CHECK_BUS_FREE(LED_Driver1_MCSR_REG))
            {
                errStatus = LED_Driver1_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = LED_Driver1_MSTR_BUS_BUSY;
            }
        }
        else if(LED_Driver1_SM_MSTR_HALT == LED_Driver1_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = LED_Driver1_MSTR_NO_ERROR;

            LED_Driver1_ClearPendingInt();
            LED_Driver1_mstrStatus &= (uint8) ~LED_Driver1_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = LED_Driver1_MSTR_NOT_READY was send before */
        }

        if(LED_Driver1_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            LED_Driver1_state = LED_Driver1_SM_MSTR_WR_ADDR;

            /* Prepare write buffer */
            LED_Driver1_mstrWrBufIndex = 0u;
            LED_Driver1_mstrWrBufSize  = cnt;
            LED_Driver1_mstrWrBufPtr   = (volatile uint8 *) wrData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            LED_Driver1_mstrControl = mode;

            /* Clear write status history */
            LED_Driver1_mstrStatus &= (uint8) ~LED_Driver1_MSTAT_WR_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            LED_Driver1_DATA_REG = (uint8) (slaveAddress << LED_Driver1_SLAVE_ADDR_SHIFT);

            if(LED_Driver1_CHECK_RESTART(mode))
            {
                LED_Driver1_GENERATE_RESTART;
            }
            else
            {
                LED_Driver1_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            LED_Driver1_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterReadBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer where to put data from slave.
*  cnt:       Size of buffer to read.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after start or restart condition is
*  generated.
*
* Global variables:
*  LED_Driver1_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  LED_Driver1_state       - The global variable used to store a current
*                                 state of the software FSM.
*  LED_Driver1_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without
*                                 Stop generation.
*  LED_Driver1_mstrRdBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  LED_Driver1_mstrRdBufIndex - The global variable  used to store a
*                                    current index within the master
*                                    write buffer.
*  LED_Driver1_mstrRdBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LED_Driver1_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode)
      
{
    uint8 errStatus;

    errStatus = LED_Driver1_MSTR_NOT_READY;

    if(NULL != rdData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(LED_Driver1_SM_IDLE == LED_Driver1_state)
        {
            /* Master is ready to transaction: check if bus is free */
            if(LED_Driver1_CHECK_BUS_FREE(LED_Driver1_MCSR_REG))
            {
                errStatus = LED_Driver1_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = LED_Driver1_MSTR_BUS_BUSY;
            }
        }
        else if(LED_Driver1_SM_MSTR_HALT == LED_Driver1_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = LED_Driver1_MSTR_NO_ERROR;

            LED_Driver1_ClearPendingInt();
            LED_Driver1_mstrStatus &= (uint8) ~LED_Driver1_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = LED_Driver1_MSTR_NOT_READY was set before */
        }

        if(LED_Driver1_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            LED_Driver1_state = LED_Driver1_SM_MSTR_RD_ADDR;

            /* Prepare read buffer */
            LED_Driver1_mstrRdBufIndex  = 0u;
            LED_Driver1_mstrRdBufSize   = cnt;
            LED_Driver1_mstrRdBufPtr    = (volatile uint8 *) rdData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            LED_Driver1_mstrControl = mode;

            /* Clear read status history */
            LED_Driver1_mstrStatus &= (uint8) ~LED_Driver1_MSTAT_RD_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            LED_Driver1_DATA_REG = ((uint8) (slaveAddress << LED_Driver1_SLAVE_ADDR_SHIFT) |
                                                  LED_Driver1_READ_FLAG);

            if(LED_Driver1_CHECK_RESTART(mode))
            {
                LED_Driver1_GENERATE_RESTART;
            }
            else
            {
                LED_Driver1_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            LED_Driver1_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterSendStart
********************************************************************************
*
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  LED_Driver1_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LED_Driver1_MasterSendStart(uint8 slaveAddress, uint8 R_nW)
      
{
    uint8 errStatus;

    errStatus = LED_Driver1_MSTR_NOT_READY;

    /* If IDLE, check if bus is free */
    if(LED_Driver1_SM_IDLE == LED_Driver1_state)
    {
        /* If bus is free, generate Start condition */
        if(LED_Driver1_CHECK_BUS_FREE(LED_Driver1_MCSR_REG))
        {
            /* Disable interrupt for manual master operation */
            LED_Driver1_DisableInt();

            /* Set address and read/write flag */
            slaveAddress = (uint8) (slaveAddress << LED_Driver1_SLAVE_ADDR_SHIFT);
            if(0u != R_nW)
            {
                slaveAddress |= LED_Driver1_READ_FLAG;
                LED_Driver1_state = LED_Driver1_SM_MSTR_RD_ADDR;
            }
            else
            {
                LED_Driver1_state = LED_Driver1_SM_MSTR_WR_ADDR;
            }

            /* Hardware actions: write address and generate Start */
            LED_Driver1_DATA_REG = slaveAddress;
            LED_Driver1_GENERATE_START_MANUAL;

            /* Wait until address is transferred */
            while(LED_Driver1_WAIT_BYTE_COMPLETE(LED_Driver1_CSR_REG))
            {
            }

        #if(LED_Driver1_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(LED_Driver1_CHECK_START_GEN(LED_Driver1_MCSR_REG))
            {
                LED_Driver1_CLEAR_START_GEN;

                /* Start condition was not generated: reset FSM to IDLE */
                LED_Driver1_state = LED_Driver1_SM_IDLE;
                errStatus = LED_Driver1_MSTR_ERR_ABORT_START_GEN;
            }
            else
        #endif /* (LED_Driver1_MODE_MULTI_MASTER_SLAVE_ENABLED) */

        #if(LED_Driver1_MODE_MULTI_MASTER_ENABLED)
            if(LED_Driver1_CHECK_LOST_ARB(LED_Driver1_CSR_REG))
            {
                LED_Driver1_BUS_RELEASE_MANUAL;

                /* Master lost arbitrage: reset FSM to IDLE */
                LED_Driver1_state = LED_Driver1_SM_IDLE;
                errStatus = LED_Driver1_MSTR_ERR_ARB_LOST;
            }
            else
        #endif /* (LED_Driver1_MODE_MULTI_MASTER_ENABLED) */

            if(LED_Driver1_CHECK_ADDR_NAK(LED_Driver1_CSR_REG))
            {
                /* Address has been NACKed: reset FSM to IDLE */
                LED_Driver1_state = LED_Driver1_SM_IDLE;
                errStatus = LED_Driver1_MSTR_ERR_LB_NAK;
            }
            else
            {
                /* Start was sent without errors */
                errStatus = LED_Driver1_MSTR_NO_ERROR;
            }
        }
        else
        {
            errStatus = LED_Driver1_MSTR_BUS_BUSY;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterSendRestart
********************************************************************************
*
* Summary:
*  Generates ReStart condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  LED_Driver1_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LED_Driver1_MasterSendRestart(uint8 slaveAddress, uint8 R_nW)
      
{
    uint8 errStatus;

    errStatus = LED_Driver1_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(LED_Driver1_CHECK_MASTER_MODE(LED_Driver1_MCSR_REG))
    {
        /* Set address and read/write flag */
        slaveAddress = (uint8) (slaveAddress << LED_Driver1_SLAVE_ADDR_SHIFT);
        if(0u != R_nW)
        {
            slaveAddress |= LED_Driver1_READ_FLAG;
            LED_Driver1_state = LED_Driver1_SM_MSTR_RD_ADDR;
        }
        else
        {
            LED_Driver1_state = LED_Driver1_SM_MSTR_WR_ADDR;
        }

        /* Hardware actions: write address and generate ReStart */
        LED_Driver1_DATA_REG = slaveAddress;
        LED_Driver1_GENERATE_RESTART_MANUAL;

        /* Wait until address has been transferred */
        while(LED_Driver1_WAIT_BYTE_COMPLETE(LED_Driver1_CSR_REG))
        {
        }

    #if(LED_Driver1_MODE_MULTI_MASTER_ENABLED)
        if(LED_Driver1_CHECK_LOST_ARB(LED_Driver1_CSR_REG))
        {
            LED_Driver1_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            LED_Driver1_state = LED_Driver1_SM_IDLE;
            errStatus = LED_Driver1_MSTR_ERR_ARB_LOST;
        }
        else
    #endif /* (LED_Driver1_MODE_MULTI_MASTER_ENABLED) */

        if(LED_Driver1_CHECK_ADDR_NAK(LED_Driver1_CSR_REG))
        {
            /* Address has been NACKed: reset FSM to IDLE */
            LED_Driver1_state = LED_Driver1_SM_IDLE;
            errStatus = LED_Driver1_MSTR_ERR_LB_NAK;
        }
        else
        {
            /* ReStart was sent without errors */
            errStatus = LED_Driver1_MSTR_NO_ERROR;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterSendStop
********************************************************************************
*
* Summary:
*  Generates I2C Stop condition on bus. Function do nothing if Start or Restart
*  condition was failed before call this function.
*
* Parameters:
*  None.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  Stop generation is required to complete the transaction.
*  This function does not wait until a Stop condition is generated.
*
* Global variables:
*  LED_Driver1_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LED_Driver1_MasterSendStop(void) 
{
    uint8 errStatus;

    errStatus = LED_Driver1_MSTR_NOT_READY;

    /* Check if master is active on bus */
    if(LED_Driver1_CHECK_MASTER_MODE(LED_Driver1_MCSR_REG))
    {
        LED_Driver1_GENERATE_STOP_MANUAL;
        LED_Driver1_state = LED_Driver1_SM_IDLE;

        /* Wait until stop has been generated */
        while(LED_Driver1_WAIT_STOP_COMPLETE(LED_Driver1_CSR_REG))
        {
        }

        errStatus = LED_Driver1_MSTR_NO_ERROR;

    #if(LED_Driver1_MODE_MULTI_MASTER_ENABLED)
        if(LED_Driver1_CHECK_LOST_ARB(LED_Driver1_CSR_REG))
        {
            LED_Driver1_BUS_RELEASE_MANUAL;

            /* NACK was generated by instead Stop */
            errStatus = LED_Driver1_MSTR_ERR_ARB_LOST;
        }
    #endif /* (LED_Driver1_MODE_MULTI_MASTER_ENABLED) */
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave. A valid Start or ReStart condition must be
*  generated before this call this function. Function do nothing if Start or
*  Restart condition was failed before call this function.
*
* Parameters:
*  data:  The data byte to send to the slave.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  LED_Driver1_state - The global variable used to store a current state of
*                           the software FSM.
*
*******************************************************************************/
uint8 LED_Driver1_MasterWriteByte(uint8 theByte) 
{
    uint8 errStatus;

    errStatus = LED_Driver1_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(LED_Driver1_CHECK_MASTER_MODE(LED_Driver1_MCSR_REG))
    {
        LED_Driver1_DATA_REG = theByte;   /* Write DATA register */
        LED_Driver1_TRANSMIT_DATA_MANUAL; /* Set transmit mode   */
        LED_Driver1_state = LED_Driver1_SM_MSTR_WR_DATA;

        /* Wait until data byte has been transmitted */
        while(LED_Driver1_WAIT_BYTE_COMPLETE(LED_Driver1_CSR_REG))
        {
        }

    #if(LED_Driver1_MODE_MULTI_MASTER_ENABLED)
        if(LED_Driver1_CHECK_LOST_ARB(LED_Driver1_CSR_REG))
        {
            LED_Driver1_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            LED_Driver1_state = LED_Driver1_SM_IDLE;
            errStatus = LED_Driver1_MSTR_ERR_ARB_LOST;
        }
        /* Check LRB bit */
        else
    #endif /* (LED_Driver1_MODE_MULTI_MASTER_ENABLED) */

        if(LED_Driver1_CHECK_DATA_ACK(LED_Driver1_CSR_REG))
        {
            LED_Driver1_state = LED_Driver1_SM_MSTR_HALT;
            errStatus = LED_Driver1_MSTR_NO_ERROR;
        }
        else
        {
            LED_Driver1_state = LED_Driver1_SM_MSTR_HALT;
            errStatus = LED_Driver1_MSTR_ERR_LB_NAK;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACK or NACK the transfer. A valid Start or
*  ReStart condition must be generated before this call this function. Function
*  do nothing if Start or Restart condition was failed before call this
*  function.
*
* Parameters:
*  acknNack:  Zero, response with NACK, if non-zero response with ACK.
*
* Return:
*  Byte read from slave.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  LED_Driver1_state - The global variable used to store a current
*                           state of the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LED_Driver1_MasterReadByte(uint8 acknNak) 
{
    uint8 theByte;

    theByte = 0u;

    /* Check if START condition was generated */
    if(LED_Driver1_CHECK_MASTER_MODE(LED_Driver1_MCSR_REG))
    {
        /* When address phase needs to release bus and receive byte,
        * then decide ACK or NACK
        */
        if(LED_Driver1_SM_MSTR_RD_ADDR == LED_Driver1_state)
        {
            LED_Driver1_READY_TO_READ_MANUAL;
            LED_Driver1_state = LED_Driver1_SM_MSTR_RD_DATA;
        }

        /* Wait until data byte has been received */
        while(LED_Driver1_WAIT_BYTE_COMPLETE(LED_Driver1_CSR_REG))
        {
        }

        theByte = LED_Driver1_DATA_REG;

        /* Command ACK to receive next byte and continue transfer.
        *  Do nothing for NACK. The NACK will be generated by
        *  Stop or ReStart routine.
        */
        if(acknNak != 0u) /* Generate ACK */
        {
            LED_Driver1_ACK_AND_RECEIVE_MANUAL;
        }
        else              /* Do nothing for the follwong NACK */
        {
            LED_Driver1_state = LED_Driver1_SM_MSTR_HALT;
        }
    }

    return(theByte);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterStatus
********************************************************************************
*
* Summary:
*  Returns the master's communication status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  LED_Driver1_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
*******************************************************************************/
uint8 LED_Driver1_MasterStatus(void) 
{
    uint8 status;

    LED_Driver1_DisableInt(); /* Lock from interrupt */

    /* Read master status */
    status = LED_Driver1_mstrStatus;

    if (LED_Driver1_CHECK_SM_MASTER)
    {
        /* Set transfer in progress flag in status */
        status |= LED_Driver1_MSTAT_XFER_INP;
    }

    LED_Driver1_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterClearStatus
********************************************************************************
*
* Summary:
*  Clears all status flags and returns the master status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  LED_Driver1_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LED_Driver1_MasterClearStatus(void) 
{
    uint8 status;

    LED_Driver1_DisableInt(); /* Lock from interrupt */

    /* Read and clear master status */
    status = LED_Driver1_mstrStatus;
    LED_Driver1_mstrStatus = LED_Driver1_MSTAT_CLEAR;

    LED_Driver1_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterGetReadBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterReadBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  LED_Driver1_mstrRdBufIndex - The global variable stores current index
*                                    within the master read buffer.
*
*******************************************************************************/
uint8 LED_Driver1_MasterGetReadBufSize(void) 
{
    return (LED_Driver1_mstrRdBufIndex);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterGetWriteBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterWriteBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  LED_Driver1_mstrWrBufIndex -  The global variable used to stores current
*                                     index within master write buffer.
*
*******************************************************************************/
uint8 LED_Driver1_MasterGetWriteBufSize(void) 
{
    return (LED_Driver1_mstrWrBufIndex);
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterClearReadBuf
********************************************************************************
*
* Summary:
*  Resets the read buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LED_Driver1_mstrRdBufIndex - The global variable used to stores current
*                                    index within master read buffer.
*  LED_Driver1_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LED_Driver1_MasterClearReadBuf(void) 
{
    LED_Driver1_DisableInt(); /* Lock from interrupt */

    LED_Driver1_mstrRdBufIndex = 0u;
    LED_Driver1_mstrStatus    &= (uint8) ~LED_Driver1_MSTAT_RD_CMPLT;

    LED_Driver1_EnableInt(); /* Release lock */
}


/*******************************************************************************
* Function Name: LED_Driver1_MasterClearWriteBuf
********************************************************************************
*
* Summary:
*  Resets the write buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LED_Driver1_mstrRdBufIndex - The global variable used to stote current
*                                    index within master read buffer.
*  LED_Driver1_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LED_Driver1_MasterClearWriteBuf(void) 
{
    LED_Driver1_DisableInt(); /* Lock from interrupt */

    LED_Driver1_mstrWrBufIndex = 0u;
    LED_Driver1_mstrStatus    &= (uint8) ~LED_Driver1_MSTAT_WR_CMPLT;

    LED_Driver1_EnableInt(); /* Release lock */
}

#endif /* (LED_Driver1_MODE_MASTER_ENABLED) */


/* [] END OF FILE */
