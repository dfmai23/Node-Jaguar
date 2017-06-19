/*******************************************************************************
* File Name: LED_Driver2.h
* Version 3.50
*
* Description:
*  This file provides constants and parameter values for the I2C component.

*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_LED_Driver2_H)
#define CY_I2C_LED_Driver2_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"

/* Check if required defines such as CY_PSOC5LP are available in cy_boot */
#if !defined (CY_PSOC5LP)
    #error Component I2C_v3_50 requires cy_boot v3.10 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*   Conditional Compilation Parameters
****************************************/

#define LED_Driver2_IMPLEMENTATION     (0u)
#define LED_Driver2_MODE               (2u)
#define LED_Driver2_ENABLE_WAKEUP      (0u)
#define LED_Driver2_ADDR_DECODE        (1u)
#define LED_Driver2_UDB_INTRN_CLOCK    (0u)


/* I2C implementation enum */
#define LED_Driver2_UDB    (0x00u)
#define LED_Driver2_FF     (0x01u)

#define LED_Driver2_FF_IMPLEMENTED     (LED_Driver2_FF  == LED_Driver2_IMPLEMENTATION)
#define LED_Driver2_UDB_IMPLEMENTED    (LED_Driver2_UDB == LED_Driver2_IMPLEMENTATION)

#define LED_Driver2_UDB_INTRN_CLOCK_ENABLED    (LED_Driver2_UDB_IMPLEMENTED && \
                                                     (0u != LED_Driver2_UDB_INTRN_CLOCK))
/* I2C modes enum */
#define LED_Driver2_MODE_SLAVE                 (0x01u)
#define LED_Driver2_MODE_MASTER                (0x02u)
#define LED_Driver2_MODE_MULTI_MASTER          (0x06u)
#define LED_Driver2_MODE_MULTI_MASTER_SLAVE    (0x07u)
#define LED_Driver2_MODE_MULTI_MASTER_MASK     (0x04u)

#define LED_Driver2_MODE_SLAVE_ENABLED         (0u != (LED_Driver2_MODE_SLAVE  & LED_Driver2_MODE))
#define LED_Driver2_MODE_MASTER_ENABLED        (0u != (LED_Driver2_MODE_MASTER & LED_Driver2_MODE))
#define LED_Driver2_MODE_MULTI_MASTER_ENABLED  (0u != (LED_Driver2_MODE_MULTI_MASTER_MASK & \
                                                            LED_Driver2_MODE))
#define LED_Driver2_MODE_MULTI_MASTER_SLAVE_ENABLED    (LED_Driver2_MODE_MULTI_MASTER_SLAVE == \
                                                             LED_Driver2_MODE)

/* Address detection enum */
#define LED_Driver2_SW_DECODE      (0x00u)
#define LED_Driver2_HW_DECODE      (0x01u)

#define LED_Driver2_SW_ADRR_DECODE             (LED_Driver2_SW_DECODE == LED_Driver2_ADDR_DECODE)
#define LED_Driver2_HW_ADRR_DECODE             (LED_Driver2_HW_DECODE == LED_Driver2_ADDR_DECODE)

/* Wakeup enabled */
#define LED_Driver2_WAKEUP_ENABLED             (0u != LED_Driver2_ENABLE_WAKEUP)

/* Adds bootloader APIs to component */
#define LED_Driver2_BOOTLOADER_INTERFACE_ENABLED   (LED_Driver2_MODE_SLAVE_ENABLED && \
                                                            ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LED_Driver2) || \
                                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface)))

/* Timeout functionality */
#define LED_Driver2_TIMEOUT_ENABLE             (0u)
#define LED_Driver2_TIMEOUT_SCL_TMOUT_ENABLE   (0u)
#define LED_Driver2_TIMEOUT_SDA_TMOUT_ENABLE   (0u)
#define LED_Driver2_TIMEOUT_PRESCALER_ENABLE   (0u)
#define LED_Driver2_TIMEOUT_IMPLEMENTATION     (0u)

/* Convert to boolean */
#define LED_Driver2_TIMEOUT_ENABLED            (0u != LED_Driver2_TIMEOUT_ENABLE)
#define LED_Driver2_TIMEOUT_SCL_TMOUT_ENABLED  (0u != LED_Driver2_TIMEOUT_SCL_TMOUT_ENABLE)
#define LED_Driver2_TIMEOUT_SDA_TMOUT_ENABLED  (0u != LED_Driver2_TIMEOUT_SDA_TMOUT_ENABLE)
#define LED_Driver2_TIMEOUT_PRESCALER_ENABLED  (0u != LED_Driver2_TIMEOUT_PRESCALER_ENABLE)

/* Timeout implementation enum. */
#define LED_Driver2_TIMEOUT_UDB    (0x00u)
#define LED_Driver2_TIMEOUT_FF     (0x01u)

#define LED_Driver2_TIMEOUT_FF_IMPLEMENTED     (LED_Driver2_TIMEOUT_FF  == \
                                                        LED_Driver2_TIMEOUT_IMPLEMENTATION)
#define LED_Driver2_TIMEOUT_UDB_IMPLEMENTED    (LED_Driver2_TIMEOUT_UDB == \
                                                        LED_Driver2_TIMEOUT_IMPLEMENTATION)

#define LED_Driver2_TIMEOUT_FF_ENABLED         (LED_Driver2_TIMEOUT_ENABLED && \
                                                     LED_Driver2_TIMEOUT_FF_IMPLEMENTED)

#define LED_Driver2_TIMEOUT_UDB_ENABLED        (LED_Driver2_TIMEOUT_ENABLED && \
                                                     LED_Driver2_TIMEOUT_UDB_IMPLEMENTED)

#define LED_Driver2_EXTERN_I2C_INTR_HANDLER    (0u)
#define LED_Driver2_EXTERN_TMOUT_INTR_HANDLER  (0u)

#define LED_Driver2_INTERN_I2C_INTR_HANDLER    (0u == LED_Driver2_EXTERN_I2C_INTR_HANDLER)
#define LED_Driver2_INTERN_TMOUT_INTR_HANDLER  (0u == LED_Driver2_EXTERN_TMOUT_INTR_HANDLER)


/***************************************
*       Type defines
***************************************/

/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;

#if (LED_Driver2_FF_IMPLEMENTED)
    uint8 xcfg;
    uint8 cfg;
    uint8 addr;
    uint8 clkDiv1;
    uint8 clkDiv2;
#else
    uint8 control;
#endif /* (LED_Driver2_FF_IMPLEMENTED) */

#if (LED_Driver2_TIMEOUT_ENABLED)
    uint16 tmoutCfg;
    uint8  tmoutIntr;
#endif /* (LED_Driver2_TIMEOUT_ENABLED) */

} LED_Driver2_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void LED_Driver2_Init(void)                            ;
void LED_Driver2_Enable(void)                          ;

void LED_Driver2_Start(void)                           ;
void LED_Driver2_Stop(void)                            ;

#define LED_Driver2_EnableInt()        CyIntEnable      (LED_Driver2_ISR_NUMBER)
#define LED_Driver2_DisableInt()       CyIntDisable     (LED_Driver2_ISR_NUMBER)
#define LED_Driver2_ClearPendingInt()  CyIntClearPending(LED_Driver2_ISR_NUMBER)
#define LED_Driver2_SetPendingInt()    CyIntSetPending  (LED_Driver2_ISR_NUMBER)

void LED_Driver2_SaveConfig(void)                      ;
void LED_Driver2_Sleep(void)                           ;
void LED_Driver2_RestoreConfig(void)                   ;
void LED_Driver2_Wakeup(void)                          ;

/* I2C Master functions prototypes */
#if (LED_Driver2_MODE_MASTER_ENABLED)
    /* Read and Clear status functions */
    uint8 LED_Driver2_MasterStatus(void)                ;
    uint8 LED_Driver2_MasterClearStatus(void)           ;

    /* Interrupt based operation functions */
    uint8 LED_Driver2_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode) \
                                                            ;
    uint8 LED_Driver2_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode) \
                                                            ;
    uint8 LED_Driver2_MasterGetReadBufSize(void)       ;
    uint8 LED_Driver2_MasterGetWriteBufSize(void)      ;
    void  LED_Driver2_MasterClearReadBuf(void)         ;
    void  LED_Driver2_MasterClearWriteBuf(void)        ;

    /* Manual operation functions */
    uint8 LED_Driver2_MasterSendStart(uint8 slaveAddress, uint8 R_nW) \
                                                            ;
    uint8 LED_Driver2_MasterSendRestart(uint8 slaveAddress, uint8 R_nW) \
                                                            ;
    uint8 LED_Driver2_MasterSendStop(void)             ;
    uint8 LED_Driver2_MasterWriteByte(uint8 theByte)   ;
    uint8 LED_Driver2_MasterReadByte(uint8 acknNak)    ;

#endif /* (LED_Driver2_MODE_MASTER_ENABLED) */

/* I2C Slave functions prototypes */
#if (LED_Driver2_MODE_SLAVE_ENABLED)
    /* Read and Clear status functions */
    uint8 LED_Driver2_SlaveStatus(void)                ;
    uint8 LED_Driver2_SlaveClearReadStatus(void)       ;
    uint8 LED_Driver2_SlaveClearWriteStatus(void)      ;

    void  LED_Driver2_SlaveSetAddress(uint8 address)   ;

    /* Interrupt based operation functions */
    void  LED_Driver2_SlaveInitReadBuf(uint8 * rdBuf, uint8 bufSize) \
                                                            ;
    void  LED_Driver2_SlaveInitWriteBuf(uint8 * wrBuf, uint8 bufSize) \
                                                            ;
    uint8 LED_Driver2_SlaveGetReadBufSize(void)        ;
    uint8 LED_Driver2_SlaveGetWriteBufSize(void)       ;
    void  LED_Driver2_SlaveClearReadBuf(void)          ;
    void  LED_Driver2_SlaveClearWriteBuf(void)         ;

    /* Communication bootloader I2C Slave APIs */
    #if defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver2_BOOTLOADER_INTERFACE_ENABLED)
        /* Physical layer functions */
        void     LED_Driver2_CyBtldrCommStart(void) CYSMALL \
                                                            ;
        void     LED_Driver2_CyBtldrCommStop(void)  CYSMALL \
                                                            ;
        void     LED_Driver2_CyBtldrCommReset(void) CYSMALL \
                                                            ;
        cystatus LED_Driver2_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) \
                                                        CYSMALL ;
        cystatus LED_Driver2_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)  CYSMALL \
                                                            ;

        #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LED_Driver2)
            #define CyBtldrCommStart    LED_Driver2_CyBtldrCommStart
            #define CyBtldrCommStop     LED_Driver2_CyBtldrCommStop
            #define CyBtldrCommReset    LED_Driver2_CyBtldrCommReset
            #define CyBtldrCommWrite    LED_Driver2_CyBtldrCommWrite
            #define CyBtldrCommRead     LED_Driver2_CyBtldrCommRead
        #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LED_Driver2) */

        /* Size of Read/Write buffers for I2C bootloader  */
        #define LED_Driver2_BTLDR_SIZEOF_READ_BUFFER   (0x80u)
        #define LED_Driver2_BTLDR_SIZEOF_WRITE_BUFFER  (0x80u)
        #define LED_Driver2_MIN_UNT16(a, b)            ( ((uint16)(a) < (b)) ? ((uint16) (a)) : ((uint16) (b)) )
        #define LED_Driver2_WAIT_1_MS                  (1u)

    #endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver2_BOOTLOADER_INTERFACE_ENABLED) */

#endif /* (LED_Driver2_MODE_SLAVE_ENABLED) */

/* Component interrupt handlers */
CY_ISR_PROTO(LED_Driver2_ISR);
#if ((LED_Driver2_FF_IMPLEMENTED) || (LED_Driver2_WAKEUP_ENABLED))
    CY_ISR_PROTO(LED_Driver2_WAKEUP_ISR);
#endif /* ((LED_Driver2_FF_IMPLEMENTED) || (LED_Driver2_WAKEUP_ENABLED)) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8 LED_Driver2_initVar;


/***************************************
*   Initial Parameter Constants
***************************************/

#define LED_Driver2_DATA_RATE          (100u)
#define LED_Driver2_DEFAULT_ADDR       (8u)
#define LED_Driver2_I2C_PAIR_SELECTED  (0u)

/* I2C pair enum */
#define LED_Driver2_I2C_PAIR_ANY   (0x01u) /* Any pins for I2C */
#define LED_Driver2_I2C_PAIR0      (0x01u) /* I2C0: (SCL = P12[4]) && (SCL = P12[5]) */
#define LED_Driver2_I2C_PAIR1      (0x02u) /* I2C1: (SCL = P12[0]) && (SDA = P12[1]) */

#define LED_Driver2_I2C1_SIO_PAIR  (LED_Driver2_I2C_PAIR1 == LED_Driver2_I2C_PAIR_SELECTED)
#define LED_Driver2_I2C0_SIO_PAIR  (LED_Driver2_I2C_PAIR0 == LED_Driver2_I2C_PAIR_SELECTED)


/***************************************
*            API Constants
***************************************/

/* Master/Slave control constants */
#define LED_Driver2_READ_XFER_MODE     (0x01u) /* Read */
#define LED_Driver2_WRITE_XFER_MODE    (0x00u) /* Write */
#define LED_Driver2_ACK_DATA           (0x01u) /* Send ACK */
#define LED_Driver2_NAK_DATA           (0x00u) /* Send NAK */
#define LED_Driver2_OVERFLOW_RETURN    (0xFFu) /* Send on bus in case of overflow */

#if (LED_Driver2_MODE_MASTER_ENABLED)
    /* "Mode" constants for MasterWriteBuf() or MasterReadBuf() function */
    #define LED_Driver2_MODE_COMPLETE_XFER     (0x00u) /* Full transfer with Start and Stop */
    #define LED_Driver2_MODE_REPEAT_START      (0x01u) /* Begin with a ReStart instead of a Start */
    #define LED_Driver2_MODE_NO_STOP           (0x02u) /* Complete the transfer without a Stop */

    /* Master status */
    #define LED_Driver2_MSTAT_CLEAR            (0x00u) /* Clear (initialize) status value */

    #define LED_Driver2_MSTAT_RD_CMPLT         (0x01u) /* Read complete */
    #define LED_Driver2_MSTAT_WR_CMPLT         (0x02u) /* Write complete */
    #define LED_Driver2_MSTAT_XFER_INP         (0x04u) /* Master transfer in progress */
    #define LED_Driver2_MSTAT_XFER_HALT        (0x08u) /* Transfer is halted */

    #define LED_Driver2_MSTAT_ERR_MASK         (0xF0u) /* Mask for all errors */
    #define LED_Driver2_MSTAT_ERR_SHORT_XFER   (0x10u) /* Master NAKed before end of packet */
    #define LED_Driver2_MSTAT_ERR_ADDR_NAK     (0x20u) /* Slave did not ACK */
    #define LED_Driver2_MSTAT_ERR_ARB_LOST     (0x40u) /* Master lost arbitration during communication */
    #define LED_Driver2_MSTAT_ERR_XFER         (0x80u) /* Error during transfer */

    /* Master API returns */
    #define LED_Driver2_MSTR_NO_ERROR          (0x00u) /* Function complete without error */
    #define LED_Driver2_MSTR_BUS_BUSY          (0x01u) /* Bus is busy, process not started */
    #define LED_Driver2_MSTR_NOT_READY         (0x02u) /* Master not Master on the bus or */
                                                            /*  Slave operation in progress */
    #define LED_Driver2_MSTR_ERR_LB_NAK        (0x03u) /* Last Byte Naked */
    #define LED_Driver2_MSTR_ERR_ARB_LOST      (0x04u) /* Master lost arbitration during communication */
    #define LED_Driver2_MSTR_ERR_ABORT_START_GEN  (0x05u) /* Master did not generate Start, the Slave */
                                                               /* was addressed before */

#endif /* (LED_Driver2_MODE_MASTER_ENABLED) */

#if (LED_Driver2_MODE_SLAVE_ENABLED)
    /* Slave Status Constants */
    #define LED_Driver2_SSTAT_RD_CMPLT     (0x01u) /* Read transfer complete */
    #define LED_Driver2_SSTAT_RD_BUSY      (0x02u) /* Read transfer in progress */
    #define LED_Driver2_SSTAT_RD_ERR_OVFL  (0x04u) /* Read overflow Error */
    #define LED_Driver2_SSTAT_RD_MASK      (0x0Fu) /* Read Status Mask */
    #define LED_Driver2_SSTAT_RD_NO_ERR    (0x00u) /* Read no Error */

    #define LED_Driver2_SSTAT_WR_CMPLT     (0x10u) /* Write transfer complete */
    #define LED_Driver2_SSTAT_WR_BUSY      (0x20u) /* Write transfer in progress */
    #define LED_Driver2_SSTAT_WR_ERR_OVFL  (0x40u) /* Write overflow Error */
    #define LED_Driver2_SSTAT_WR_MASK      (0xF0u) /* Write Status Mask  */
    #define LED_Driver2_SSTAT_WR_NO_ERR    (0x00u) /* Write no Error */

    #define LED_Driver2_SSTAT_RD_CLEAR     (0x0Du) /* Read Status clear */
    #define LED_Driver2_SSTAT_WR_CLEAR     (0xD0u) /* Write Status Clear */

#endif /* (LED_Driver2_MODE_SLAVE_ENABLED) */


/***************************************
*       I2C state machine constants
***************************************/

/* Default slave address states */
#define  LED_Driver2_SM_IDLE           (0x10u) /* Default state - IDLE */
#define  LED_Driver2_SM_EXIT_IDLE      (0x00u) /* Pass master and slave processing and go to IDLE */

/* Slave mode states */
#define  LED_Driver2_SM_SLAVE          (LED_Driver2_SM_IDLE) /* Any Slave state */
#define  LED_Driver2_SM_SL_WR_DATA     (0x11u) /* Master writes data to slave  */
#define  LED_Driver2_SM_SL_RD_DATA     (0x12u) /* Master reads data from slave */

/* Master mode states */
#define  LED_Driver2_SM_MASTER         (0x40u) /* Any master state */

#define  LED_Driver2_SM_MSTR_RD        (0x08u) /* Any master read state          */
#define  LED_Driver2_SM_MSTR_RD_ADDR   (0x49u) /* Master sends address with read */
#define  LED_Driver2_SM_MSTR_RD_DATA   (0x4Au) /* Master reads data              */

#define  LED_Driver2_SM_MSTR_WR        (0x04u) /* Any master read state           */
#define  LED_Driver2_SM_MSTR_WR_ADDR   (0x45u) /* Master sends address with write */
#define  LED_Driver2_SM_MSTR_WR_DATA   (0x46u) /* Master writes data              */

#define  LED_Driver2_SM_MSTR_HALT      (0x60u) /* Master waits for ReStart */

#define LED_Driver2_CHECK_SM_MASTER    (0u != (LED_Driver2_SM_MASTER & LED_Driver2_state))
#define LED_Driver2_CHECK_SM_SLAVE     (0u != (LED_Driver2_SM_SLAVE  & LED_Driver2_state))


/***************************************
*              Registers
***************************************/

#if (LED_Driver2_FF_IMPLEMENTED)
    /* Fixed Function registers */
    #define LED_Driver2_XCFG_REG           (*(reg8 *) LED_Driver2_I2C_FF__XCFG)
    #define LED_Driver2_XCFG_PTR           ( (reg8 *) LED_Driver2_I2C_FF__XCFG)

    #define LED_Driver2_ADDR_REG           (*(reg8 *) LED_Driver2_I2C_FF__ADR)
    #define LED_Driver2_ADDR_PTR           ( (reg8 *) LED_Driver2_I2C_FF__ADR)

    #define LED_Driver2_CFG_REG            (*(reg8 *) LED_Driver2_I2C_FF__CFG)
    #define LED_Driver2_CFG_PTR            ( (reg8 *) LED_Driver2_I2C_FF__CFG)

    #define LED_Driver2_CSR_REG            (*(reg8 *) LED_Driver2_I2C_FF__CSR)
    #define LED_Driver2_CSR_PTR            ( (reg8 *) LED_Driver2_I2C_FF__CSR)

    #define LED_Driver2_DATA_REG           (*(reg8 *) LED_Driver2_I2C_FF__D)
    #define LED_Driver2_DATA_PTR           ( (reg8 *) LED_Driver2_I2C_FF__D)

    #define LED_Driver2_MCSR_REG           (*(reg8 *) LED_Driver2_I2C_FF__MCSR)
    #define LED_Driver2_MCSR_PTR           ( (reg8 *) LED_Driver2_I2C_FF__MCSR)

    #define LED_Driver2_ACT_PWRMGR_REG     (*(reg8 *) LED_Driver2_I2C_FF__PM_ACT_CFG)
    #define LED_Driver2_ACT_PWRMGR_PTR     ( (reg8 *) LED_Driver2_I2C_FF__PM_ACT_CFG)
    #define LED_Driver2_ACT_PWR_EN         ( (uint8)  LED_Driver2_I2C_FF__PM_ACT_MSK)

    #define LED_Driver2_STBY_PWRMGR_REG    (*(reg8 *) LED_Driver2_I2C_FF__PM_STBY_CFG)
    #define LED_Driver2_STBY_PWRMGR_PTR    ( (reg8 *) LED_Driver2_I2C_FF__PM_STBY_CFG)
    #define LED_Driver2_STBY_PWR_EN        ( (uint8)  LED_Driver2_I2C_FF__PM_STBY_MSK)

    #define LED_Driver2_PWRSYS_CR1_REG     (*(reg8 *) CYREG_PWRSYS_CR1)
    #define LED_Driver2_PWRSYS_CR1_PTR     ( (reg8 *) CYREG_PWRSYS_CR1)

    #define LED_Driver2_CLKDIV1_REG    (*(reg8 *) LED_Driver2_I2C_FF__CLK_DIV1)
    #define LED_Driver2_CLKDIV1_PTR    ( (reg8 *) LED_Driver2_I2C_FF__CLK_DIV1)

    #define LED_Driver2_CLKDIV2_REG    (*(reg8 *) LED_Driver2_I2C_FF__CLK_DIV2)
    #define LED_Driver2_CLKDIV2_PTR    ( (reg8 *) LED_Driver2_I2C_FF__CLK_DIV2)

#else
    /* UDB implementation registers */
    #define LED_Driver2_CFG_REG \
            (*(reg8 *) LED_Driver2_bI2C_UDB_SyncCtl_CtrlReg__CONTROL_REG)
    #define LED_Driver2_CFG_PTR \
            ( (reg8 *) LED_Driver2_bI2C_UDB_SyncCtl_CtrlReg__CONTROL_REG)

    #define LED_Driver2_CSR_REG        (*(reg8 *) LED_Driver2_bI2C_UDB_StsReg__STATUS_REG)
    #define LED_Driver2_CSR_PTR        ( (reg8 *) LED_Driver2_bI2C_UDB_StsReg__STATUS_REG)

    #define LED_Driver2_INT_MASK_REG   (*(reg8 *) LED_Driver2_bI2C_UDB_StsReg__MASK_REG)
    #define LED_Driver2_INT_MASK_PTR   ( (reg8 *) LED_Driver2_bI2C_UDB_StsReg__MASK_REG)

    #define LED_Driver2_INT_ENABLE_REG (*(reg8 *) LED_Driver2_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)
    #define LED_Driver2_INT_ENABLE_PTR ( (reg8 *) LED_Driver2_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)

    #define LED_Driver2_DATA_REG       (*(reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__A0_REG)
    #define LED_Driver2_DATA_PTR       ( (reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__A0_REG)

    #define LED_Driver2_GO_REG         (*(reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__F1_REG)
    #define LED_Driver2_GO_PTR         ( (reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__F1_REG)

    #define LED_Driver2_GO_DONE_REG    (*(reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__A1_REG)
    #define LED_Driver2_GO_DONE_PTR    ( (reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__A1_REG)

    #define LED_Driver2_MCLK_PRD_REG   (*(reg8 *) LED_Driver2_bI2C_UDB_Master_ClkGen_u0__D0_REG)
    #define LED_Driver2_MCLK_PRD_PTR   ( (reg8 *) LED_Driver2_bI2C_UDB_Master_ClkGen_u0__D0_REG)

    #define LED_Driver2_MCLK_CMP_REG   (*(reg8 *) LED_Driver2_bI2C_UDB_Master_ClkGen_u0__D1_REG)
    #define LED_Driver2_MCLK_CMP_PTR   ( (reg8 *) LED_Driver2_bI2C_UDB_Master_ClkGen_u0__D1_REG)

    #if (LED_Driver2_MODE_SLAVE_ENABLED)
        #define LED_Driver2_ADDR_REG       (*(reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__D0_REG)
        #define LED_Driver2_ADDR_PTR       ( (reg8 *) LED_Driver2_bI2C_UDB_Shifter_u0__D0_REG)

        #define LED_Driver2_PERIOD_REG     (*(reg8 *) LED_Driver2_bI2C_UDB_Slave_BitCounter__PERIOD_REG)
        #define LED_Driver2_PERIOD_PTR     ( (reg8 *) LED_Driver2_bI2C_UDB_Slave_BitCounter__PERIOD_REG)

        #define LED_Driver2_COUNTER_REG    (*(reg8 *) LED_Driver2_bI2C_UDB_Slave_BitCounter__COUNT_REG)
        #define LED_Driver2_COUNTER_PTR    ( (reg8 *) LED_Driver2_bI2C_UDB_Slave_BitCounter__COUNT_REG)

        #define LED_Driver2_COUNTER_AUX_CTL_REG \
                                    (*(reg8 *) LED_Driver2_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)
        #define LED_Driver2_COUNTER_AUX_CTL_PTR \
                                    ( (reg8 *) LED_Driver2_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)

    #endif /* (LED_Driver2_MODE_SLAVE_ENABLED) */

#endif /* (LED_Driver2_FF_IMPLEMENTED) */


/***************************************
*        Registers Constants
***************************************/

/* LED_Driver2_I2C_IRQ */
#define LED_Driver2_ISR_NUMBER     ((uint8) LED_Driver2_I2C_IRQ__INTC_NUMBER)
#define LED_Driver2_ISR_PRIORITY   ((uint8) LED_Driver2_I2C_IRQ__INTC_PRIOR_NUM)

/* I2C Slave Data Register */
#define LED_Driver2_SLAVE_ADDR_MASK    (0x7Fu)
#define LED_Driver2_SLAVE_ADDR_SHIFT   (0x01u)
#define LED_Driver2_DATA_MASK          (0xFFu)
#define LED_Driver2_READ_FLAG          (0x01u)

/* Block reset constants */
#define LED_Driver2_CLEAR_REG          (0x00u)
#define LED_Driver2_BLOCK_RESET_DELAY  (2u)
#define LED_Driver2_FF_RESET_DELAY     (LED_Driver2_BLOCK_RESET_DELAY)
#define LED_Driver2_RESTORE_TIMEOUT    (255u)

#if (LED_Driver2_FF_IMPLEMENTED)
    /* XCFG I2C Extended Configuration Register */
    #define LED_Driver2_XCFG_CLK_EN        (0x80u) /* Enable gated clock to block */
    #define LED_Driver2_XCFG_I2C_ON        (0x40u) /* Enable I2C as wake up source*/
    #define LED_Driver2_XCFG_RDY_TO_SLEEP  (0x20u) /* I2C ready go to sleep */
    #define LED_Driver2_XCFG_FORCE_NACK    (0x10u) /* Force NACK all incoming transactions */
    #define LED_Driver2_XCFG_NO_BC_INT     (0x08u) /* No interrupt on byte complete */
    #define LED_Driver2_XCFG_BUF_MODE      (0x02u) /* Enable buffer mode */
    #define LED_Driver2_XCFG_HDWR_ADDR_EN  (0x01u) /* Enable Hardware address match */

    /* CFG I2C Configuration Register */
    #define LED_Driver2_CFG_SIO_SELECT     (0x80u) /* Pin Select for SCL/SDA lines */
    #define LED_Driver2_CFG_PSELECT        (0x40u) /* Pin Select */
    #define LED_Driver2_CFG_BUS_ERR_IE     (0x20u) /* Bus Error Interrupt Enable */
    #define LED_Driver2_CFG_STOP_IE        (0x10u) /* Enable Interrupt on STOP condition */
    #define LED_Driver2_CFG_CLK_RATE_MSK   (0x0Cu) /* Clock rate select */
    #define LED_Driver2_CFG_CLK_RATE_100   (0x00u) /* Clock rate select 100K */
    #define LED_Driver2_CFG_CLK_RATE_400   (0x04u) /* Clock rate select 400K */
    #define LED_Driver2_CFG_CLK_RATE_050   (0x08u) /* Clock rate select 50K  */
    #define LED_Driver2_CFG_CLK_RATE_RSVD  (0x0Cu) /* Clock rate select Invalid */
    #define LED_Driver2_CFG_EN_MSTR        (0x02u) /* Enable Master operation */
    #define LED_Driver2_CFG_EN_SLAVE       (0x01u) /* Enable Slave operation */

    #define LED_Driver2_CFG_CLK_RATE_LESS_EQUAL_50 (0x04u) /* Clock rate select <= 50kHz */
    #define LED_Driver2_CFG_CLK_RATE_GRATER_50     (0x00u) /* Clock rate select > 50kHz */

    /* CSR I2C Control and Status Register */
    #define LED_Driver2_CSR_BUS_ERROR      (0x80u) /* Active high when bus error has occurred */
    #define LED_Driver2_CSR_LOST_ARB       (0x40u) /* Set to 1 if lost arbitration in host mode */
    #define LED_Driver2_CSR_STOP_STATUS    (0x20u) /* Set if Stop has been detected */
    #define LED_Driver2_CSR_ACK            (0x10u) /* ACK response */
    #define LED_Driver2_CSR_NAK            (0x00u) /* NAK response */
    #define LED_Driver2_CSR_ADDRESS        (0x08u) /* Set in firmware 0 = status bit, 1 Address is slave */
    #define LED_Driver2_CSR_TRANSMIT       (0x04u) /* Set in firmware 1 = transmit, 0 = receive */
    #define LED_Driver2_CSR_LRB            (0x02u) /* Last received bit */
    #define LED_Driver2_CSR_LRB_ACK        (0x00u) /* Last received bit was an ACK */
    #define LED_Driver2_CSR_LRB_NAK        (0x02u) /* Last received bit was an NAK */
    #define LED_Driver2_CSR_BYTE_COMPLETE  (0x01u) /* Informs that last byte has been sent */
    #define LED_Driver2_CSR_STOP_GEN       (0x00u) /* Generate a stop condition */
    #define LED_Driver2_CSR_RDY_TO_RD      (0x00u) /* Set to receive mode */

    /* MCSR I2C Master Control and Status Register */
    #define LED_Driver2_MCSR_STOP_GEN      (0x10u) /* Firmware sets this bit to initiate a Stop condition */
    #define LED_Driver2_MCSR_BUS_BUSY      (0x08u) /* Status bit, Set at Start and cleared at Stop condition */
    #define LED_Driver2_MCSR_MSTR_MODE     (0x04u) /* Status bit, Set at Start and cleared at Stop condition */
    #define LED_Driver2_MCSR_RESTART_GEN   (0x02u) /* Firmware sets this bit to initiate a ReStart condition */
    #define LED_Driver2_MCSR_START_GEN     (0x01u) /* Firmware sets this bit to initiate a Start condition */

    /* PWRSYS_CR1 to handle Sleep */
    #define LED_Driver2_PWRSYS_CR1_I2C_REG_BACKUP  (0x04u) /* Enables, power to I2C regs while sleep */

#else
    /* CONTROL REG bits location */
    #define LED_Driver2_CTRL_START_SHIFT           (7u)
    #define LED_Driver2_CTRL_STOP_SHIFT            (6u)
    #define LED_Driver2_CTRL_RESTART_SHIFT         (5u)
    #define LED_Driver2_CTRL_NACK_SHIFT            (4u)
    #define LED_Driver2_CTRL_ANY_ADDRESS_SHIFT     (3u)
    #define LED_Driver2_CTRL_TRANSMIT_SHIFT        (2u)
    #define LED_Driver2_CTRL_ENABLE_MASTER_SHIFT   (1u)
    #define LED_Driver2_CTRL_ENABLE_SLAVE_SHIFT    (0u)
    #define LED_Driver2_CTRL_START_MASK            ((uint8) (0x01u << LED_Driver2_CTRL_START_SHIFT))
    #define LED_Driver2_CTRL_STOP_MASK             ((uint8) (0x01u << LED_Driver2_CTRL_STOP_SHIFT))
    #define LED_Driver2_CTRL_RESTART_MASK          ((uint8) (0x01u << LED_Driver2_CTRL_RESTART_SHIFT))
    #define LED_Driver2_CTRL_NACK_MASK             ((uint8) (0x01u << LED_Driver2_CTRL_NACK_SHIFT))
    #define LED_Driver2_CTRL_ANY_ADDRESS_MASK      ((uint8) (0x01u << LED_Driver2_CTRL_ANY_ADDRESS_SHIFT))
    #define LED_Driver2_CTRL_TRANSMIT_MASK         ((uint8) (0x01u << LED_Driver2_CTRL_TRANSMIT_SHIFT))
    #define LED_Driver2_CTRL_ENABLE_MASTER_MASK    ((uint8) (0x01u << LED_Driver2_CTRL_ENABLE_MASTER_SHIFT))
    #define LED_Driver2_CTRL_ENABLE_SLAVE_MASK     ((uint8) (0x01u << LED_Driver2_CTRL_ENABLE_SLAVE_SHIFT))

    /* STATUS REG bits location */
    #define LED_Driver2_STS_LOST_ARB_SHIFT         (6u)
    #define LED_Driver2_STS_STOP_SHIFT             (5u)
    #define LED_Driver2_STS_BUSY_SHIFT             (4u)
    #define LED_Driver2_STS_ADDR_SHIFT             (3u)
    #define LED_Driver2_STS_MASTER_MODE_SHIFT      (2u)
    #define LED_Driver2_STS_LRB_SHIFT              (1u)
    #define LED_Driver2_STS_BYTE_COMPLETE_SHIFT    (0u)
    #define LED_Driver2_STS_LOST_ARB_MASK          ((uint8) (0x01u << LED_Driver2_STS_LOST_ARB_SHIFT))
    #define LED_Driver2_STS_STOP_MASK              ((uint8) (0x01u << LED_Driver2_STS_STOP_SHIFT))
    #define LED_Driver2_STS_BUSY_MASK              ((uint8) (0x01u << LED_Driver2_STS_BUSY_SHIFT))
    #define LED_Driver2_STS_ADDR_MASK              ((uint8) (0x01u << LED_Driver2_STS_ADDR_SHIFT))
    #define LED_Driver2_STS_MASTER_MODE_MASK       ((uint8) (0x01u << LED_Driver2_STS_MASTER_MODE_SHIFT))
    #define LED_Driver2_STS_LRB_MASK               ((uint8) (0x01u << LED_Driver2_STS_LRB_SHIFT))
    #define LED_Driver2_STS_BYTE_COMPLETE_MASK     ((uint8) (0x01u << LED_Driver2_STS_BYTE_COMPLETE_SHIFT))

    /* AUX_CTL bits definition */
    #define LED_Driver2_COUNTER_ENABLE_MASK        (0x20u) /* Enable 7-bit counter */
    #define LED_Driver2_INT_ENABLE_MASK            (0x10u) /* Enable interrupt from status register */
    #define LED_Driver2_CNT7_ENABLE                (LED_Driver2_COUNTER_ENABLE_MASK)
    #define LED_Driver2_INTR_ENABLE                (LED_Driver2_INT_ENABLE_MASK)

#endif /* (LED_Driver2_FF_IMPLEMENTED) */


/***************************************
*        Marco
***************************************/

/* ACK and NACK for data and address checks */
#define LED_Driver2_CHECK_ADDR_ACK(csr)    ((LED_Driver2_CSR_LRB_ACK | LED_Driver2_CSR_ADDRESS) == \
                                                 ((LED_Driver2_CSR_LRB    | LED_Driver2_CSR_ADDRESS) &  \
                                                  (csr)))


#define LED_Driver2_CHECK_ADDR_NAK(csr)    ((LED_Driver2_CSR_LRB_NAK | LED_Driver2_CSR_ADDRESS) == \
                                                 ((LED_Driver2_CSR_LRB    | LED_Driver2_CSR_ADDRESS) &  \
                                                  (csr)))

#define LED_Driver2_CHECK_DATA_ACK(csr)    (0u == ((csr) & LED_Driver2_CSR_LRB_NAK))

/* MCSR conditions check */
#define LED_Driver2_CHECK_BUS_FREE(mcsr)       (0u == ((mcsr) & LED_Driver2_MCSR_BUS_BUSY))
#define LED_Driver2_CHECK_MASTER_MODE(mcsr)    (0u != ((mcsr) & LED_Driver2_MCSR_MSTR_MODE))

/* CSR conditions check */
#define LED_Driver2_WAIT_BYTE_COMPLETE(csr)    (0u == ((csr) & LED_Driver2_CSR_BYTE_COMPLETE))
#define LED_Driver2_WAIT_STOP_COMPLETE(csr)    (0u == ((csr) & (LED_Driver2_CSR_BYTE_COMPLETE | \
                                                                     LED_Driver2_CSR_STOP_STATUS)))
#define LED_Driver2_CHECK_BYTE_COMPLETE(csr)   (0u != ((csr) & LED_Driver2_CSR_BYTE_COMPLETE))
#define LED_Driver2_CHECK_STOP_STS(csr)        (0u != ((csr) & LED_Driver2_CSR_STOP_STATUS))
#define LED_Driver2_CHECK_LOST_ARB(csr)        (0u != ((csr) & LED_Driver2_CSR_LOST_ARB))
#define LED_Driver2_CHECK_ADDRESS_STS(csr)     (0u != ((csr) & LED_Driver2_CSR_ADDRESS))

/* Software start and end of transaction check */
#define LED_Driver2_CHECK_RESTART(mstrCtrl)    (0u != ((mstrCtrl) & LED_Driver2_MODE_REPEAT_START))
#define LED_Driver2_CHECK_NO_STOP(mstrCtrl)    (0u != ((mstrCtrl) & LED_Driver2_MODE_NO_STOP))

/* Send read or write completion depends on state */
#define LED_Driver2_GET_MSTAT_CMPLT ((0u != (LED_Driver2_state & LED_Driver2_SM_MSTR_RD)) ? \
                                                 (LED_Driver2_MSTAT_RD_CMPLT) : (LED_Driver2_MSTAT_WR_CMPLT))

/* Returns 7-bit slave address */
#define LED_Driver2_GET_SLAVE_ADDR(dataReg)   (((dataReg) >> LED_Driver2_SLAVE_ADDR_SHIFT) & \
                                                                  LED_Driver2_SLAVE_ADDR_MASK)

#if (LED_Driver2_FF_IMPLEMENTED)
    /* Check enable of module */
    #define LED_Driver2_I2C_ENABLE_REG     (LED_Driver2_ACT_PWRMGR_REG)
    #define LED_Driver2_IS_I2C_ENABLE(reg) (0u != ((reg) & LED_Driver2_ACT_PWR_EN))
    #define LED_Driver2_IS_ENABLED         (0u != (LED_Driver2_ACT_PWRMGR_REG & LED_Driver2_ACT_PWR_EN))

    #define LED_Driver2_CHECK_PWRSYS_I2C_BACKUP    (0u != (LED_Driver2_PWRSYS_CR1_I2C_REG_BACKUP & \
                                                                LED_Driver2_PWRSYS_CR1_REG))

    /* Check start condition generation */
    #define LED_Driver2_CHECK_START_GEN(mcsr)  ((0u != ((mcsr) & LED_Driver2_MCSR_START_GEN)) && \
                                                     (0u == ((mcsr) & LED_Driver2_MCSR_MSTR_MODE)))

    #define LED_Driver2_CLEAR_START_GEN        do{ \
                                                        LED_Driver2_MCSR_REG &=                                   \
                                                                           ((uint8) ~LED_Driver2_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define LED_Driver2_ENABLE_INT_ON_STOP     do{ \
                                                        LED_Driver2_CFG_REG |= LED_Driver2_CFG_STOP_IE; \
                                                    }while(0)

    #define LED_Driver2_DISABLE_INT_ON_STOP    do{ \
                                                        LED_Driver2_CFG_REG &=                                 \
                                                                           ((uint8) ~LED_Driver2_CFG_STOP_IE); \
                                                    }while(0)

    /* Transmit data */
    #define LED_Driver2_TRANSMIT_DATA          do{ \
                                                        LED_Driver2_CSR_REG = LED_Driver2_CSR_TRANSMIT; \
                                                    }while(0)

    #define LED_Driver2_ACK_AND_TRANSMIT       do{ \
                                                        LED_Driver2_CSR_REG = (LED_Driver2_CSR_ACK |      \
                                                                                    LED_Driver2_CSR_TRANSMIT); \
                                                    }while(0)

    #define LED_Driver2_NAK_AND_TRANSMIT       do{ \
                                                        LED_Driver2_CSR_REG = LED_Driver2_CSR_NAK; \
                                                    }while(0)

    /* Special case: udb needs to ack, ff needs to nak */
    #define LED_Driver2_ACKNAK_AND_TRANSMIT    do{ \
                                                        LED_Driver2_CSR_REG  = (LED_Driver2_CSR_NAK |      \
                                                                                     LED_Driver2_CSR_TRANSMIT); \
                                                    }while(0)
    /* Receive data */
    #define LED_Driver2_ACK_AND_RECEIVE        do{ \
                                                        LED_Driver2_CSR_REG = LED_Driver2_CSR_ACK; \
                                                    }while(0)

    #define LED_Driver2_NAK_AND_RECEIVE        do{ \
                                                        LED_Driver2_CSR_REG = LED_Driver2_CSR_NAK; \
                                                    }while(0)

    #define LED_Driver2_READY_TO_READ          do{ \
                                                        LED_Driver2_CSR_REG = LED_Driver2_CSR_RDY_TO_RD; \
                                                    }while(0)

    /* Release bus after lost arbitration */
    #define LED_Driver2_BUS_RELEASE    LED_Driver2_READY_TO_READ

    /* Master Start/ReStart/Stop conditions generation */
    #define LED_Driver2_GENERATE_START         do{ \
                                                        LED_Driver2_MCSR_REG = LED_Driver2_MCSR_START_GEN; \
                                                    }while(0)

    #define LED_Driver2_GENERATE_RESTART \
                    do{                       \
                        LED_Driver2_MCSR_REG = (LED_Driver2_MCSR_RESTART_GEN | \
                                                     LED_Driver2_MCSR_STOP_GEN);    \
                        LED_Driver2_CSR_REG  = LED_Driver2_CSR_TRANSMIT;       \
                    }while(0)

    #define LED_Driver2_GENERATE_STOP \
                    do{                    \
                        LED_Driver2_MCSR_REG = LED_Driver2_MCSR_STOP_GEN; \
                        LED_Driver2_CSR_REG  = LED_Driver2_CSR_TRANSMIT;  \
                    }while(0)

    /* Master manual APIs compatible defines */
    #define LED_Driver2_GENERATE_START_MANUAL      LED_Driver2_GENERATE_START
    #define LED_Driver2_GENERATE_RESTART_MANUAL    LED_Driver2_GENERATE_RESTART
    #define LED_Driver2_GENERATE_STOP_MANUAL       LED_Driver2_GENERATE_STOP
    #define LED_Driver2_TRANSMIT_DATA_MANUAL       LED_Driver2_TRANSMIT_DATA
    #define LED_Driver2_READY_TO_READ_MANUAL       LED_Driver2_READY_TO_READ
    #define LED_Driver2_ACK_AND_RECEIVE_MANUAL     LED_Driver2_ACK_AND_RECEIVE
    #define LED_Driver2_BUS_RELEASE_MANUAL         LED_Driver2_BUS_RELEASE

#else

    /* Masks to enable interrupts from Status register */
    #define LED_Driver2_STOP_IE_MASK           (LED_Driver2_STS_STOP_MASK)
    #define LED_Driver2_BYTE_COMPLETE_IE_MASK  (LED_Driver2_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: CSR register bit-fields */
    #define LED_Driver2_CSR_LOST_ARB       (LED_Driver2_STS_LOST_ARB_MASK)
    #define LED_Driver2_CSR_STOP_STATUS    (LED_Driver2_STS_STOP_MASK)
    #define LED_Driver2_CSR_BUS_ERROR      (0x00u)
    #define LED_Driver2_CSR_ADDRESS        (LED_Driver2_STS_ADDR_MASK)
    #define LED_Driver2_CSR_TRANSMIT       (LED_Driver2_CTRL_TRANSMIT_MASK)
    #define LED_Driver2_CSR_LRB            (LED_Driver2_STS_LRB_MASK)
    #define LED_Driver2_CSR_LRB_NAK        (LED_Driver2_STS_LRB_MASK)
    #define LED_Driver2_CSR_LRB_ACK        (0x00u)
    #define LED_Driver2_CSR_BYTE_COMPLETE  (LED_Driver2_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: MCSR registers bit-fields */
    #define LED_Driver2_MCSR_REG           (LED_Driver2_CSR_REG)  /* UDB incorporates master and slave regs */
    #define LED_Driver2_MCSR_BUS_BUSY      (LED_Driver2_STS_BUSY_MASK)      /* Is bus is busy               */
    #define LED_Driver2_MCSR_START_GEN     (LED_Driver2_CTRL_START_MASK)    /* Generate Start condition     */
    #define LED_Driver2_MCSR_RESTART_GEN   (LED_Driver2_CTRL_RESTART_MASK)  /* Generates RESTART condition  */
    #define LED_Driver2_MCSR_MSTR_MODE     (LED_Driver2_STS_MASTER_MODE_MASK)/* Define if active Master     */

    /* Data to write into TX FIFO to release FSM */
    #define LED_Driver2_PREPARE_TO_RELEASE (0xFFu)
    #define LED_Driver2_RELEASE_FSM        (0x00u)

    /* Define release command done: history of byte complete status is cleared */
    #define LED_Driver2_WAIT_RELEASE_CMD_DONE  (LED_Driver2_RELEASE_FSM != LED_Driver2_GO_DONE_REG)

    /* Check enable of module */
    #define LED_Driver2_I2C_ENABLE_REG     (LED_Driver2_CFG_REG)
    #define LED_Driver2_IS_I2C_ENABLE(reg) ((0u != ((reg) & LED_Driver2_ENABLE_MASTER)) || \
                                                 (0u != ((reg) & LED_Driver2_ENABLE_SLAVE)))

    #define LED_Driver2_IS_ENABLED         (0u != (LED_Driver2_CFG_REG & LED_Driver2_ENABLE_MS))

    /* Check start condition generation */
    #define LED_Driver2_CHECK_START_GEN(mcsr)  ((0u != (LED_Driver2_CFG_REG &        \
                                                             LED_Driver2_MCSR_START_GEN)) \
                                                    &&                                         \
                                                    (0u == ((mcsr) & LED_Driver2_MCSR_MSTR_MODE)))

    #define LED_Driver2_CLEAR_START_GEN        do{ \
                                                        LED_Driver2_CFG_REG &=                 \
                                                        ((uint8) ~LED_Driver2_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define LED_Driver2_ENABLE_INT_ON_STOP     do{ \
                                                       LED_Driver2_INT_MASK_REG |= LED_Driver2_STOP_IE_MASK; \
                                                    }while(0)

    #define LED_Driver2_DISABLE_INT_ON_STOP    do{ \
                                                        LED_Driver2_INT_MASK_REG &=                               \
                                                                             ((uint8) ~LED_Driver2_STOP_IE_MASK); \
                                                    }while(0)

    /* Transmit data */
    #define LED_Driver2_TRANSMIT_DATA \
                                    do{    \
                                        LED_Driver2_CFG_REG     = (LED_Driver2_CTRL_TRANSMIT_MASK | \
                                                                       LED_Driver2_CTRL_DEFAULT);        \
                                        LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE;   \
                                        LED_Driver2_GO_REG      = LED_Driver2_RELEASE_FSM;          \
                                    }while(0)

    #define LED_Driver2_ACK_AND_TRANSMIT   LED_Driver2_TRANSMIT_DATA

    #define LED_Driver2_NAK_AND_TRANSMIT \
                                        do{   \
                                            LED_Driver2_CFG_REG     = (LED_Driver2_CTRL_NACK_MASK     | \
                                                                            LED_Driver2_CTRL_TRANSMIT_MASK | \
                                                                            LED_Driver2_CTRL_DEFAULT);       \
                                            LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE;   \
                                            LED_Driver2_GO_REG      = LED_Driver2_RELEASE_FSM;          \
                                        }while(0)

    /* Receive data */
    #define LED_Driver2_READY_TO_READ  \
                                        do{ \
                                            LED_Driver2_CFG_REG     = LED_Driver2_CTRL_DEFAULT;       \
                                            LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE; \
                                            LED_Driver2_GO_REG      = LED_Driver2_RELEASE_FSM;       \
                                        }while(0)

    #define LED_Driver2_ACK_AND_RECEIVE    LED_Driver2_READY_TO_READ

    /* Release bus after arbitration is lost */
    #define LED_Driver2_BUS_RELEASE    LED_Driver2_READY_TO_READ

    #define LED_Driver2_NAK_AND_RECEIVE \
                                        do{  \
                                            LED_Driver2_CFG_REG     = (LED_Driver2_CTRL_NACK_MASK |   \
                                                                            LED_Driver2_CTRL_DEFAULT);     \
                                            LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE; \
                                            LED_Driver2_GO_REG      = LED_Driver2_RELEASE_FSM;       \
                                        }while(0)

    /* Master condition generation */
    #define LED_Driver2_GENERATE_START \
                                        do{ \
                                            LED_Driver2_CFG_REG     = (LED_Driver2_CTRL_START_MASK |  \
                                                                            LED_Driver2_CTRL_DEFAULT);     \
                                            LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE; \
                                            LED_Driver2_GO_REG      = LED_Driver2_RELEASE_FSM;       \
                                        }while(0)

    #define LED_Driver2_GENERATE_RESTART \
                                        do{   \
                                            LED_Driver2_CFG_REG     = (LED_Driver2_CTRL_RESTART_MASK | \
                                                                            LED_Driver2_CTRL_NACK_MASK    | \
                                                                            LED_Driver2_CTRL_DEFAULT);      \
                                            LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE;  \
                                            LED_Driver2_GO_REG  =     LED_Driver2_RELEASE_FSM;         \
                                        }while(0)

    #define LED_Driver2_GENERATE_STOP  \
                                        do{ \
                                            LED_Driver2_CFG_REG    = (LED_Driver2_CTRL_NACK_MASK |    \
                                                                           LED_Driver2_CTRL_STOP_MASK |    \
                                                                           LED_Driver2_CTRL_DEFAULT);      \
                                            LED_Driver2_GO_DONE_REG = LED_Driver2_PREPARE_TO_RELEASE; \
                                            LED_Driver2_GO_REG      = LED_Driver2_RELEASE_FSM;        \
                                        }while(0)

    /* Master manual APIs compatible macros */
    /* These macros wait until byte complete history is cleared after command issued */
    #define LED_Driver2_GENERATE_START_MANUAL \
                                        do{ \
                                            LED_Driver2_GENERATE_START;                  \
                                            /* Wait until byte complete history is cleared */ \
                                            while(LED_Driver2_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)
                                        
    #define LED_Driver2_GENERATE_RESTART_MANUAL \
                                        do{          \
                                            LED_Driver2_GENERATE_RESTART;                \
                                            /* Wait until byte complete history is cleared */ \
                                            while(LED_Driver2_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define LED_Driver2_GENERATE_STOP_MANUAL \
                                        do{       \
                                            LED_Driver2_GENERATE_STOP;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(LED_Driver2_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define LED_Driver2_TRANSMIT_DATA_MANUAL \
                                        do{       \
                                            LED_Driver2_TRANSMIT_DATA;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(LED_Driver2_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define LED_Driver2_READY_TO_READ_MANUAL \
                                        do{       \
                                            LED_Driver2_READY_TO_READ;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(LED_Driver2_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define LED_Driver2_ACK_AND_RECEIVE_MANUAL \
                                        do{         \
                                            LED_Driver2_ACK_AND_RECEIVE;                 \
                                            /* Wait until byte complete history is cleared */ \
                                            while(LED_Driver2_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define LED_Driver2_BUS_RELEASE_MANUAL LED_Driver2_READY_TO_READ_MANUAL

#endif /* (LED_Driver2_FF_IMPLEMENTED) */


/***************************************
*     Default register init constants
***************************************/

#define LED_Driver2_DISABLE    (0u)
#define LED_Driver2_ENABLE     (1u)

#if (LED_Driver2_FF_IMPLEMENTED)
    /* LED_Driver2_XCFG_REG: bits definition */
    #define LED_Driver2_DEFAULT_XCFG_HW_ADDR_EN ((LED_Driver2_HW_ADRR_DECODE) ? \
                                                        (LED_Driver2_XCFG_HDWR_ADDR_EN) : (0u))

    #define LED_Driver2_DEFAULT_XCFG_I2C_ON    ((LED_Driver2_WAKEUP_ENABLED) ? \
                                                        (LED_Driver2_XCFG_I2C_ON) : (0u))


    #define LED_Driver2_DEFAULT_CFG_SIO_SELECT ((LED_Driver2_I2C1_SIO_PAIR) ? \
                                                        (LED_Driver2_CFG_SIO_SELECT) : (0u))


    /* LED_Driver2_CFG_REG: bits definition */
    #define LED_Driver2_DEFAULT_CFG_PSELECT    ((LED_Driver2_WAKEUP_ENABLED) ? \
                                                        (LED_Driver2_CFG_PSELECT) : (0u))

    #define LED_Driver2_DEFAULT_CLK_RATE0  ((LED_Driver2_DATA_RATE <= 50u) ?        \
                                                    (LED_Driver2_CFG_CLK_RATE_050) :     \
                                                    ((LED_Driver2_DATA_RATE <= 100u) ?   \
                                                        (LED_Driver2_CFG_CLK_RATE_100) : \
                                                        (LED_Driver2_CFG_CLK_RATE_400)))

    #define LED_Driver2_DEFAULT_CLK_RATE1  ((LED_Driver2_DATA_RATE <= 50u) ?           \
                                                 (LED_Driver2_CFG_CLK_RATE_LESS_EQUAL_50) : \
                                                 (LED_Driver2_CFG_CLK_RATE_GRATER_50))

    #define LED_Driver2_DEFAULT_CLK_RATE   (LED_Driver2_DEFAULT_CLK_RATE1)


    #define LED_Driver2_ENABLE_MASTER      ((LED_Driver2_MODE_MASTER_ENABLED) ? \
                                                 (LED_Driver2_CFG_EN_MSTR) : (0u))

    #define LED_Driver2_ENABLE_SLAVE       ((LED_Driver2_MODE_SLAVE_ENABLED) ? \
                                                 (LED_Driver2_CFG_EN_SLAVE) : (0u))

    #define LED_Driver2_ENABLE_MS      (LED_Driver2_ENABLE_MASTER | LED_Driver2_ENABLE_SLAVE)


    /* LED_Driver2_DEFAULT_XCFG_REG */
    #define LED_Driver2_DEFAULT_XCFG   (LED_Driver2_XCFG_CLK_EN         | \
                                             LED_Driver2_DEFAULT_XCFG_I2C_ON | \
                                             LED_Driver2_DEFAULT_XCFG_HW_ADDR_EN)

    /* LED_Driver2_DEFAULT_CFG_REG */
    #define LED_Driver2_DEFAULT_CFG    (LED_Driver2_DEFAULT_CFG_SIO_SELECT | \
                                             LED_Driver2_DEFAULT_CFG_PSELECT    | \
                                             LED_Driver2_DEFAULT_CLK_RATE       | \
                                             LED_Driver2_ENABLE_MASTER          | \
                                             LED_Driver2_ENABLE_SLAVE)

    /*LED_Driver2_DEFAULT_DIVIDE_FACTOR_REG */
    #define LED_Driver2_DEFAULT_DIVIDE_FACTOR  ((uint16) 1u)

#else
    /* LED_Driver2_CFG_REG: bits definition  */
    #define LED_Driver2_ENABLE_MASTER  ((LED_Driver2_MODE_MASTER_ENABLED) ? \
                                             (LED_Driver2_CTRL_ENABLE_MASTER_MASK) : (0u))

    #define LED_Driver2_ENABLE_SLAVE   ((LED_Driver2_MODE_SLAVE_ENABLED) ? \
                                             (LED_Driver2_CTRL_ENABLE_SLAVE_MASK) : (0u))

    #define LED_Driver2_ENABLE_MS      (LED_Driver2_ENABLE_MASTER | LED_Driver2_ENABLE_SLAVE)


    #define LED_Driver2_DEFAULT_CTRL_ANY_ADDR   ((LED_Driver2_HW_ADRR_DECODE) ? \
                                                      (0u) : (LED_Driver2_CTRL_ANY_ADDRESS_MASK))

    /* LED_Driver2_DEFAULT_CFG_REG */
    #define LED_Driver2_DEFAULT_CFG    (LED_Driver2_DEFAULT_CTRL_ANY_ADDR)

    /* All CTRL default bits to be used in macro */
    #define LED_Driver2_CTRL_DEFAULT   (LED_Driver2_DEFAULT_CTRL_ANY_ADDR | LED_Driver2_ENABLE_MS)

    /* Master clock generator: d0 and d1 */
    #define LED_Driver2_MCLK_PERIOD_VALUE  (0x0Fu)
    #define LED_Driver2_MCLK_COMPARE_VALUE (0x08u)

    /* Slave bit-counter: control period */
    #define LED_Driver2_PERIOD_VALUE       (0x07u)

    /* LED_Driver2_DEFAULT_INT_MASK */
    #define LED_Driver2_DEFAULT_INT_MASK   (LED_Driver2_BYTE_COMPLETE_IE_MASK)

    /* LED_Driver2_DEFAULT_MCLK_PRD_REG */
    #define LED_Driver2_DEFAULT_MCLK_PRD   (LED_Driver2_MCLK_PERIOD_VALUE)

    /* LED_Driver2_DEFAULT_MCLK_CMP_REG */
    #define LED_Driver2_DEFAULT_MCLK_CMP   (LED_Driver2_MCLK_COMPARE_VALUE)

    /* LED_Driver2_DEFAULT_PERIOD_REG */
    #define LED_Driver2_DEFAULT_PERIOD     (LED_Driver2_PERIOD_VALUE)

#endif /* (LED_Driver2_FF_IMPLEMENTED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

#define LED_Driver2_SSTAT_RD_ERR       (0x08u)
#define LED_Driver2_SSTAT_WR_ERR       (0x80u)
#define LED_Driver2_MSTR_SLAVE_BUSY    (LED_Driver2_MSTR_NOT_READY)
#define LED_Driver2_MSTAT_ERR_BUF_OVFL (0x80u)
#define LED_Driver2_SSTAT_RD_CMPT      (LED_Driver2_SSTAT_RD_CMPLT)
#define LED_Driver2_SSTAT_WR_CMPT      (LED_Driver2_SSTAT_WR_CMPLT)
#define LED_Driver2_MODE_MULTI_MASTER_ENABLE    (LED_Driver2_MODE_MULTI_MASTER_MASK)
#define LED_Driver2_DATA_RATE_50       (50u)
#define LED_Driver2_DATA_RATE_100      (100u)
#define LED_Driver2_DEV_MASK           (0xF0u)
#define LED_Driver2_SM_SL_STOP         (0x14u)
#define LED_Driver2_SM_MASTER_IDLE     (0x40u)
#define LED_Driver2_HDWR_DECODE        (0x01u)

#endif /* CY_I2C_LED_Driver2_H */


/* [] END OF FILE */
