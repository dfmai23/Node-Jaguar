#include <project.h>
#include <stdio.h>
#include "can_manga.h"

#define PWM_PULSE_WIDTH_STEP        (10u)
#define SWITCH_PRESSED              (0u)
#define PWM_MESSAGE_ID              (0x1AAu)
#define PWM_MESSAGE_IDE             (0u)    /* Standard message */
#define PWM_MESSAGE_IRQ             (0u)    /* No transmit IRQ */
#define PWM_MESSAGE_RTR             (0u)    /* No RTR */
#define CAN_RX_MAILBOX_0_SHIFT      (1u)
#define CAN_RX_MAILBOX_1_SHIFT      (2u)
#define DATA_SIZE                   (6u)
#define ONE_BYTE_OFFSET             (8u)

#define PEDAL_TIMEOUT 100 // Timeout after (PEDAL_TIMEOUT * 10)ms

/* Function prototypes */
//CY_ISR_PROTO(ISR_CAN);

/* Global variables used to store configuration and data for BASIC CAN mailbox */
//CAN_DATA_BYTES_MSG dataPWM;
//CAN_TX_MSG messagePWM;

/* Global variable used to store PWM pulse width value */
//uint8 pulseWidthValue = 0u;

/* Global variable used to store receive message mailbox number */
//volatile uint8 receiveMailboxNumber = 0xFFu;

void nodeCheckStart()
{
    Node_Timer_Start();
    isr_nodeok_Start();
}

typedef enum 
{
	Startup,
	LV,
	Precharging,
	HV_Enabled,
	Drive,
	Fault
    
}Dash_State;

typedef enum 
{
	OK,
	fromLV,
	fromPrecharging,
	fromHV_Enabled,
	fromDrive,
    fromFault,
    nodeFailure
    
}Error_State;

/* Switch state defines -- Active High*/ 
#define SWITCH_ON         (1u)
#define SWITCH_OFF        (0u)
/* Switch debounce delay in milliseconds */
#define SWITCH_DEBOUNCE_UNIT   (1u)
/* Number of debounce units to count delay, before consider that switch is pressed */
#define SWITCH_DEBOUNCE_PERIOD (10u)
/* Function prototypes */
static uint32 ReadSwSwitch(void);

/* Global variable used to store switch state */
uint8 HVSwitch = SWITCH_OFF;
uint8 DriveSwitch = SWITCH_OFF;
//volatile Dash_State state = Startup;

// Global variables used to track status of nodes
volatile uint32_t pedalOK = 0; // for pedal node

//CY_ISR(isr_can_handler){
    
//    can_send_status(state);
    
//}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  main() performs the following functions:
*  1: Initializes a structure for the Basic CAN mailbox to send messages.
*  2: Starts the CAN and LCD components.
*  3: When received Message 1, sends the PWM pulse width and displays
*     received switch status and value of PWM pulse width on an LCD; 
*     When received Message 2, display received ADC data on an LCD.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/

int main()
{   
    Dash_State state = Startup;
    Error_State error_state = OK;
    
    //Tach Meter Stuff
    uint8_t value=0; // replace the value with 
    int8_t direction=1;
    WaveDAC8_1_Start();
    
    //precharging time counter
    volatile uint32_t PrechargingTimeCount = 0;
    uint32_t DriveTimeCount = 0;
    
    //CyDelay(5000);
    //Initialize CAN
    //CAN_GlobalIntEnable();
    //CAN_Init();
    //CAN_Start();
    
    /* Set CAN interrupt handler to local routine */
    //CyIntSetVector(CAN_ISR_NUMBER, ISR_CAN);   
    
    //CAN_Timer_Start();
    //isr_can_StartEx(isr_can_handler);    

    CyGlobalIntEnable;
    
    //while(1){
    //}
    
    //char8 txData[DATA_SIZE];
    //uint16 adcData;
    
    /* BASIC CAN mailbox configuration */
    //messagePWM.dlc = CAN_TX_DLC_MAX_VALUE;
    //messagePWM.id  = PWM_MESSAGE_ID;
    //messagePWM.ide = PWM_MESSAGE_IDE;
    //messagePWM.irq = PWM_MESSAGE_IRQ;
    //messagePWM.msg = &dataPWM;
    //messagePWM.rtr = PWM_MESSAGE_RTR;

    ////LCD_Start();

    
    /* Display value of ADC output on LCD */
    //LCD_Position(0u, 0u);
    //LCD_PrintString("ADC");

    /* Display state of switch on LCD */
    //LCD_Position(1u, 0u);
    //LCD_PrintString("SW");

    /* Display state of PWM pulse width on LCD */
    //LCD_Position(0u, 10u);
    //LCD_PrintString("PWM");

    //test_inject(data_queue, &data_tail);
    
    nodeCheckStart();
    
    for(;;)
    {
        //LED_Write(~LED_ReadDataReg());    
        
        //can_send_status(state);
        //CyDelay(2000);
        
        RGB3_2_Write(1);
        RGB2_2_Write(1);
        RGB1_2_Write(1);
        
        // Check if all nodes are OK
        if (pedalOK > PEDAL_TIMEOUT)
        {
            can_send_cmd(1,0,0); // setInterlock. 
            state = Fault;
            error_state = nodeFailure;
        }
        
        switch(state)
        {    
            //CyDelay(1000);
            case Startup:
  
                //CyDelay(5000);
                //Initialize CAN
                CAN_GlobalIntEnable();
                CAN_Init();
                CAN_Start();
                CyDelay(1000);
                //nodeCheckStart(); EDIT UNCOMMENT
                
                //CyDelay(5000);
                can_send_status(state, error_state);

                //CyGlobalIntEnable;
                //CAN_GlobalIntDisable();
                //CyGlobalIntDisable;
                LCD_Start();
                
                //UI
                
                Buzzer_Write(1);
                WaveDAC8_1_SetValue(253);
                CyDelay(50);
                
                
                /*
                //Active Low
                RGB3_1_Write(1);
                RGB2_1_Write(1);
                RGB1_1_Write(1);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);
                
                //RGB Testing
                RGB3_1_Write(0);
                CyDelay(2000);
                RGB3_1_Write(1);
                RGB2_1_Write(0);
                CyDelay(2000);
                RGB2_1_Write(1);
                RGB1_1_Write(0);
                CyDelay(2000);
                RGB1_1_Write(1);
                RGB3_2_Write(0);
                CyDelay(2000);
                RGB3_2_Write(1);
                RGB2_2_Write(0);
                CyDelay(2000);
                RGB2_2_Write(1);
                RGB1_2_Write(0);
                CyDelay(2000);
                RGB1_2_Write(1);
                */
                
                
                Buzzer_Write(0);
                WaveDAC8_1_SetValue(250);
                //CyDelay(5000);
                WaveDAC8_1_SetValue(0);
                
                state = LV;
                
            break;
                
            case LV:
                
                can_send_cmd(0,0,0);
                CAN_GlobalIntEnable();
                CAN_Init();
                CAN_Start();
                //nodeCheckStart();
          
                can_send_status(state, error_state);

                /*
                LCD_Position(1u, 10u);
                LCD_PrintInt16(data_queue[0].id);
                
                uint16_t temp = can_read(data_queue, data_head, data_tail, 0x07FF, 0);
                
                LCD_Position(0u, 0u);
                LCD_PrintInt16(temp);
                
                temp = can_read(data_queue, data_head, data_tail, 0x07FF, 1);
                LCD_Position(1u, 0u);
                LCD_PrintInt8(temp);
                
                temp = can_read(data_queue, data_head, data_tail, 0x07FF, 2);
                LCD_Position(0u, 10u);
                LCD_PrintInt8(temp);
                */
                
                
                //UI
                
                Buzzer_Write(0);
                
                
                //
                // RGB code goes here
                // pick a color
                // all on. 
                
                RGB3_1_Write(0);
                RGB2_1_Write(1);
                RGB1_1_Write(0);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);
                
                if (Drive_Read())
                {
                    state = Fault;
                    error_state = fromLV;
                    break;
                }
            
                if (HV_Read())    /* Switch state changed status */
                {
                    state = Precharging;
                    break;
                } 
                
            break;
                
            case Precharging:
                CAN_GlobalIntEnable();
                CAN_Init();
                CAN_Start();
                //nodeCheckStart();
                
                can_send_status(state, error_state);
                
                RGB3_1_Write(0);
                RGB2_1_Write(0);
                RGB1_1_Write(1);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);
                
                Buzzer_Write(0);
                
                
                PrechargingTimeCount = 0;
                
                while(1)
                {
                    can_send_cmd(1,0,0); // setInterlock
                
                    // UNUSED //uint8_t MainState = can_read(data_queue, data_head, data_tail, 0x0566, 0);
                    uint8_t CapacitorVolt = getCapacitorVoltage(); //can_read(data_queue, data_head, data_tail, 0x0566, 0);
                    // UNUSED //uint8_t NomialVolt =  can_read(data_queue, data_head, data_tail, 0x0566, 2);
                
                    if(CapacitorVolt >= 0x16) // need to be tuned
                    {
                        state = HV_Enabled;
                        break;
                    }
                
                
                    PrechargingTimeCount++;
                
                    if (PrechargingTimeCount == 10)
                    {
                        state = Fault;
                        error_state = fromPrecharging;
                        break;
                    }
                
                    CyDelay(1000);
                }    
                
            break;
	        
            case HV_Enabled:

                CAN_GlobalIntEnable();
                CAN_Init();
                CAN_Start();
                
                //nodeCheckStart();
                
                can_send_status(state, error_state);
                
                //
                // RGB code goes here
                // Blue
                RGB3_1_Write(1);
                RGB2_1_Write(1);
                RGB1_1_Write(0);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);
                //CyDelay(5000); ///for debug
                
                Buzzer_Write(0);
                
                if (Drive_Read())
                {
                    CyDelay(1000); // wait for the brake msg to be sent
                    if(getErrorTolerance() == 1) // 100 for error tolerance /// needs to be getErrorTolerance
                    {
                        Buzzer_Write(1);
                        CyDelay(1000);
                        Buzzer_Write(0);
                        state = Drive;
                        break;
                    }
                    else
                    {
                        state = Fault;
                        error_state = fromHV_Enabled;
                        break;
                    }                     
                }
                
                // if capacitor voltage is undervoltage, change the threshold 0x16
                if(!HV_Read() | getCapacitorVoltage() < 0x16)
                {
                    state = LV;
                    DriveTimeCount = 0;
                    break;
                }
                
            break;
                
	        case Drive:
                //CAN_GlobalIntEnable();
                //CAN_Init();
                //CAN_Start();
                
                can_send_status(state, error_state);
                //
                // RGB code goes here
                // Green
                RGB3_1_Write(1);
                RGB2_1_Write(0);
                RGB1_1_Write(1);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);
                
                //CyDelay(10000); // debug
                
                //Buzzer_Write(1);
                //CyDelay(5000);
                //Buzzer_Write(0);
                
                //CyDelay(500);
                
                
                uint8_t ACK = 0xFF;
                
                DriveTimeCount++;
                if (DriveTimeCount > 100) //EDIT: was 100!
                {
                    DriveTimeCount = 0; 
                    ACK = getAckRx();
                }
   
                uint8_t ABS_Motor_RPM = getABSMotorRPM();
                uint8_t Throttle_High = getPedalHigh();//manga_getThrottleHigh(); // use 123 for pedal node place holder
                uint8_t Throttle_Low = getPedalLow();//manga_getThrottleLow();
                
                WaveDAC8_1_SetValue(ABS_Motor_RPM);
                can_send_cmd(1,Throttle_High,Throttle_Low); // setInterlock 
                
                //check if everything is going well
                if (!HV_Read())
                    state = LV;
                
                if (!Drive_Read())
                    state = HV_Enabled;
                
                if ((ACK != 0xFF) | 
                    (!getCurtisHeartBeatCheck())) // EDIT: Removed | CurtisFaultCheck from this 
                {
                    state = Fault;
                    error_state = fromDrive;
                    DriveTimeCount = 0;
                    break;
                }
                
                    
                // need to map ABS_Motor_RPM into 1-253 scale.
                
                /*
                if (ABS_Motor_RPM>=253)
                {
                    direction=-1;
                }
                else if (ABS_Motor_RPM<=1)
                {
                    direction=1;
                }
                value+=direction;
                //CyDelay(100);
                */
            break;
                
	        case Fault:
                
                CAN_GlobalIntEnable();
                CAN_Init();
                CAN_Start();
                //nodeCheckStart();
                
                can_send_status(state, error_state);
                
                //
                // RGB code goes here
                // flashing red
                RGB3_1_Write(1);
                RGB2_1_Write(1);
                RGB1_1_Write(1);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);
                
                RGB3_1_Write(0);
                CyDelay(1000);
                RGB3_1_Write(1);
                CyDelay(1000);
                
                Buzzer_Write(0);
                
                if(error_state == fromLV)
                {
                    if(!Drive_Read())
                    {
                        state = LV;
                        error_state = OK;
                    }
                }
                else if (error_state == fromPrecharging)
                {
                    if(!Drive_Read() && !HV_Read())
                    {
                        state = LV;
                        error_state = OK;
                    }
                }
                else if (error_state == fromHV_Enabled)
                {
                    if(!Drive_Read())
                    {
                        state = HV_Enabled;
                        error_state = OK;
                    }
                }
                else if (error_state == fromDrive)
                {   
                    can_send_cmd(1,Throttle_High,Throttle_Low); // setInterlock
                    
                    CyDelay(200);
                    
                    // Curtis Come back online again without error
                    /*
                    if((getCurtisHeartBeatCheck())) // EDIT: Removed !(Curtis_Fault_Check(data_queue,data_head,data_tail) & 
                    {
                        state = LV;
                        error_state = OK;
                    }
                    else if(0xFF == getAckRx()) //ACK received
                    {
                        state = HV_Enabled;
                        error_state = OK;
                    } 
                    */
                }
                else if (error_state == nodeFailure)
                {
                    state = Fault;
                }
            break;
                
            default:
                RGB3_1_Write(1);
                RGB2_1_Write(1);
                RGB1_1_Write(1);
                
                RGB3_2_Write(1);
                RGB2_2_Write(1);
                RGB1_2_Write(1);

        }// end of switch
        
        
        //if (receiveMailboxNumber == CAN_RX_MAILBOX_switchStatus)
        //{
            //LCD_Position(1u, 3u);
            //send
            //if (CAN_RX_DATA_BYTE1(CAN_RX_MAILBOX_switchStatus) == SWITCH_PRESSED)
            //{
                /* Display received switch status on LCD */
            
                //LCD_PrintString("pressed ");

                /* Increase the PWM pulse width */
                //pulseWidthValue += PWM_PULSE_WIDTH_STEP;

                /* Send message with the new PWM pulse width */
                //dataPWM.byte[0u] = pulseWidthValue;
                //CAN_SendMsg(&messagePWM);

                /* Display value of PWM pulse width on LCD */
                //LCD_Position(0u, 14u);
                //LCD_PrintInt8(pulseWidthValue);

            //}
            //else
            //{
                /* Display received switch status on LCD */
            //    LCD_PrintString("released");
            //}
            //receiveMailboxNumber = 0xFFu;
        //}
        
        //received

        //if (receiveMailboxNumber == CAN_RX_MAILBOX_ADCdata)
        //{
        //    adcData = ((uint16) ((uint16) CAN_RX_DATA_BYTE1(CAN_RX_MAILBOX_ADCdata) << ONE_BYTE_OFFSET)) | 
        //    CAN_RX_DATA_BYTE2(CAN_RX_MAILBOX_ADCdata);
            
            /* Display received ADC data on LCD */
       //     sprintf(txData, "%u.%.3u", (adcData / 1000u), (adcData % 1000u));
        //    txData[DATA_SIZE - 1u] = (char8) '\0';
            
        //    LCD_Position(0u, 4u);
         //   LCD_PrintString(txData);
         //   receiveMailboxNumber = 0xFFu;
        //}
    } 
}

/*
// 0 for HV, 1 for Drive
static uint32 ReadSwSwitch(uint8_t choice)
{
    uint32 heldDown;
    uint32 swStatus;

    swStatus = 0u;  // Switch is not active 
    heldDown = 0u;  // Reset debounce counter

    
    if (choice)
        swStatus = Drive_Read();
    else
        swStatus = HV_Read();

    return (swStatus);
}
*/
/*******************************************************************************
* Function Name: ISR_CAN
********************************************************************************
*
* Summary:
*  This ISR is executed at a Receive Message event and set receiveMailboxNumber
*  global variable with receive message mailbox number.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
CY_ISR(ISR_CAN)
{   
    /* Clear Receive Message flag */
    CAN_INT_SR_REG.byte[1u] = CAN_RX_MESSAGE_MASK;

    /* Set the isrFlag */
    //isrFlag = 1u;    

    /* Acknowledges receipt of new message */
    CAN_RX_ACK_MESSAGE(CAN_RX_MAILBOX_0);

    ///* Clear Receive Message flag */
    //CAN_INT_SR_REG.byte[1u] = CAN_RX_MESSAGE_MASK;
    /* Switch Status message received */
   // if ((CY_GET_REG16((reg16 *) &CAN_BUF_SR_REG.byte[0u]) & CAN_RX_MAILBOX_0_SHIFT) != 0u)
   // {        
   //     receiveMailboxNumber = CAN_RX_MAILBOX_switchStatus;

        /* Acknowledges receipt of new message */
   //     CAN_RX_ACK_MESSAGE(CAN_RX_MAILBOX_switchStatus);
   // }

    /* ADC data message received */
   // if ((CY_GET_REG16((reg16 *) &CAN_BUF_SR_REG.byte[0u]) & CAN_RX_MAILBOX_1_SHIFT) != 0u)
   // {
   //     receiveMailboxNumber = CAN_RX_MAILBOX_ADCdata;

        /* Acknowledges receipt of new message */
   //     CAN_RX_ACK_MESSAGE(CAN_RX_MAILBOX_ADCdata);
   // }
}

/* [] END OF FILE */
