/*	Recieves CAN information for motor RPM, BMS temp, motor temp, motor controller temp 
	and SoC and displatys on LED's.
    
    Dustin Mai, 2016-2017 FRUCD
    
	tach = RPM
	b1 = BMS temp
	b2 = motor temp
	b3 = motor controller temp
	c1 = SoC
*/

#include "led_manager.h"

//temperature (C), step points for each 5-LED monitor set
int bms_temp_map[] = {0x40, 0x45, 0x50, 0x55, 0x60};
int mtr_temp_map[] = {50, 75, 100, 125, 150};
int mtr_ctrl_temp_map[] = {60, 70, 80, 90, 100};

//byte of LED address which holds which led's are on/off
uint8_t byte05 = 0x00;
uint8_t byte06 = 0x00;
uint8_t byte07 = 0x00;
uint8_t byte08 = 0x00;

uint8_t byte05_2 = 0x00;
uint8_t byte06_2 = 0x00;
uint8_t byte07_2 = 0x00;
uint8_t byte08_2 = 0x00;

//holds which led's are on/off
int tach_arr[22] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int b1_arr[5] = {0,0,0,0,0};
int b2_arr[5] = {0,0,0,0,0};
int b3_arr[5] = {0,0,0,0,0};
int c1_arr[5] = {0,0,0,0,0};

int tach_critical = 0;
int b1_critical = 0;
int b2_critical = 0;
int b3_critical = 0;
int c1_critical = 0;

int tach_crit_led = 0;  //led status to toggle them on/off when monitor reaches critical
int b1_crit_led = 0;	
int b2_crit_led = 0;
int b3_crit_led = 0;
int c1_crit_led = 0;

CY_ISR(tach_crit_isr) {
    tach_crit_led = tach_crit_led == 0 ? 1:0;
    d1_write(tach_crit_led);
    d2_write(tach_crit_led);
    d3_write(tach_crit_led);
    d4_write(tach_crit_led);
    d5_write(tach_crit_led);
    d6_write(tach_crit_led);
    d7_write(tach_crit_led);
    d8_write(tach_crit_led);
    d9_write(tach_crit_led);
    d10_write(tach_crit_led);
    d11_write(tach_crit_led);
    d12_write(tach_crit_led);
    d13_write(tach_crit_led);
    d14_write(tach_crit_led);
    d15_write(tach_crit_led);
    d16_write(tach_crit_led);
    d17_write(tach_crit_led);
    d18_write(tach_crit_led);
    d19_write(tach_crit_led);
    d20_write(tach_crit_led);
    d21_write(tach_crit_led);
    d22_write(tach_crit_led);
    led_update_tach(0);
}


CY_ISR(b1_crit_isr) {
	b1_crit_led = b1_crit_led == 0 ? 1:0;
	b11_write(b1_crit_led);
	b12_write(b1_crit_led);
	b13_write(b1_crit_led);
	b14_write(b1_crit_led);
	b15_write(b1_crit_led);
    led_update_stat();

}
CY_ISR(b2_crit_isr) {
	b2_crit_led = b2_crit_led == 0 ? 1:0;
	b21_write(b2_crit_led);
	b22_write(b2_crit_led);
	b23_write(b2_crit_led);
	b24_write(b2_crit_led);
	b25_write(b2_crit_led);
    led_update_stat();

}
CY_ISR(b3_crit_isr) {
	b3_crit_led = b3_crit_led == 0 ? 1:0;
	b31_write(b3_crit_led);
	b32_write(b3_crit_led);
	b33_write(b3_crit_led);
	b34_write(b3_crit_led);
	b35_write(b3_crit_led);
    led_update_stat();

}
CY_ISR(c1_crit_isr) {
	c1_crit_led = c1_crit_led == 0 ? 1:0;
	c1_write(c1_crit_led);
	c2_write(c1_crit_led);
	c3_write(c1_crit_led);
	c4_write(c1_crit_led);
	c5_write(c1_crit_led);
    led_update_stat();
}


void led_driver_init() {
	LED_Driver1_Start();
	LED_Driver2_Start();

	//tach settings
	LED_Driver1_MasterSendStart(LED_ADDR, 0);
	LED_Driver1_MasterWriteByte(0x00);				//move to 0x00 addr
	LED_Driver1_MasterWriteByte(serial_setting);	//0x00 addr
	LED_Driver1_MasterWriteByte(fade_setting);		//x01
	LED_Driver1_MasterWriteByte(RLED_curr);			//x02
	LED_Driver1_MasterWriteByte(GLED_curr);			//x03
	LED_Driver1_MasterWriteByte(BLED_curr);			//x04
	LED_Driver1_MasterSendStop();
	LED_Driver1_MasterSendStart(LED_ADDR, 0);
	LED_Driver1_MasterWriteByte(0x09);
	LED_Driver1_MasterWriteByte(output_setting);	//x09 addr
	LED_Driver1_MasterWriteByte(output_setting);	//x0A
	LED_Driver1_MasterWriteByte(output_setting);	//x0B
	LED_Driver1_MasterWriteByte(output_setting);	//x0C
	LED_Driver1_MasterWriteByte(output_setting);	//x0D
	LED_Driver1_MasterWriteByte(output_setting);	//x0E
	LED_Driver1_MasterWriteByte(fade_fn);			//x0F	LEDR
	LED_Driver1_MasterWriteByte(fade_fn);			//x10	LEDG
	LED_Driver1_MasterWriteByte(fade_fn);			//x11	LEDB
	LED_Driver1_MasterSendStop();
	
	//monitor settings
	LED_Driver2_MasterSendStart(LED_ADDR, 0);		//set mode
	LED_Driver2_MasterWriteByte(0x00);				//move to 0x00 addr
	LED_Driver2_MasterWriteByte(serial_setting);	//0x00 addr
	LED_Driver2_MasterWriteByte(fade_setting);		//x01
	LED_Driver2_MasterWriteByte(RLED_curr);			//x02
	LED_Driver2_MasterWriteByte(GLED_curr);			//x03
	LED_Driver2_MasterWriteByte(BLED_curr);			//x04
	LED_Driver2_MasterSendStop();
	LED_Driver2_MasterSendStart(LED_ADDR, 0);		//set output setting PWM
	LED_Driver2_MasterWriteByte(0x09);
	LED_Driver2_MasterWriteByte(output_setting);	//x09 addr
	LED_Driver2_MasterWriteByte(output_setting);	//x0A
	LED_Driver2_MasterWriteByte(output_setting);	//x0B
	LED_Driver2_MasterWriteByte(output_setting);	//x0C
	LED_Driver2_MasterWriteByte(output_setting);	//x0D
	LED_Driver2_MasterWriteByte(output_setting);	//x0E
	LED_Driver2_MasterWriteByte(fade_fn);			//x0F	LEDR
	LED_Driver2_MasterWriteByte(fade_fn);			//x10	LEDG
	LED_Driver2_MasterWriteByte(fade_fn);			//x11	LEDB
	LED_Driver2_MasterSendStop();
	
	write_all_tach(0);
	write_all_stat(0);
} //led_driver_init()


//calculates and writes RPM to tach led bar based on the maxed RPM
void led_write_tach(uint16_t MTR_RPM) {
	long tach_pct = MTR_RPM / MAX_RPM;	//tach percent
	int steps = 0;
    
    if(tach_pct < 100) { //not full rpm anymore
        if(tach_critical) {
            tach_critical = 0;
            tach_crit_int_Stop();
            tach_critical_timer_Stop();
            d22_write(0);   //actually not using last led for anything but full rpm blinking
            led_update_tach();
        } //if 
    } //if
    
	//using TACH_MAX_STEPS = 20 leds, 5% per step
    if (tach_pct == 0)
        steps = 0;
	else if(tach_pct > 0 && tach_pct <= 5)
		steps = 2;	//first step lights up first 2 led's
	else if(tach_pct > 5 && tach_pct <= 10)
		steps = 3;
	else if(tach_pct > 10 && tach_pct <= 15)
		steps = 4;
	else if(tach_pct > 15 && tach_pct <= 20)
		steps = 5;
	else if(tach_pct > 20 && tach_pct <= 25)
		steps = 6;
	else if(tach_pct > 25 && tach_pct <= 30)
		steps = 7;
	else if(tach_pct > 30 && tach_pct <= 35)
		steps = 8;
	else if(tach_pct > 35 && tach_pct <= 40)
		steps = 9;
	else if(tach_pct > 40 && tach_pct <= 45)
		steps = 10;
	else if(tach_pct > 45 && tach_pct <= 50)
		steps = 11;
	else if(tach_pct > 50 && tach_pct <= 55)
		steps = 12;
	else if(tach_pct > 55 && tach_pct <= 60)
		steps = 13;
	else if(tach_pct > 60 && tach_pct <= 65)
		steps = 14;
	else if(tach_pct > 65 && tach_pct <= 70)
		steps = 15;
	else if(tach_pct > 70 && tach_pct <= 75)
		steps = 16;
	else if(tach_pct > 75 && tach_pct <= 80)
		steps = 17;
	else if(tach_pct > 80 && tach_pct <= 85)
		steps = 18;
	else if(tach_pct > 85 && tach_pct <= 90)
		steps = 19;
	else if(tach_pct > 90 && tach_pct <= 95)
		steps = 20;
	else if(tach_pct > 95 && tach_pct <= 99)
		steps = 21;
    else { //(tach_pct = 100)
        if(!tach_critical) {	//start blinking
			tach_critical_timer_Start();
			tach_crit_int_StartEx(tach_crit_isr);
			tach_critical = 1;
		} //if
	} //else
	
	if(!tach_critical) {
    	int i;
    	for(i=0; i<steps; i++)
    		tach_arr[i] = 1;	//on leds
    	for(i=steps; i<TACH_MAX_STEPS+2; i++)	//+2 to get d1 and d22
    		tach_arr[i] = 0;	//off leds
    	
    	d1_write(tach_arr[0]);
    	d2_write(tach_arr[1]);
    	d3_write(tach_arr[2]);
    	d4_write(tach_arr[3]);
    	d5_write(tach_arr[4]);
    	d6_write(tach_arr[5]);
    	d7_write(tach_arr[6]);
    	d8_write(tach_arr[7]);
    	d9_write(tach_arr[8]);
    	d10_write(tach_arr[9]);
    	d11_write(tach_arr[10]);
    	d12_write(tach_arr[11]);
    	d13_write(tach_arr[12]);
    	d14_write(tach_arr[13]);
    	d15_write(tach_arr[14]);
    	d16_write(tach_arr[15]);
    	d17_write(tach_arr[16]);
    	d18_write(tach_arr[17]);
    	d19_write(tach_arr[18]);
    	d20_write(tach_arr[19]);
    	d21_write(tach_arr[20]);
    	d22_write(tach_arr[21]);
        led_update_tach();
    } //if
} //led_write_tach

void led_write_b1(uint8_t BMS_TEMP) {	//left vertical bar, 5 leds
    if(BMS_TEMP < mtr_temp_map[4]) { //temperature has cooled down, stop blinking
        if(b1_critical) {
            b1_critical = 0;
            b1_crit_int_Stop();
            b1_critical_timer_Stop();
        } //if 
    } //if

    int steps = 0;
	if (BMS_TEMP <= bms_temp_map[0])
		steps = 1;
	else if (BMS_TEMP > bms_temp_map[0] && BMS_TEMP <= bms_temp_map[1])
		steps = 2;
	else if (BMS_TEMP > bms_temp_map[1] && BMS_TEMP <= bms_temp_map[2])
		steps = 3;
	else if (BMS_TEMP > bms_temp_map[2] && BMS_TEMP <= bms_temp_map[3])
		steps = 4;
	else if (BMS_TEMP > bms_temp_map[3] && BMS_TEMP < bms_temp_map[4])
		steps = 5;
	else {	//(BMS_TEMP >= 60)
		if(!b1_critical) {	//start blinking
			b1_critical_timer_Start();
			b1_crit_int_StartEx(b1_crit_isr);
			b1_critical = 1;
		} //if
	} //else
	
	if(!b1_critical) {
		int i;
		for(i=0; i<steps; i++)
			b1_arr[i] = 1;	//on leds
		for(i=steps; i<5; i++)
			b1_arr[i] = 0;	//off leds
		
		b11_write(b1_arr[0]);
		b12_write(b1_arr[1]);
		b13_write(b1_arr[2]);
		b14_write(b1_arr[3]);
		b15_write(b1_arr[4]);
        led_update_stat();
	} //if
}	//led_write_b1()

void led_write_b2(uint8_t MTR_TEMP) {	//center vertical bar, 5 leds
    if(MTR_TEMP < mtr_temp_map[4]) { //temperature has cooled down, stop blinking
        if(b2_critical) {
            b2_critical = 0;
            b2_crit_int_Stop();
            b2_critical_timer_Stop();
        } //if 
    } //if
    
	int steps = 0;
	if (MTR_TEMP <= mtr_temp_map[0])
		steps = 1;
	else if (MTR_TEMP > mtr_temp_map[0] && MTR_TEMP <= mtr_temp_map[1])
		steps = 2;
	else if (MTR_TEMP > mtr_temp_map[1] && MTR_TEMP <= mtr_temp_map[2])
		steps = 3;
	else if (MTR_TEMP > mtr_temp_map[2] && MTR_TEMP <= mtr_temp_map[3])
		steps = 4;
	else if (MTR_TEMP > mtr_temp_map[3] && MTR_TEMP <= mtr_temp_map[4])
		steps = 5;
	else {	//(MTR_TEMP >= 60)
		if(!b2_critical) {	//start blinking
			b2_critical_timer_Start();
			b2_crit_int_StartEx(b2_crit_isr);
			b2_critical = 1;
		} //if
	} //else
	
	if (!b2_critical) {
		int i;
		for(i=0; i<steps; i++)
			b2_arr[i] = 1;	//on leds
		for(i=steps; i<5; i++)
			b2_arr[i] = 0;	//off leds
		
		b21_write(b2_arr[0]);
		b22_write(b2_arr[1]);
		b23_write(b2_arr[2]);
		b24_write(b2_arr[3]);
		b25_write(b2_arr[4]);
        led_update_stat();
	} //if
} //led_write_b2()

void led_write_b3(uint8_t MTR_CTRL_TEMP) {	//right vertical bar, 5 leds
    if(MTR_CTRL_TEMP < mtr_ctrl_temp_map[4]) { //temperature has cooled down, stop blinking
        if(b3_critical) {
            b3_critical = 0;
            b3_crit_int_Stop();
            b3_critical_timer_Stop();
        } //if 
    } //if
    
	int steps = 0;
	if (MTR_CTRL_TEMP <= mtr_ctrl_temp_map[0])
		steps = 1;
	else if (MTR_CTRL_TEMP > mtr_ctrl_temp_map[0] && MTR_CTRL_TEMP <= mtr_ctrl_temp_map[1])
		steps = 2;
	else if (MTR_CTRL_TEMP > mtr_ctrl_temp_map[1] && MTR_CTRL_TEMP <= mtr_ctrl_temp_map[2])
		steps = 3;
	else if (MTR_CTRL_TEMP > mtr_ctrl_temp_map[2] && MTR_CTRL_TEMP <= mtr_ctrl_temp_map[3])
		steps = 4;
	else if (MTR_CTRL_TEMP > mtr_ctrl_temp_map[3] && MTR_CTRL_TEMP <= mtr_ctrl_temp_map[4])
		steps = 5;
	else {	//(MTR_CTRL_TEMP >= 59)
		if(!b3_critical) {	//start blinking
			b3_critical_timer_Start();
			b3_crit_int_StartEx(b3_crit_isr);
			b3_critical = 1;
		} //if
	} 	//else
	
	if(!b3_critical) {
		int i;
		for(i=0; i<steps; i++)
			b3_arr[i] = 1;	//on leds
		for(i=steps; i<5; i++)
			b3_arr[i] = 0;	//off leds
		
		b31_write(b3_arr[0]);
		b32_write(b3_arr[1]);
		b33_write(b3_arr[2]);
		b34_write(b3_arr[3]);
		b35_write(b3_arr[4]);
        led_update_stat();
	} //if
} //led_write_b3()

void led_write_c1(uint8_t SoC) {	//horizontal bar, 5 leds
	int steps = 0;
	if (SoC > 80 && SoC < 100)
		steps = 5;
	else if (SoC > 60  && SoC < 80)
		steps = 4;
	else if (SoC > 40 && SoC < 60)
		steps = 3;
	else if (SoC > 20 && SoC < 40)
		steps = 2;
	else if (SoC > 10 && SoC < 20)
		steps = 1;
	else {	//(SoC <= 10)
		if(!c1_critical) {	//start blinking
			c1_critical_timer_Start();
			c1_crit_int_StartEx(c1_crit_isr);
			c1_critical = 1;
		} //if
	} //else
	
	if(!c1_critical) {
		int i;
		for(i=0; i<steps; i++)
			c1_arr[i] = 1;	//on leds
		for(i=steps; i<5; i++)
			c1_arr[i] = 0;	//off leds
		
		c1_write(c1_arr[0]);
		c2_write(c1_arr[1]);
		c3_write(c1_arr[2]);
		c4_write(c1_arr[3]);
		c5_write(c1_arr[4]);
        led_update_stat();
	} //if
} //led_write_c3()

void write_all_tach(int bool) {
	uint8_t allon = 0x77;
	
	LED_Driver1_MasterSendStart(LED_ADDR, 0);
	LED_Driver1_MasterWriteByte(LED05);
	if(bool == 1) {
		LED_Driver1_MasterWriteByte(allon);			//0x05 addr
		LED_Driver1_MasterWriteByte(allon);			//0x06 addr
		LED_Driver1_MasterWriteByte(allon);			//0x07 addr
		LED_Driver1_MasterWriteByte(allon);			//0x08 addr
	}
	else {
		LED_Driver1_MasterWriteByte(0x00);
		LED_Driver1_MasterWriteByte(0x00);
		LED_Driver1_MasterWriteByte(0x00);
		LED_Driver1_MasterWriteByte(0x00);
	}
	LED_Driver1_MasterSendStop();
}

void write_all_stat(int bool) {
	uint8_t allon = 0x77;
	
	LED_Driver2_MasterSendStart(LED_ADDR, 0);
	LED_Driver2_MasterWriteByte(LED05);	
	if(bool == 1) {
		LED_Driver2_MasterWriteByte(allon);			//0x05 addr
		LED_Driver2_MasterWriteByte(allon);			//0x06 addr
		LED_Driver2_MasterWriteByte(allon);			//0x07 addr
		LED_Driver2_MasterWriteByte(allon);			//0x08 addr
	}
	else {
		LED_Driver2_MasterWriteByte(0x00);
		LED_Driver2_MasterWriteByte(0x00);
		LED_Driver2_MasterWriteByte(0x00);
		LED_Driver2_MasterWriteByte(0x00);
	}
	LED_Driver2_MasterSendStop();
}

//display updated LED's to dashboard
void led_update_tach() {
	LED_Driver1_MasterSendStart(LED_ADDR, 0);	// 0 = send write command, 1 = send read command
	LED_Driver1_MasterWriteByte(LED05);		//move to LED addr
	LED_Driver1_MasterWriteByte(byte05);	//write byte to addr
    LED_Driver1_MasterWriteByte(byte06);
    LED_Driver1_MasterWriteByte(byte07);
    LED_Driver1_MasterWriteByte(byte08);
	LED_Driver1_MasterSendStop();  
}

void led_update_stat() {
	LED_Driver2_MasterSendStart(LED_ADDR, 0);	// 0 = send write command, 1 = send read command
	LED_Driver2_MasterWriteByte(LED05);		//move to LED addr
	LED_Driver2_MasterWriteByte(byte05_2);	//write byte to addr
    LED_Driver2_MasterWriteByte(byte06_2);
    LED_Driver2_MasterWriteByte(byte07_2);
    LED_Driver2_MasterWriteByte(byte08_2);
	LED_Driver2_MasterSendStop();
}


void write_startup_tach() { //based on max rpm = 6000
    //rev to max and back
    led_write_tach(0);      CyDelay(TACH_STEP_DELAY);
    led_write_tach(300);    CyDelay(TACH_STEP_DELAY);
    led_write_tach(600);    CyDelay(TACH_STEP_DELAY);
    led_write_tach(900);    CyDelay(TACH_STEP_DELAY);
    led_write_tach(1200);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(1500);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(1800);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(2100);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(2400);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(2700);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3000);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3300);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3600);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3900);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(4200);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(4500);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(4800);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(5100);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(5400);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(5700);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(6000);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(5700);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(5400);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(5100);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(4800);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(4500);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(4200);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3900);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3600);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3300);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(3000);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(2700);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(2400);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(2100);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(1800);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(1500);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(1200);   CyDelay(TACH_STEP_DELAY);
    led_write_tach(900);    CyDelay(TACH_STEP_DELAY);
    led_write_tach(600);    CyDelay(TACH_STEP_DELAY);
    led_write_tach(300);    CyDelay(TACH_STEP_DELAY);
    led_write_tach(0);      CyDelay(TACH_STEP_DELAY);
}
void write_startup_stat() {
    //slowly turns on all bars of each monitor at a time
    LED_Driver2_MasterSendStart(LED_ADDR, 0);       //max fade 
    LED_Driver2_MasterWriteByte(0x01);
    LED_Driver2_MasterWriteByte(0x77);
    LED_Driver2_MasterSendStop();
    
    led_write_b1(59);
    CyDelay(500);
    led_write_b2(59);
    CyDelay(500);
    led_write_b3(59);
    CyDelay(500);
    led_write_c1(99);
    
    LED_Driver2_MasterSendStart(LED_ADDR, 0);       //zero fade 
    LED_Driver2_MasterWriteByte(0x01);
    LED_Driver2_MasterWriteByte(0x00);
    LED_Driver2_MasterSendStop();
}


//test stuff
void led_on() {
	d1_write(1);
	//d2_write(1);
	//d3_write(1);
	//d4_write(1);
	//d5_write(1);
	//d6_write(1);
	//d7_write(1);
	//d8_write(1);
	//d9_write(1);
	//d10_write(1);
	//d11_write(1);
	//d12_write(1);
	//d13_write(1);
	d14_write(1);
	d15_write(1);
	//d16_write(1);
	//d17_write(1);
	//d18_write(1);
	d19_write(1);
	//d20_write(1);
	//d21_write(1);
	//d22_write(1);
	
	/*
	b11_write(1);
	b12_write(1);
	b13_write(1);
	b14_write(1);
	b15_write(1);
	
	b21_write(1);
	b22_write(1);
	b23_write(1);
	b24_write(1);
	b25_write(1);
	
	b31_write(1);
	b32_write(1);
	b33_write(1);
	b34_write(1);
	b35_write(1);
	*/
	/*
	c1_write(1);
	c2_write(1);
	c3_write(1);
	c4_write(1);
	c5_write(1);
	*/
}
void led_off() {
	d1_write(0);
	//d2_write(0);
	//d3_write(0);
	//d4_write(0);
	//d5_write(0);
	//d6_write(0);
	//d7_write(0);
	//d8_write(0);
	//d9_write(0);
	//d10_write(0);
	//d11_write(0);
	//d12_write(0);
	//d13_write(0);
	d14_write(0);
	d15_write(0);
	//d16_write(0);
	//d17_write(0);
	//d18_write(0);
	d19_write(0);
	//d20_write(0);
	//d21_write(0);
	//d22_write(0);
	/*
	b11_write(0);
	b12_write(0);
	b13_write(0);
	b14_write(0);
	b15_write(0);
	
	b21_write(0);
	b22_write(0);
	b23_write(0);
	b24_write(0);
	b25_write(0);
	
	b31_write(0);
	b32_write(0);
	b33_write(0);
	b34_write(0);
	b35_write(0);
	*/
	/*
	c1_write(0);
	c2_write(0);
	c3_write(0);
	c4_write(0);
	c5_write(0);
	*/
}
int led_writetest(int i, int* arr) {
	d1_write(arr[0]);
	d2_write(arr[1]);
	d3_write(arr[2]);
	d4_write(arr[3]);
	d5_write(arr[4]);
	d6_write(arr[5]);
	d7_write(arr[6]);
	d8_write(arr[7]);
	d9_write(arr[8]);
	d10_write(arr[9]);
	d11_write(arr[10]);
	d12_write(arr[11]);
	d13_write(arr[12]);
	d14_write(arr[13]);
	d15_write(arr[14]);
	d16_write(arr[15]);
	d17_write(arr[16]);
	d18_write(arr[17]);
	d19_write(arr[18]);
	d20_write(arr[19]);
	d21_write(arr[20]);
	d22_write(arr[21]);
	
	arr[i] = 0;
	arr[(i+5) % 22] = 1;
	i++;
	if(i > 21)
		i=0;
	return i;
}

//tach
void d1_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte05 = byte05 | bit;
	else			//led off
		byte05 = byte05 & ~bit;
}
void d2_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte05 = byte05 | bit;
	else			//led off
		byte05 = byte05 & ~bit;
}
void d3_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte06 = byte06 | bit;
	else
    //led off
		byte06 = byte06 & ~bit;
}
void d4_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte06 = byte06 | bit;
	else			//led off
		byte06 = byte06 & ~bit;
}
void d5_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte07 = byte07 | bit;
	else			//led off
		byte07 = byte07 & ~bit;
}
void d6_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte07 = byte07 | bit;
	else			//led off
		byte07 = byte07 & ~bit;
}
void d7_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte08 = byte08 | bit;
	else			//led off
		byte08 = byte08 & ~bit;
}
void d8_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte08 = byte08 | bit;
	else			//led off
		byte08 = byte08 & ~bit;
}
void d9_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte05 = byte05 | bit;
	else			//led off
		byte05 = byte05 & ~bit;
}
void d10_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte05 = byte05 | bit;
	else			//led off
		byte05 = byte05 & ~bit;
}
void d11_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte06 = byte06 | bit;
	else			//led off
		byte06 = byte06 & ~bit;
}
void d12_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte06 = byte06 | bit;
	else			//led off
		byte06 = byte06 & ~bit;
}
void d13_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte07 = byte07 | bit;
	else			//led off
		byte07 = byte07 & ~bit;
}
void d14_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte07 = byte07 | bit;
	else			//led off
		byte07 = byte07 & ~bit;
}
void d15_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte08 = byte08 | bit;
	else			//led off
		byte08 = byte08 & ~bit;
}
void d16_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte08 = byte08 | bit;
	else			//led off
		byte08 = byte08 & ~bit;
}
void d17_write(int bool) {
	uint8_t bit = 0x04;
	if(bool == 1) 	//led on
		byte05 = byte05 | bit;
	else			//led off
		byte05 = byte05 & ~bit;
}
void d18_write(int bool) {
	uint8_t bit = 0x40;
	if(bool == 1) 	//led on
		byte05 = byte05 | bit;
	else			//led off
		byte05 = byte05 & ~bit;
}
void d19_write(int bool) {
	uint8_t bit = 0x04;
	if(bool == 1) 	//led on
		byte06 = byte06 | bit;
	else			//led off
		byte06 = byte06 & ~bit;
}
void d20_write(int bool) {
	uint8_t bit = 0x40;
	if(bool == 1) 	//led on
		byte06 = byte06 | bit;
	else			//led off
		byte06 = byte06 & ~bit;
}
void d21_write(int bool) {
	uint8_t bit = 0x04;
	if(bool == 1) 	//led on
		byte07 = byte07 | bit;
	else			//led off
		byte07 = byte07 & ~bit;
}
void d22_write(int bool) {
	uint8_t bit = 0x40;
	if(bool == 1) 	//led on
		byte07 = byte07 | bit;
	else			//led off
		byte07 = byte07 & ~bit;
}

//side bars
//left vertical bar
void b11_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte05_2 = byte05_2 | bit;
	else			//led off
		byte05_2 = byte05_2 & ~bit;
}
void b12_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte05_2 = byte05_2 | bit;
	else			//led off
		byte05_2 = byte05_2 & ~bit;
}
void b13_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte06_2 = byte06_2 | bit;
	else			//led off
		byte06_2 = byte06_2 & ~bit;
}
void b14_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte06_2 = byte06_2 | bit;
	else			//led off
		byte06_2 = byte06_2 & ~bit;
}
void b15_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte07_2 = byte07_2 | bit;
	else			//led off
		byte07_2 = byte07_2 & ~bit;
}

//center vertical bar
void b21_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte07_2 = byte07_2 | bit;
	else			//led off
		byte07_2 = byte07_2 & ~bit;
}
void b22_write(int bool) {
	uint8_t bit = 0x01;
	if(bool == 1) 	//led on
		byte08_2 = byte08_2 | bit;
	else			//led off
		byte08_2 = byte08_2 & ~bit;
}
void b23_write(int bool) {
	uint8_t bit = 0x10;
	if(bool == 1) 	//led on
		byte08_2 = byte08_2 | bit;
	else			//led off
		byte08_2 = byte08_2 & ~bit;
}
void b24_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte05_2 = byte05_2 | bit;
	else			//led off
		byte05_2 = byte05_2 & ~bit;
}
void b25_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte05_2 = byte05_2 | bit;
	else			//led off
		byte05_2 = byte05_2 & ~bit;
}

//right bar
void b31_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte06_2 = byte06_2 | bit;
	else			//led off
		byte06_2 = byte06_2 & ~bit;
}
void b32_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte06_2 = byte06_2 | bit;
	else			//led off
		byte06_2 = byte06_2 & ~bit;
}
void b33_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte07_2 = byte07_2 | bit;
	else			//led off
		byte07_2 = byte07_2 & ~bit;
}
void b34_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte07_2 = byte07_2 | bit;
	else			//led off
		byte07_2 = byte07_2 & ~bit;
}
void b35_write(int bool) {
	uint8_t bit = 0x02;
	if(bool == 1) 	//led on
		byte08_2 = byte08_2 | bit;
	else			//led off
		byte08_2 = byte08_2 & ~bit;
}

//main center horizontal bar
void c5_write(int bool) {
	uint8_t bit = 0x20;
	if(bool == 1) 	//led on
		byte08_2 = byte08_2 | bit;
	else			//led off
		byte08_2 = byte08_2 & ~bit;
}
void c4_write(int bool) {
	uint8_t bit = 0x04;
	if(bool == 1) 	//led on
		byte05_2 = byte05_2 | bit;
	else			//led off
		byte05_2 = byte05_2 & ~bit;
}
void c3_write(int bool) {
	uint8_t bit = 0x40;
	if(bool == 1) 	//led on
		byte05_2 = byte05_2 | bit;
	else			//led off
		byte05_2 = byte05_2 & ~bit;
}
//right bar
void c2_write(int bool) {
	uint8_t bit = 0x04;
	if(bool == 1) 	//led on
		byte06_2 = byte06_2 | bit;
	else			//led off
		byte06_2 = byte06_2 & ~bit;
}
//right bar
void c1_write(int bool) {
	uint8_t bit = 0x40;
	if(bool == 1) 	//led on
		byte06_2 = byte06_2 | bit;
	else			//led off
		byte06_2 = byte06_2 & ~bit;
}