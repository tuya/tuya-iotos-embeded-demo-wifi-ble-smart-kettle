/*
 * @Author: xjw 
 * @email: fudi.xu@tuya.com
 * @LastEditors: xjw 
 * @file name: kettle_app.h
 * @Description: The main application header file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-05-20 15:10:00
 */

#ifndef __KETTLE_APP_H__
#define __KETTLE_APP_H__
/* Includes ------------------------------------------------------------------*/
#include "tuya_cloud_types.h"
#include "tuya_iot_com_api.h"
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/              
/* Exported macro ------------------------------------------------------------*/

#define DP_BOIL           101
#define DP_TEMPER         102
#define DP_TEMP_SET       103
#define DP_KEEP_WARM      104
#define DP_WATER_TYPE     105
#define DP_FAULT          106
/* Exported functions ------------------------------------------------------- */
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root);
VOID_T report_one_dp_status(int dp_id);
VOID_T kettle_init();
//VOID_T time_open_handle(IN int value);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __KETTLE_APP_H__ */
