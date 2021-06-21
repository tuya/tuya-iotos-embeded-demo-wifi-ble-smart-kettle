# Tuya IoTOS Embeded Demo WiFi & BLE  smart kettle

[English](./README.md) | [中文](./README_zh.md) 



## 简介 

本 Demo 基于涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded WiFi &Ble SDK，使用涂鸦WiFi/WiFi+BLE系列模组快速组建一个智能烧水壶demo。

## 快速上手 

### 开发环境搭建 

+ 安装linux开发环境

+ 下载[Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231n) 

+ 下载Demo至SDK目录的apps目录下 

  ```bash
  $ cd apps
  $ git clone https://github.com/tuya/tuya-iotos-embeded-demo-wifi-ble-smart-kettle.git
  ```



### 编译烧录

+ 代码修改

  1. 在 `tuya_device.h` 填入在涂鸦 IoT 工作台创建的智能产品 PID。
     ```c
     #define PRODECT_ID     "xxxxxxxx"
     ```
     将其中 `xxxxxxxx` 替换为PID。
  
+ 编译运行Demo代码

  在SDK根目录下执行以下命令开始编译：
  
  ```bash
  sh build_app.sh apps/bk7231n_smart_kettle_demo/ bk7231n_smart_kettle_demo 1.0.0
  ```
  
  固件烧录授权相关信息请参考：[Wi-Fi + BLE 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 



### 文件介绍 

|      文件名       |          功能           |
| :---------------: | :---------------------: |
|   tuya_device.c   |      工程入口文件       |
| tuya_kettle_app.c |    应用逻辑处理相关     |
|      b3950.c      | b3950温度传感器驱动相关 |
|  gpio_control.c   |      IO口控制相关       |
|    ts02n_key.c    |      触摸按键驱动       |
|      timer.c      |     定时器相关操作      |

<br>

### 应用入口
入口文件：tuya_device.c

重要函数：device_init()

+ 调用 tuya_iot_wf_soc_dev_init_param() 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。
+ 调用 tuya_iot_reg_get_wf_nw_stat_cb() 接口注册设备网络状态回调函数。
+ 调用应用层初始化函数 kettle_init()。

<br>

### dp点相关

+ 下发dp点数据流：dev_obj_dp_cb() -> deal_dp_proc()
+ 上报dp点接口: dev_report_dp_json_async()

|函数名 | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt)|
|	---|---|
|    devid | 设备id（若为网关、MCU、SOC类设备则devid = NULL;若为子设备，则devid = sub-device_id)|
|    dp_data | dp结构体数组名|
|    cnt |dp结构体数组的元素个数|
|    Return    |  OPRT_OK: 成功  Other: 失败 |

## 详细说明文档 

+ [涂鸦 Demo 中心](https://developer.tuya.com/demo)  

## 技术支持 

您可以通过以下方法获得涂鸦的支持:

+ 开发者中心：[https://developer.tuya.com]https://developer.tuya.com
+ 帮助中心: [https://support.tuya.com/help]https://support.tuya.com/help
+ 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/) 

