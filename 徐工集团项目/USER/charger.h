#ifndef _CHANGER_H
#define _CHANGER_H

#include "stm32f10x.h"
#include "can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

typedef enum {CLOSE = 0, OPEN = 1 } _CHANGER_STATUS;

static void setCloseDelay(void);
extern u8 changerCTRLLoop(void);
static bool isOnConnect(void);
static bool isCloseDelay(void);
static bool isChangerNotGood(void);
static bool isChargeOver(void);

static void openChanger(void);
static void closeChanger(void);
static void setChanger(void);
static bool isBattryVolGood(void);
static bool isCurGood(void);
static bool isOverCharge(void);


static bool isEmmergency(void);
static _CHANGER_STATUS checkChangerStatusOpen(void);
#endif // !_CHANGER_H
