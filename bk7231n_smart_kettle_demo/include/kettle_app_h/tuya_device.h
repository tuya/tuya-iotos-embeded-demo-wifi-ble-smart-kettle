/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: xjw 
 * @file name: tuya_device.h
 * @Description: Application layer entry file header file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-05-17 15:10:00
 */

#ifndef _TUYA_DEVICE_H
#define _TUYA_DEVICE_H

/* Includes ------------------------------------------------------------------*/
#include "tuya_cloud_com_defs.h"
    
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
    
#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT 
#else
    #define _TUYA_DEVICE_EXT extern
#endif /* _TUYA_DEVICE_GLOBAL */ 
    
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/           
/* Exported macro ------------------------------------------------------------*/
// device information define
#define DEV_SW_VERSION USER_SW_VER
//#define PRODECT_ID "enri20q02gdaxtub"
#define PRODECT_ID "enri20q02gdaxtub"

/* Exported functions ------------------------------------------------------- */

/**
 * @Function: device_init
 * @Description: Device initialization processing
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
_TUYA_DEVICE_EXT \
OPERATE_RET device_init(VOID);
void state_led_set(IN     BOOL_T bONOFF);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* tuya_device.h */
