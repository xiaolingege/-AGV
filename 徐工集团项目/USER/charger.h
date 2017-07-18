#ifndef _CHANGER_H
#define _CHANGER_H

#include "stm32f10x.h"
#include "can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

typedef enum { CLOSE = 0, OPEN = 1 } _CHARGER_STATUS;
typedef enum { module0 = 0, module1 = 1, module2 = 2, no_module = 3 } _CHANGER_MODULE;
//配置充电电压参数命令
#define _SET_VOLATE_CMD 0xf1, 0x02, 0x04, 0x21
#define _SET_VOLATE_CMD_BACK 0xf1020421ul
//配置充电电压参数数据
#define _SET_VOLATE_DATA 0xf1, 0x12, 0x0e, 0xb3
#define _SET_VOLATE_DATA_BACK 0xf1120eb3ul
//配置充电电流参数命令
#define _SET_CURR_CMD 0xf1, 0x02, 0x04, 0x23
#define _SET_CURR_CMD_BACK 0xf1020423ul
//配置充电电流参数数据
#define _SET_CURR_DATA 0xf1, 0x12, 0x0a, 0x00
#define _SET_CURR_DATA_BACK 0xf1120a00ul
//打开充电机命令
#define _OPEN_CMD 0xf1, 0x02, 0x04, 0x87
#define _OPEN_CMD_BACK 0xf1020487ul
//打开充电机数据
#define _OPEN_DATA 0xf1, 0x12, 0x00, 0x00
#define _OPEN_DATA_BACK 0xf1120000ul
//关闭充电机命令
#define _CLOSE_CMD 0xf1, 0x02, 0x04, 0x87
#define _CLODE_CMD_BACK 0xf1020487ul
//关闭充电机数据
#define _CLOSE_DATA 0xf1, 0x12, 0x00, 0x01
#define _CLOSE_DATA_BACK 0xf1120001ul
//检测电池电压命令
#define _CHECK_BATTRY_CMD 0xf1, 0x01, 0x04, 0x31
#define _CHECK_BATTRY_CMD_BACK 0xf1010431ul
//检测电池电压数据
#define _CHECK_BATTY_DATA 0xf1, 0x31, 0x04, 0x31
//检测充电电流命令
#define _CHECK_CURR_CMD 0xf1, 0x01, 0x04, 0x30
#define _CHECK_CURR_CMD_BACK 0xf1010430ul
//检测充电电流数据
#define _CHECK_CURR_DATA 0xf1, 0x31, 0x04, 0x30

#define _CHECK_VOLATE_CMD 0xf1, 0x01, 0x04, 0x32
#define _CHECK_VOLATE_CMD_BACK 0xf1010432ul

#define _CHECK_VOLATE_DATA 0xf1, 0x31, 0x04, 0x32

#define _CONNECT_CMD 0xf1, 0x01, 0x04, 0x30

//static void setCloseDelay(void);
extern u8 chargerCTRLLoop(void);
static bool isOnConnect(void);
static bool isCloseDelay(void);
bool isChargerNotGood(void);
static bool isChargeOver(void);

static s8 openCharger(void);
static s8 closeCharger(void);
static s8 setCharger(void);
static bool isBattryVolGood(void);
static bool isCurGood(void);
static bool isOverCharge(void);
static bool isEmmergency(void);
static _CHARGER_STATUS checkChangerStatusOpen(void);
static void setChangerOver(void);
//static void clrChargerOver(void);
static void setChangerMoudle(_CHANGER_MODULE mode);
extern u16 getCurr(void);
#endif // !_CHANGER_H
