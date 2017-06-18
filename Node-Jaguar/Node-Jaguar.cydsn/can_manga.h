/* ========================================
 * Can API for FRUCD
 * Schuyler Alschuler 2016 - 2017
 * ========================================
*/

#ifndef CAN_MANGA_H
#define CAN_MANGA_H

#include "CAN.h"

// Basic CAN functionality
void can_receive(uint8_t *msg, int ID);
void can_test_send();
void can_send(uint8_t data[8], uint32_t ID);

// Advanced CAN functionality
void can_send_cmd(uint8_t SetInterlock, uint16_t VCL_Throttle_High, uint16_t VCL_Throttle_Low);
void can_send_status(uint8_t state, uint8_t errorState);

uint8_t getCapacitorVoltage();
uint8_t getCurtisFaultCheck();
uint8_t getCurtisHeartBeatCheck();
uint8_t getAckRx();
uint8_t getErrorTolerance();
uint8_t getABSMotorRPM();
uint8_t getPedalLow();
uint8_t getPedalHigh();

#endif

