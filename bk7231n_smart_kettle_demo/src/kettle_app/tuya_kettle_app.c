/*
 * @file name: kettle_app.c
 * @Descripttion: The main application file
 * @Author: xjw
 * @email: fudi.xu@tuya.com
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-05-17 16:35:33
 * @LastEditors: xjw
 * @LastEditTime: 2021-05-30 15:21:28
 */
/* Includes ------------------------------------------------------------------*/
#include "uni_log.h"
#include "uni_time_queue.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_os_adapter.h"
#include "tuya_cloud_error_code.h"
#include "uni_thread.h"
#include "tuya_gpio.h"
#include <string.h>
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
#include "tuya_device.h"
#include "b3950.h"
#include "timer.h"
#include "tuya_kettle_app.h"
#include "gpio_control.h"
#include "ts02n_key.h"
#include "sys_timer.h"
/* Private define ------------------------------------------------------------*/
#define Default_Warm_Temperature  55
#define Boil_Temperature 97
#define Indoor_Temperature 30
#define Limiting_Temperature 105
#define ON 1
#define OFF 0
/* Private typedef -----------------------------------------------------------*/
typedef UINT8_T DP_ID_T;

typedef struct {
    unsigned int status;
    unsigned int cur_temperatue;
    unsigned int water_mode;
    unsigned int warm_temperature;
}KETTLE_USER_T;

typedef struct {
    DP_ID_T dp_id;
    
    bool power;
}DP_BOOL_T;

typedef struct {
    DP_ID_T dp_id;
    
    INT_T value;
}DP_VALUE_T;
    
/* Private function prototypes -----------------------------------------------*/
void app_kettle_thread(void);
void thread_init(void);
void get_temper_timer_cb(void);
void set_dp_boil_value(bool value);
VOID_T report_one_dp_status(int dp_id);
void key1_cb_fun();
void key2_cb_fun();
void key2_long_press_cb_fun();
void key1_Low_level_cb_fun();
void key2_Low_level_cb_fun();


/* Private variables ---------------------------------------------------------*/
TIMER_ID boil_timer = 1;
static enum water_type {
    tap_water = 0,
    clear_water
};
static enum work_status {
    nature = 0,
    boil,
    keep_warm_mode1,
    keep_warm_mode2,
};
DP_BOOL_T boil_s = {
    .dp_id = DP_BOIL,
    .power = 0,
};

DP_VALUE_T temper_s = {
    .dp_id = DP_TEMPER,
    .value = 0,
};

DP_VALUE_T temp_set_s = {
    .dp_id = DP_TEMP_SET,
    .value = 0,
};

DP_BOOL_T keep_warm_s = {
    .dp_id = DP_KEEP_WARM,
    .power = 0,
};

DP_VALUE_T fault_s = {
    .dp_id = DP_FAULT,
    .value = 0, 
};

DP_VALUE_T water_type_s = {
    .dp_id = DP_WATER_TYPE,
    .value = tap_water,
};
TS02N_KEY_DEF_S kettle_key_def_s = {
    .key_pin1 = TY_GPIOA_7,
    .key_pin2 = TY_GPIOA_8,
    .key1_cb = key1_cb_fun,
    .key2_cb = key2_cb_fun,
    .key1_long_press_cb = NULL,
    .key2_long_press_cb = key2_long_press_cb_fun,
    .key1_Low_level_cb = key1_Low_level_cb_fun,
    .key2_Low_level_cb = key2_Low_level_cb_fun,
    .long_time_set = 5000,
    .scan_time = 100,
};
KETTLE_USER_T kettle_work_information = {
    .status = nature,
    .cur_temperatue = 0,
    .water_mode = tap_water,
    .warm_temperature = Default_Warm_Temperature
};
TIMER_ID get_temper_timer;
STATIC MUTEX_HANDLE mutex = NULL;
static bool buzzer_flag = 1;
/* Private functions ---------------------------------------------------------*/
/**
 * @Function: kettle_init
 * @Description: smart kettle init
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T kettle_init()
{
    b3950_init();
    thread_init();
    kettle_gpio_init();
    ts02n_key_init(&kettle_key_def_s);
}


/**
 * @Function: thread_init
 * @Description: thread_init
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void thread_init(void)
{
    int rt = OPRT_OK;
    // create mutex
    if(NULL == mutex) {
        rt = tuya_hal_mutex_create_init(&mutex);
        if(OPRT_OK != rt) {
            PR_ERR("tuya_hal_mutex_create_init err:%d",rt);
            return rt;
        }
    }
    //A timer with a period of 5 second is used to capture the temperature
    rt = sys_add_timer(get_temper_timer_cb, NULL, &get_temper_timer);
    if(rt != OPRT_OK) {
        PR_ERR("add timer error!: %d", rt); 
        return;
    }else {
        rt = sys_start_timer(get_temper_timer,2000,TIMER_CYCLE);
        if(rt != OPRT_OK) {
            PR_ERR("start timer error!: %d", rt); 
            return;
        }
    }

    rt = tuya_hal_thread_create(NULL, "app_kettle_thread", 512*4, TRD_PRIO_4, app_kettle_thread, NULL);
    if (rt != OPRT_OK) {
        PR_ERR("Create app_kettle_thread error!: %d", rt); 
        return;
    }
}
/**
 * @Function: key1_cb_fun
 * @Description: The boil button handle function
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void key1_cb_fun()
{   
    //Button press prompt
    led1_set(OFF);
    buzzer_flag = 1;
    //Button press prompt
    
    if(get_kettle_work_status() == boil) { //if current status is boil, close boiling 
        set_kettle_work_status(nature);
        set_dp_boil_value(FALSE);
        report_one_dp_status(DP_BOIL);
    }else {
        set_kettle_work_status(boil);
        set_dp_boil_value(TRUE);
        report_one_dp_status(DP_BOIL);
    }



}
/**
 * @Function: key2_cb_fun
 * @Description: The keep warm button handle function
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void key2_cb_fun()
{
    //Button press prompt
    led2_set(OFF);
    buzzer_flag = 1;
    //Button press prompt
    if(get_kettle_work_status() == keep_warm_mode1) {//if current status is keep_warm_mode1,turn off insulation function
        set_kettle_work_status(nature);
        set_dp_keep_warm_switch(0);
        report_one_dp_status(DP_KEEP_WARM);
    }else {
        set_kettle_work_status(keep_warm_mode1);
        //tap water warm mode :1.boil -> 2.keep warm
        set_kettle_keep_warm_temper(Default_Warm_Temperature);
        set_dp_keep_warm_switch(1);
        report_one_dp_status(DP_KEEP_WARM);
        report_one_dp_status(DP_TEMP_SET);
    }



}
/**
 * @Function: key2_long_press_cb_fun
 * @Description: key2 long press trigger network connection
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void key2_long_press_cb_fun()
{

    led2_set(OFF);
    tuya_iot_wf_gw_unactive();
    buzzer_flag = 1;

}
/**
 * @Function: key1_Low_level_cb_fun
 * @Description: Triggered when key1 is low level  Button indicator lights up
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void key1_Low_level_cb_fun()
{
    PR_DEBUG("key1_Low_level_cb");
    led1_set(ON);
    if(buzzer_flag) {
        timer_init();
        buzzer_flag = 0;
    }
    
}
/**
 * @Function: key2_Low_level_cb_fun
 * @Description: Triggered when key2 is low level  Button indicator lights up
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void key2_Low_level_cb_fun()
{
    led2_set(ON);
    PR_DEBUG("key2_Low_level_cb");
    if(buzzer_flag) {
        timer_init();
        buzzer_flag = 0;
    }
}

/**
 * @Function: set_dp_keep_warm_switch
 * @Description: set keep warm switch on/off
 * @Input: bool value
 * @Output: none
 * @Return: none
 * @Others: 
 */
void set_dp_keep_warm_switch(bool value)
{

    tuya_hal_mutex_lock(mutex);
    keep_warm_s.power = value;
    tuya_hal_mutex_unlock(mutex);

}
/**
 * @Function: set_dp_fault_status
 * @Description: set_dp_fault_status
 * @Input: bool status
 * @Output: none
 * @Return: none
 * @Others: 
 */
void set_dp_fault_status(bool status)
{
    tuya_hal_mutex_lock(mutex);
    fault_s.value = status;
    tuya_hal_mutex_unlock(mutex);
}
/**
 * @Function: set_dp_boil_value
 * @Description: set_dp_boil_value
 * @Input: bool value
 * @Output: none
 * @Return: none
 * @Others: 
 */
void set_dp_boil_value(bool value)
{
    tuya_hal_mutex_lock(mutex);
    boil_s.power = value;
    tuya_hal_mutex_unlock(mutex);
}
/**
 * @Function: set_kettle_keep_warm_temper
 * @Description: set_kettle_keep_warm_temper
 * @Input: int status
 * @Output: none
 * @Return: none
 * @Others: 
 */
void set_kettle_keep_warm_temper(int value)
{
    if(44 < value < 91) {
        tuya_hal_mutex_lock(mutex);
        temp_set_s.value = value;
        kettle_work_information.warm_temperature = value;
        tuya_hal_mutex_unlock(mutex);
        PR_DEBUG("set keep warm temper:%d",value);
    }
}
/**
 * @Function: set_kettle_work_status
 * @Description: set_kettle_work_status
 * @Input: int status
 * @Output: none
 * @Return: none
 * @Others: 
 */
void set_kettle_work_status(int status)
{
    tuya_hal_mutex_lock(mutex);
    kettle_work_information.status = status;
    tuya_hal_mutex_unlock(mutex);
}
/**
 * @Function: set_current_temperature
 * @Description: set_current_temperature
 * @Input: int status
 * @Output: none
 * @Return: none
 * @Others: 
 */
void set_current_temperature(int temperature)
{
    tuya_hal_mutex_lock(mutex);
    kettle_work_information.cur_temperatue = temperature;
    tuya_hal_mutex_unlock(mutex);
}
/**
 * @Function: get_dp_fault_status
 * @Description: get_dp_fault_status
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
int get_dp_fault_status()
{
    return fault_s.value;
}
/**
 * @Function: get_kettle_work_status
 * @Description: get_kettle_work_status
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
int get_kettle_work_status()
{
    return kettle_work_information.status;
}
/**
 * @Function: get_water_temperature
 * @Description: get_water_temperature
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
int get_water_temperature()
{
    return kettle_work_information.cur_temperatue;
}
/**
 * @Function: get_keep_wram_temperature
 * @Description: get_keep_wram_temperature
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
int get_keep_wram_temperature()
{
    return kettle_work_information.warm_temperature;
}
/**
 * @Function: get_water_type
 * @Description: get water type
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
int get_water_type()
{
    return kettle_work_information.water_mode;
}
/**
 * @brief: boil timer task
 * @param {none}
 * @retval: none
 */
STATIC VOID boil_time_task(VOID)
{
    /*Reaches near the boiling point and reheat for 5 seconds, then turn off the device*/
    set_kettle_work_status(nature); //Switch to cool mode
    relay_set(OFF);
    led1_set(OFF);
    led2_set(OFF);  
    state_led_set(OFF); //Turn off the light when the temperature does not reach the set value
    set_dp_boil_value(0);
    report_one_dp_status(DP_BOIL);
    PR_DEBUG("boil->nature");
}
/**
 * @Function: app_kettle_thread
 * @Description: smart kettle app handle function
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void app_kettle_thread(void)
{
    static int temp_work_status = 0;
    while(1)
    {
        temp_work_status = get_kettle_work_status();
        //According to the different Settings of the corresponding processing
        switch (temp_work_status) {
        
        case nature: {
                led1_set(OFF);
                led2_set(OFF);   //Turn on the light when the temperature does not reach the set value
                state_led_set(OFF); //Turn off the light when the temperature does not reach the set value

        }
        break;
        case boil: {
            if(get_water_temperature() >= Boil_Temperature) {
                int op_ret = -1;
                op_ret = sys_add_timer(boil_time_task,NULL,&boil_timer);
                if(op_ret != 0) {
                    PR_ERR("add timer err");
                    return;
                }
                op_ret = sys_start_timer(boil_timer,5000,TIMER_ONCE);
                //If the scheduled shutdown fails to start the task, turn off the heating immediately
                if(op_ret != 0) {
                    PR_ERR("start timer err");
                    set_kettle_work_status(nature); //Switch to cool mode
                    relay_set(OFF);
                    led1_set(OFF);
                    led2_set(OFF);  
                    state_led_set(OFF);
                    set_dp_boil_value(0);
                    report_one_dp_status(DP_BOIL);
                    PR_DEBUG("boil->nature");
                    return;
                }

            }else {
                relay_set(ON);
                led1_set(ON);
                led2_set(OFF);  
                state_led_set(OFF); //Turn off the light when the temperature does not reach the set value
            }
        }
        break;
        //tap water keep warm
        case keep_warm_mode1: {

            state_led_set(OFF);
            led1_set(ON);
            if(get_water_temperature() >= Boil_Temperature) {
                set_kettle_work_status(keep_warm_mode2);
                relay_set(OFF);
                led1_set(OFF);
                led2_set(ON);  
                state_led_set(OFF); 
                PR_DEBUG("keep_warm_mode1->keep_warm_mode2");
            }else {
                relay_set(ON);
                led1_set(OFF);
                led2_set(ON);  
                state_led_set(OFF); 
                PR_DEBUG("keep_warm_mode1 relay_set on");
            }

        }
        break;
        //clear water keep warm
        case keep_warm_mode2: {
            if(get_water_temperature() > (get_keep_wram_temperature() - 3)) {  //cur_temp > set temper  close the heating
                relay_set(OFF);
                led1_set(OFF);
                led2_set(ON);  
                state_led_set(OFF); 
                PR_DEBUG("keep_warm_mode2 relay_set 0");
            }else if(get_water_temperature() < (get_keep_wram_temperature() - 5)) { //cur_temp < set temper  open the heating
                relay_set(ON);
                led1_set(OFF);
                led2_set(ON);  
                state_led_set(OFF); 
                PR_DEBUG("keep_warm_mode2 relay_set 1");
            }else { // cur_temp == set temper
                state_led_set(ON); //It indicates that the temperature has reached the insulation value
                led1_set(OFF);
                led2_set(OFF);  
                relay_set(OFF);
            }
        }
        break;
        default:
        break;
        }
        //Over temperature protection, Limiting_Temperature:105  lack water warning
        if(get_water_temperature() > Limiting_Temperature) {
            timer_init(); //buzzer on , Voice call the police
            set_dp_fault_status(TRUE); //app display warning
            report_one_dp_status(DP_FAULT);
            relay_set(OFF); //turn off relay
        }else {
            if(get_dp_fault_status() == TRUE) {
                set_dp_fault_status(FALSE);
                report_one_dp_status(DP_FAULT);
            }
        }
        
        tuya_hal_system_sleep(1000);

    }

}

/**
 * @Function: get_temper_timer_cb
 * @Description: Timing task  ,get current water temperature
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
void get_temper_timer_cb(void)
{
        static int last_temper = 0;
        last_temper = cur_temper_get();
        if(last_temper > 150) {
            last_temper = 150;
        }
        else if(last_temper < 0) {
            last_temper = 0;
        }
        set_current_temperature(last_temper);
        tuya_hal_mutex_lock(mutex);
        temper_s.value = last_temper;
        tuya_hal_mutex_unlock(mutex);
        PR_DEBUG("last_temper : %d",last_temper);
        report_one_dp_status(DP_TEMPER);
}
/**
 * @Function: dp_water_type_handle
 * @Description: set water type  tap water/clear water
 * @Input: int value
 * @Output: none
 * @Return: none
 * @Others: 
 */
void dp_water_type_handle(int value)
{
    if(value == tap_water || value == clear_water) {
        tuya_hal_mutex_lock(mutex);
        kettle_work_information.water_mode = value;
        water_type_s.value = value;
        tuya_hal_mutex_unlock(mutex);
        if(get_kettle_work_status() == keep_warm_mode1 && get_water_type() == clear_water) {
            set_kettle_work_status(keep_warm_mode2);
        }else if(get_kettle_work_status() == keep_warm_mode2 && get_water_type() == tap_water) {
            set_kettle_work_status(keep_warm_mode1);
        }
        PR_DEBUG("water choose :%d",value);
        report_one_dp_status(DP_WATER_TYPE);
    }

}
/**
 * @Function: boil_handle
 * @Description:dp_boil handle function
 * @Input: IN BOOL_T bONOFF
 * @Output: none
 * @Return: none
 * @Others: 
 */
static void dp_boil_handle(IN BOOL_T bONOFF)
{
    if(bONOFF == TRUE) {
        set_kettle_work_status(boil);
        set_dp_boil_value(TRUE);
        set_dp_keep_warm_switch(FALSE);
        PR_DEBUG("dp_boil on");
    }else {
        set_kettle_work_status(nature);
        set_dp_boil_value(FALSE);
        PR_DEBUG("dp_boil off");
    }
    report_one_dp_status(DP_BOIL);
    report_one_dp_status(DP_KEEP_WARM);
}
/**
 * @Function: dp_keep_warm_set_handle
 * @Description:keep warm temperature set function
 * @Input: int value
 * @Output: none
 * @Return: none
 * @Others: 
 */
static void dp_keep_warm_set_handle(IN int value)
{
    set_kettle_keep_warm_temper(value);
    report_one_dp_status(DP_TEMP_SET);
}
/**
 * @Function: dp_keep_warm_handle
 * @Description:keep warm switch on/off 
 * @Input: IN BOOL_T bONOFF
 * @Output: none
 * @Return: none
 * @Others: 
 */
static void dp_keep_warm_handle(IN BOOL_T bONOFF)
{
    if(bONOFF == TRUE && (get_water_type() == tap_water)) {
        set_kettle_work_status(keep_warm_mode1);
        set_dp_keep_warm_switch(TRUE);
        set_dp_boil_value(FALSE);
        PR_DEBUG("keep_warm_mode1");
    }else if(bONOFF == TRUE && (get_water_type() == clear_water)){
        set_kettle_work_status(keep_warm_mode2);
        set_dp_keep_warm_switch(TRUE);
        set_dp_boil_value(FALSE);
        PR_DEBUG("keep_warm_mode2");
    }else {
        set_kettle_work_status(nature);
        set_dp_keep_warm_switch(FALSE);
        PR_DEBUG("close keep warm");
    }
    report_one_dp_status(DP_KEEP_WARM);
    report_one_dp_status(DP_BOIL);
}
/**
 * @Function: deal_dp_proc
 * @Description: Dp data trigger event execution function
 * @Input: IN CONST TY_OBJ_DP_S *root
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;
    dpid = root->dpid;
    
    switch (dpid){
    
    case DP_BOIL: {
        dp_boil_handle(root->value.dp_bool);
        
        }
        break;

    case DP_TEMP_SET: {
        dp_keep_warm_set_handle(root->value.dp_value);
        }
        break;

    case DP_KEEP_WARM: {
        dp_keep_warm_handle(root->value.dp_bool);

        }
        break;

    case DP_WATER_TYPE: {
        dp_water_type_handle(root->value.dp_enum);
        }
        break;

    default:
    
        break;
    }

}
/**
 * @Function: report_one_dp_status
 * @Description:update the dp data
 * @Input: int dp_id
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID_T report_one_dp_status(int dp_id)
{
    
    OPERATE_RET op_ret = OPRT_OK;
    GW_WIFI_NW_STAT_E wifi_state = 0xFF;
    op_ret = get_wf_gw_nw_status(&wifi_state);
    if (OPRT_OK != op_ret) {
        PR_ERR("get wifi state err");
        return;
    }
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return;
    }
    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }
    memset(dp_arr, 0, SIZEOF(TY_OBJ_DP_S));

    switch (dp_id){
    case DP_BOIL: {
        dp_arr[0].dpid = boil_s.dp_id;
        dp_arr[0].type = PROP_BOOL;
        dp_arr[0].time_stamp = 0;
        dp_arr[0].value.dp_bool = boil_s.power;

        }
        break;

    case DP_TEMPER: {
        dp_arr[0].dpid = temper_s.dp_id;
        dp_arr[0].type = PROP_VALUE;
        dp_arr[0].time_stamp = 0;
        dp_arr[0].value.dp_value = temper_s.value;
        }
        break;

    case DP_TEMP_SET: {
        dp_arr[0].dpid = temp_set_s.dp_id;
        dp_arr[0].type = PROP_VALUE;
        dp_arr[0].time_stamp = 0;
        dp_arr[0].value.dp_bool = temp_set_s.value;
        }
        break;

    case DP_KEEP_WARM: {
        dp_arr[0].dpid = keep_warm_s.dp_id;
        dp_arr[0].type = PROP_BOOL;
        dp_arr[0].time_stamp = 0;
        dp_arr[0].value.dp_enum = keep_warm_s.power;
        }
        break;

    case DP_WATER_TYPE : {
        dp_arr[0].dpid = water_type_s.dp_id;
        dp_arr[0].type = PROP_ENUM;
        dp_arr[0].time_stamp = 0;
        dp_arr[0].value.dp_enum = water_type_s.value;
        }
        break;


    case DP_FAULT: {
        dp_arr[0].dpid = fault_s.dp_id;
        dp_arr[0].type = PROP_ENUM;
        dp_arr[0].time_stamp = 0;
        dp_arr[0].value.dp_bool = fault_s.value;
        }
        break;
    default:
    
        break;
    }

    op_ret = dev_report_dp_json_async(NULL , dp_arr, 1);
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }
}
