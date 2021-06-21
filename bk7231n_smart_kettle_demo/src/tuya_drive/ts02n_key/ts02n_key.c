/*
 * @Author: xjw
 * @email: fudi.xu@tuya.com
 * @LastEditors: xjw
 * @file name: ts02n_key.c
 * @Description: key drive file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-5-19 15:22:03
 * @LastEditTime: 2021-05-29 15:35:04
 */
/* Private includes ----------------------------------------------------------*/
#include "ts02n_key.h"
#include "uni_log.h"
#include "uni_time_queue.h"
#include "tuya_gpio.h"
/* Private define ---------------------------------------------------------*/
#define KEY_INIT_ERR -1
#define KEY_INIT_OK 0
/* Private typedef ---------------------------------------------------------*/

typedef struct {
    unsigned int down_time; 
    unsigned char status;   

}KEY_PRESS_STATUS_S;

typedef struct {
    TS02N_KEY_DEF_S* ts02n_def_s;
    KEY_PRESS_STATUS_S key_status;
}TS02N_KEY_MANAGE_S;
/* Private function prototypes -----------------------------------------------*/
static int key_scan();
static void key_timer_cb();

/* Private variables ---------------------------------------------------------*/
static TIMER_ID key_timer;
static TS02N_KEY_MANAGE_S *key_mag = NULL; 
enum KEY_STAT_E{
    KEY1_UP = 0,
    KEY1_DOWN,
    KEY1_FINISH,
    KEY2_UP,
    KEY2_DOWN,
    KEY2_FINISH,
};

enum KEY_PRESS_STAT{
    KEY1_NORMAL_PRESS = 0,
    KEY2_NORMAL_PRESS, 
    KEY1_LONG_PRESS,
    KEY2_LONG_PRESS,
    NO_KEY_PRESS,
};

/* Private functions ---------------------------------------------------------*/
/**
 * @Function: ts02n_key_init
 * @Description: key init 
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
int ts02n_key_init(TS02N_KEY_DEF_S* user_key_def)
{
    int op_ret = 0;
    key_mag = (TS02N_KEY_MANAGE_S *)Malloc(SIZEOF(TS02N_KEY_MANAGE_S));
    memset(key_mag, 0, SIZEOF(TS02N_KEY_MANAGE_S));
    //No callback function is registered    key init err
    if((user_key_def->key1_cb == NULL) && (user_key_def->key2_cb == NULL) && \
    (user_key_def->key1_long_press_cb == NULL) && (user_key_def->key2_long_press_cb == NULL)){
        return KEY_INIT_ERR;
    }
    //key pin init 
    tuya_pin_init(user_key_def->key_pin1, TUYA_PIN_MODE_IN_PU);
    tuya_pin_init(user_key_def->key_pin2, TUYA_PIN_MODE_IN_PU);
    //get user define
    key_mag->ts02n_def_s = user_key_def;
    //creat key scan handle timer
    op_ret = sys_add_timer(key_timer_cb,NULL,&key_timer);
    if(op_ret != KEY_INIT_OK) {
        return KEY_INIT_ERR;
        PR_ERR("add timer err");
    }
    op_ret = sys_start_timer(key_timer,key_mag->ts02n_def_s->scan_time,TIMER_CYCLE);
    if(op_ret != KEY_INIT_OK) {
        return KEY_INIT_ERR;
        PR_ERR("start timer err");
    }

}
/**
 * @Function: key_timer_cb
 * @Description: Timing key scan processing function
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
static void key_timer_cb()
{
    int ret = -1;
    //get key press status
    ret = key_scan();
    switch (ret) {
    
    case KEY1_NORMAL_PRESS: {
        key_mag->ts02n_def_s->key1_cb();
    }
    break;

    case KEY1_LONG_PRESS: {
        key_mag->ts02n_def_s->key1_long_press_cb();
    }
    break;

    case KEY2_NORMAL_PRESS: {
        key_mag->ts02n_def_s->key2_cb();
    }
    break;

    case KEY2_LONG_PRESS: {
        key_mag->ts02n_def_s->key2_long_press_cb();
    }
    break;
    default:
    break;
    }
}
/**
 * @Function: key_scan
 * @Description: key scan handle function
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
static int key_scan()
{
    if((tuya_pin_read(key_mag->ts02n_def_s->key_pin1) == 0) && (tuya_pin_read(key_mag->ts02n_def_s->key_pin2) == 0 )) {
        return NO_KEY_PRESS;
    }
    if(tuya_pin_read(key_mag->ts02n_def_s->key_pin1) == 0 ) {

        key_mag->ts02n_def_s->key1_Low_level_cb();
        key_mag->key_status.status = KEY1_DOWN;
        key_mag->key_status.down_time += key_mag->ts02n_def_s->scan_time;
        if(key_mag->key_status.down_time >= key_mag->ts02n_def_s->long_time_set) {
            key_mag->key_status.status = KEY1_FINISH;
        }
    }else {
        if(key_mag->key_status.status == KEY1_DOWN) {
            key_mag->key_status.status = KEY1_FINISH;
        }
    }

    if(tuya_pin_read(key_mag->ts02n_def_s->key_pin2) == 0 ) {

        key_mag->ts02n_def_s->key2_Low_level_cb();
        key_mag->key_status.status = KEY2_DOWN;
        key_mag->key_status.down_time += key_mag->ts02n_def_s->scan_time;
        if(key_mag->key_status.down_time >= key_mag->ts02n_def_s->long_time_set) {
            key_mag->key_status.status = KEY2_FINISH;
        }
    }else {
        if(key_mag->key_status.status == KEY2_DOWN) {
            key_mag->key_status.status = KEY2_FINISH;
        }
    }

    switch (key_mag->key_status.status) {
    
    case (KEY1_FINISH): {
        if(key_mag->key_status.down_time >= key_mag->ts02n_def_s->long_time_set) {
            PR_DEBUG("key_pin1 long press");
            key_mag->key_status.down_time = 0;
            key_mag->key_status.status = KEY1_UP;
            return KEY1_LONG_PRESS;
        }else {
            key_mag->key_status.status = KEY1_UP;
            key_mag->key_status.down_time = 0;
            PR_DEBUG("key_pin1 press");
            return KEY1_NORMAL_PRESS;
        }
    }
    break;

    case (KEY2_FINISH): {
        if(key_mag->key_status.down_time >= key_mag->ts02n_def_s->long_time_set) {
            PR_DEBUG("key_pin2 long press");
            key_mag->key_status.down_time = 0;
            key_mag->key_status.status = KEY2_UP;
            return KEY2_LONG_PRESS;
        }else {
            key_mag->key_status.status = KEY2_UP;
            key_mag->key_status.down_time = 0;
            PR_DEBUG("key_pin2 press");
            return KEY2_NORMAL_PRESS;
        }
    }
    break;
    default:
        return NO_KEY_PRESS;
    break;

    }

}