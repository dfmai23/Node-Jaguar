/* ========================================
 * Can API for FRUCD
 * Dustin Mai, Schuyler Alschuler 2016 - 2017
 * ========================================
*/

#include "CAN.h"
#include "can_manga.h"
#include "led_manager.h"

volatile uint8_t CAPACITOR_VOLT = 0;
volatile uint8_t CURTIS_FAULT_CHECK = 0;
volatile uint8_t CURTIS_HEART_BEAT_CHECK = 0;
volatile uint8_t ACK_RX = 0;
volatile uint8_t ERROR_TOLERANCE = 0;
volatile uint16_t ABS_MOTOR_RPM = 0;
volatile uint16_t THROTTLE_CHECK_BIT = 0;
volatile uint8_t THROTTLE_HIGH = 0;
volatile uint8_t THROTTLE_LOW = 0;
volatile uint8_t BMS_TEMP = 0x00;
volatile uint16_t MTR_TEMP = 0x00;
volatile uint16_t MTR_CTRL_TEMP = 0x00;
volatile uint32_t BMS_VOLTAGE = 0;
volatile uint8_t SoC = 0;

uint8_t getCapacitorVoltage() {
    return CAPACITOR_VOLT;
}

uint8_t getCurtisFaultCheck() {
    return CURTIS_FAULT_CHECK;
}

uint8_t getCurtisHeartBeatCheck() {
    return CURTIS_HEART_BEAT_CHECK;
}

uint8_t getAckRx() {
    return ACK_RX;
}

uint8_t getErrorTolerance() {
    return ERROR_TOLERANCE;
}

uint8_t getABSMotorRPM() {
    return ABS_MOTOR_RPM;
}

uint8_t getPedalLow() {
    return THROTTLE_LOW;
}

uint8_t getPedalHigh() {
    return THROTTLE_HIGH;
}

void can_receive(uint8_t *msg, int ID) {
    uint8 InterruptState = CyEnterCriticalSection();
    
    switch (ID) {
        case 0x566:    //Curtis Status 566
            CAPACITOR_VOLT = msg[CAN_DATA_BYTE_1];
            ABS_MOTOR_RPM = msg[CAN_DATA_BYTE_3] << 8;  //high byte
            ABS_MOTOR_RPM = ABS_MOTOR_RPM | msg[CAN_DATA_BYTE_4];       //low byte
            MTR_TEMP = msg[CAN_DATA_BYTE_5] << 8;
            MTR_TEMP = MTR_TEMP | msg[CAN_DATA_BYTE_6];
            //led_write_tach(ABS_MOTOR_RPM);
            //led_write_b2(MTR_TEMP);
            break;
        case 0x0A6:
            CURTIS_FAULT_CHECK = 0x1;
            break;
        case 0x726:
            CURTIS_HEART_BEAT_CHECK = 0x1;
            break;
        case 0x666:    //Curtis Recieve
            ACK_RX = msg[CAN_DATA_BYTE_1];
            MTR_CTRL_TEMP = msg[CAN_DATA_BYTE_7] << 8;
            MTR_CTRL_TEMP = MTR_CTRL_TEMP | msg[CAN_DATA_BYTE_8];
            //led_write_b3(MTR_CTRL_TEMP);
            break;
        case 0x201:
            ERROR_TOLERANCE = msg[CAN_DATA_BYTE_1];
            break;
        case 0x200: 
            THROTTLE_CHECK_BIT = msg[CAN_DATA_BYTE_1];
            THROTTLE_HIGH = msg[CAN_DATA_BYTE_2];
            THROTTLE_LOW = msg[CAN_DATA_BYTE_3];
            break;
        case 0x488:     //BMS_TEMP
            BMS_TEMP = msg[CAN_DATA_BYTE_8];    //recieved as hex, read in decimal
            led_write_b1(BMS_TEMP);
            break;
        case 0x247:     //sensor_status SoC pct
            SoC = msg[CAN_DATA_BYTE_7];
            led_write_c1(SoC);
            break;
        default:
            break;
    } //switch
    CyExitCriticalSection(InterruptState);
}

void can_test_send()
{
    //CAN_1_DATA_BYTES_MSG msg;
    CAN_TX_MSG HeartB;
    
    HeartB.id = 0x300;
    HeartB.rtr = 0;
    HeartB.ide = 0;
    HeartB.dlc = 1;
    
    HeartB.irq = 0;
    
    HeartB.msg->byte[0] = (uint8) 1;
    HeartB.msg->byte[1] = (uint8) 1;
    HeartB.msg->byte[2] = (uint8) 1;
    HeartB.msg->byte[3] = (uint8) 1;
    HeartB.msg->byte[4] = (uint8) 1;
    HeartB.msg->byte[5] = (uint8) 1;
    HeartB.msg->byte[6] = (uint8) 1;
    HeartB.msg->byte[7] = (uint8) 1;
    
    CAN_SendMsg(&HeartB);
}

void can_send(uint8_t data[8], uint32_t ID)
{
    uint8_t i;
	CAN_TX_MSG message;
	CAN_DATA_BYTES_MSG payload;
	message.id = ID; 
	message.rtr = 0;
	message.ide = 0;
	message.dlc = 0x08;
	message.irq = 0;
	message.msg = &payload;
	for(i=0;i<8;i++)
		payload.byte[i] = data[i];
	CAN_SendMsg(&message); 
}

void can_send_status(
    uint8_t state,
    uint8_t errorState)
{
    //max and min voltage means the voltage of single cell
        uint8_t data[8];
        
        data[0] = state;
        data[1] = errorState;
        
        data[2] = 0;
        data[3] = 0;
        
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;

        can_send(data, 0x626);

        
} // can_send_status()

void can_send_cmd(
    uint8_t SetInterlock,
    uint16_t VCL_Throttle_High,
    uint16_t VCL_Throttle_Low
)
{
    //max and min voltage means the voltage of single cell
        
        uint8_t data[8];
        
        data[0] = SetInterlock;
        
        data[1] = VCL_Throttle_High;
        data[2] = VCL_Throttle_Low;
        
        data[3] = 0;
        data[4] = 0;
        
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;

        can_send(data, 0x766);
        CyDelay(1);

} // can_send_cmd()



