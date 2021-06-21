/*
 * @file name: timer.c
 * @Descripttion: 
 * @Author: xjw
 * @email: fudi.xu@tuya.com
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-04-26 16:35:33
 * @LastEditors: xjw
 * @LastEditTime: 2021-05-10 15:21:28
 */
/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "gpio_control.h"
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static tuya_timer_t *timer = NULL;

/* Private function prototypes -----------------------------------------------*/
void tuya_timer0_cb(void *arg);
/* Private functions ---------------------------------------------------------*/

/**
 * @Function: timer_init
 * @Description: Adjust the buzzer sound
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void timer_init(void)
{
    timer = (tuya_timer_t *)tuya_driver_find(TUYA_DRV_TIMER, TUYA_TIMER1);
    //The timer uses cycle mode
    TUYA_TIMER_CFG(timer, TUYA_TIMER_MODE_PERIOD, tuya_timer0_cb, NULL);
    tuya_timer_init(timer);
    //Start the timer, 100us is a counting cycle
    tuya_timer_start(timer, 100);
    buzzer_set(1);


}
/**
 * @Function: timer_stop
 * @Description: stop timer0
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void timer_stop(void)
{
    tuya_timer_stop(timer);

}
/**
 * @Function: tuya_timer0_cb
 * @Description: tuya_timer0_cb
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void tuya_timer0_cb(void *arg)
{
    static uint32_t s_tick;
    static int level = 1;
    level = ~level;
    buzzer_set(level);
    if (s_tick++ >= 700) {
        s_tick = 0;
        tuya_timer_stop(timer);
        buzzer_set(0);
    }

}



