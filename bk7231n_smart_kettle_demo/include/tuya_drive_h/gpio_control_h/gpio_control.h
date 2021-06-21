/*
 * @file name: gpio_control.h
 * @Descripttion: gpio control file
 * @Author: xjw
 * @email: fudi.xu@tuya.com
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-04-26 16:45:15
 * @LastEditors: xjw
 * @LastEditTime: 2021-05-10 12:05:48
 */
#ifndef __GPIO_CONTROL_H__
#define __GPIO_CONTROL_H__

#ifdef __cplusplus
    extern "C" {
#endif
/*============================ INCLUDES ======================================*/


#include "tuya_pin.h"
#include "tuya_hal_system.h"


/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
void led1_set(IN     BOOL_T bONOFF);
void led2_set(IN     BOOL_T bONOFF);
void kettle_gpio_init();
void buzzer_set(IN     BOOL_T bONOFF);
void relay_set(IN     BOOL_T bONOFF);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
