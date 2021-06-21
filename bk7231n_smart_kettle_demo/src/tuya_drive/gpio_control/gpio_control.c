/*
 * @file name: gpio_control.c
 * @Descripttion: gpio control file
 * @Author: xjw
 * @email: fudi.xu@tuya.com
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-05-17 16:45:15
 * @LastEditors: xjw
 * @LastEditTime: 2021-05-30 12:05:48
 */
#include "gpio_control.h"

#define led1_pin 24
#define led2_pin 26
#define buzzer_pin  17
#define relay_pin  14
/**
 * @Function: kettle_gpio_init
 * @Description: gpio init
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void kettle_gpio_init()
{
    //led pin init
    tuya_pin_init(led1_pin, TUYA_PIN_MODE_OUT_PP_HIGH);
    tuya_pin_init(led2_pin, TUYA_PIN_MODE_OUT_PP_HIGH);
    //buzzer pin init
    tuya_pin_init(buzzer_pin, TUYA_PIN_MODE_OUT_PP_LOW);
    //relay pin init
    tuya_pin_init(relay_pin, TUYA_PIN_MODE_OUT_PP_LOW);
}

/**
 * @Function: led1_set
 * @Description: 1.boil button prompt light 2.When the water temperature is not up to the requirements is lit
 * @Input: BOOL_T bONOFF
 * @Output: none
 * @Return: none
 * @Others: 
 */
void led1_set(IN     BOOL_T bONOFF)
{
    static bool last_status = 0;
    if(last_status != bONOFF) {
        if(bONOFF == TRUE) {
        tuya_pin_write(led1_pin, TUYA_PIN_LOW);
        }else {
        tuya_pin_write(led1_pin, TUYA_PIN_HIGH);
        }
        last_status = bONOFF;
    }

}
/**
 * @Function: led2_set
 * @Description: keep warm button prompt light
 * @Input: BOOL_T bONOFF
 * @Output: none
 * @Return: none
 * @Others: 
 */
void led2_set(IN     BOOL_T bONOFF)
{
    static bool last_status = 0;
    if(last_status != bONOFF) {
        if(bONOFF == TRUE) {
        tuya_pin_write(led2_pin, TUYA_PIN_LOW);
        }else {
        tuya_pin_write(led2_pin, TUYA_PIN_HIGH);
        }
        last_status = bONOFF;
    }
}
/**
 * @Function: buzzer_set
 * @Description: buzzer_set
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void buzzer_set(IN     BOOL_T bONOFF)
{
        if(bONOFF == TRUE) {
        tuya_pin_write(buzzer_pin, TUYA_PIN_HIGH);
        }else {
        tuya_pin_write(buzzer_pin, TUYA_PIN_LOW);
        }
}
/**
 * @Function: buzzer_set
 * @Description: buzzer_set
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void relay_set(IN     BOOL_T bONOFF)
{
    static bool last_status = 0;
    if(last_status != bONOFF) {
        if(bONOFF == TRUE) {
        tuya_pin_write(relay_pin, TUYA_PIN_HIGH);
        }else {
        tuya_pin_write(relay_pin, TUYA_PIN_LOW);
        }
        last_status = bONOFF;
    }
}