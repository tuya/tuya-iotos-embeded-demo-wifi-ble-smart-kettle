/*
 * @file name: ts02n_key.h
 * @Descripttion: ts02n_key header file
 * @Author: xjw
 * @email: fudi.xu@tuya.com
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-04-26 16:45:15
 * @LastEditors: xjw
 * @LastEditTime: 2021-05-10 12:05:48
 */
#ifndef __TS02N_KEY_H__
#define __TS02N_KEY_H__

#ifdef __cplusplus
    extern "C" {
#endif
/*============================ INCLUDES ======================================*/


#include "tuya_pin.h"
#include "tuya_hal_system.h"


/*============================ MACROS ========================================*/
typedef VOID(* KEY_CALLBACK)();
typedef struct {   // user define
    unsigned int key_pin1; //ts02n CH1 PIN
    unsigned int key_pin2; //ts02n CH2 PIN
    KEY_CALLBACK key1_cb;  //normal press key1 callback function
    KEY_CALLBACK key2_cb;  //normal press key2 callback function
    KEY_CALLBACK key1_long_press_cb;  //normal press key1 callback function
    KEY_CALLBACK key2_long_press_cb;  //normal press key2 callback function
    KEY_CALLBACK key1_Low_level_cb;  //Key1 low level process callback function
    KEY_CALLBACK key2_Low_level_cb;  //Key2 low level process callback function
    unsigned int long_time_set; //long press key time set
    unsigned int scan_time; //Scan cycle set   unit: ms

}TS02N_KEY_DEF_S;
/*============================ MACROFIED FUNCTIONS ===========================*/
int ts02n_key_init(TS02N_KEY_DEF_S* user_key_def);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
