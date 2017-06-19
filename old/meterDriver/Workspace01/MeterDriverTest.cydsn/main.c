/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    uint8_t value=0;
    int8_t direction=1;
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    WaveDAC8_1_Start();
    for(;;)
    {
        /* Place your application code here. */
        
        WaveDAC8_1_SetValue(value);
        if (value>=253){
            direction=-1;
        }else if (value<=1){
            direction=1;
        }
        value+=direction;
        CyDelay(100);
        
        
    }
        
}

/* [] END OF FILE */
