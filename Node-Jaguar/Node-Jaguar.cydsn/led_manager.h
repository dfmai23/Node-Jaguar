
#include <project.h>

#define MAX_RPM		    6000	//motor rpm
#define TACH_MAX_STEPS	20	//number of led's to use
#define TACH_STEP_DELAY 50
//datasheet for address descriptions
//i2c addresses for led driver
#define LED_ADDR	0x40		//master address 100 0000, same addr for both drivers, diff pins

//register addresses
#define	LED05			0x05
#define LED06			0x06
#define LED07			0x07
#define LED08			0x08

//LED driver settings
#define serial_setting	0x00		//serial mode & PWM duty cycle
#define fade_setting	0x00		//LED fade out/in	0x00 no fade, 0x77 max fade 
#define RLED_curr		0x1F		//0x1F max current 30.72mA
#define GLED_curr		0x1F
#define BLED_curr		0x1F
#define output_setting	0x55		//output mode x55 = fade output for al led's
#define fade_fn			0xFF		//fade invalid(disable)=0, effective(enable)=1

CY_ISR_PROTO(b1_crit_isr);
CY_ISR_PROTO(b2_crit_isr);
CY_ISR_PROTO(b3_crit_isr);
CY_ISR_PROTO(c1_crit_isr);

void led_driver_init();
void led_write_tach(uint16_t MTR_RPM);
void led_write_b1(uint8_t BMS_TEMP);	//left vertical bar
void led_write_b2(uint8_t MTR_TEMP);	//center vertical bar
void led_write_b3(uint8_t MTR_CTRL_TEMP);	//right vertical bar
void led_write_c1(uint8_t SoC);	        //horizontal bar
void led_update_tach();
void led_update_stat();

void write_all_tach(int bool);
void write_all_stat(int bool);
void write_startup_tach();
void write_startup_stat();
void led_on();
void led_off();
int  led_writetest(int i, int* arr);

void d1_write(int bool);	//tach meter
void d2_write(int bool);
void d3_write(int bool);
void d4_write(int bool);
void d5_write(int bool);
void d6_write(int bool);
void d7_write(int bool);
void d8_write(int bool);
void d9_write(int bool);
void d10_write(int bool);
void d11_write(int bool);
void d12_write(int bool);
void d13_write(int bool);
void d14_write(int bool);
void d15_write(int bool);
void d16_write(int bool);
void d17_write(int bool);
void d18_write(int bool);
void d19_write(int bool);
void d20_write(int bool);
void d21_write(int bool);
void d22_write(int bool);

void b11_write(int bool);	//side vertical bars
void b12_write(int bool);
void b13_write(int bool);
void b14_write(int bool);
void b15_write(int bool);

void b21_write(int bool);
void b22_write(int bool);
void b23_write(int bool);
void b24_write(int bool);
void b25_write(int bool);

void b31_write(int bool);
void b32_write(int bool);
void b33_write(int bool);
void b34_write(int bool);
void b35_write(int bool);

void c1_write(int bool);	//center horizontal bar
void c2_write(int bool);
void c3_write(int bool);
void c4_write(int bool);
void c5_write(int bool);