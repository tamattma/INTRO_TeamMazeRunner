/**
 * \file
 * \brief Module to handle the LCD display
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module is driving the Nokia LCD Display.
 */

#include "Platform.h"
#if PL_CONFIG_HAS_LCD
#include "LCD.h"
#include "PDC1.h"
#include "GDisp1.h"
#include "GFONT1.h"
#include "FDisp1.h"
#include "Application.h"
#include "UTIL1.h"
#include "LCD_LED.h"
#include "Event.h"
#include "FRTOS1.h"
#include "RApp.h"
#include "LCDMenu.h"
/*! \todo Add additional includes as needed */
//#include "Snake.h"
#include "RNet_App.h"
/* status variables */
static bool LedBackLightisOn = TRUE;
static bool remoteModeIsOn = FALSE;
static bool requestLCDUpdate = FALSE;

#if PL_CONFIG_HAS_LCD_MENU
typedef enum {
  LCD_MENU_ID_NONE = LCDMENU_ID_NONE, /* special value! */
  LCD_MENU_ID_MAIN,
    LCD_MENU_ID_BACKLIGHT,
    LCD_MENU_ID_NUM_VALUE,
  //LCD_MENU_ID_SNAKE,
  LCD_MENU_ID_ROBOT,
  	  LCD_MENU_ID_SUMO_START_STOP,
	  	  LCD_MENU_ID_SUMO_STOP,
	  	  LCD_MENU_ID_SUMO_TRAP,
		  LCD_MENU_ID_SUMO_VOLLGAS,
		  LCD_MENU_ID_SUMO_DUMMY,
	  LCD_MENU_ID_BATTERY_VOLTAGE,
	  LCD_MENU_ID_MINT_TOF_SENSOR,
} LCD_MenuIDs;

static struct {
  struct {
    bool dataValid;
    bool isRunning;
    uint8_t str[sizeof("???????????")+1]; /* used to store menu string, either "Start" or "Stop" */
  } sumo;
  struct {
    bool dataValid;
    uint8_t mm[4]; /* ToF Sensor Values */
    uint8_t str[sizeof("D:??:??:??:??")+1]; /* used to store menu string */
  } tof;
  struct {
    bool dataValid;
    uint16_t centiV;
    uint8_t str[sizeof("Batt: ?.??V")+1]; /* used to store menu string */
  } battVoltage;
} remoteValues;

#if PL_CONFIG_HAS_RADIO
static LCDMenu_StatusFlags RobotRemoteMenuHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
  LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

  if (event==LCDMENU_EVENT_GET_TEXT && dataP!=NULL) {
    if (item->id==LCD_MENU_ID_MINT_TOF_SENSOR) {
      unsigned int i;

      UTIL1_strcpy(remoteValues.tof.str, sizeof(remoteValues.tof.str), (uint8_t*)"D:");
      for(i=0;i<sizeof(remoteValues.tof.mm);i++) {
        if (remoteValues.tof.dataValid) {
          UTIL1_strcatNum8Hex(remoteValues.tof.str, sizeof(remoteValues.tof.str), remoteValues.tof.mm[i]);
        } else {
          UTIL1_strcat(remoteValues.tof.str, sizeof(remoteValues.tof.str), (uint8_t*)"??");
        }
        if (i<sizeof(remoteValues.tof.mm)-1) {
          UTIL1_chcat(remoteValues.tof.str, sizeof(remoteValues.tof.str), ':');
        }
        *dataP = remoteValues.tof.str;
      }
      flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
    } else if (item->id==LCD_MENU_ID_SUMO_START_STOP) {
      if (remoteValues.sumo.dataValid) { /* have valid data */
//        if (remoteValues.sumo.isRunning) {
//          UTIL1_strcpy(remoteValues.sumo.str, sizeof(remoteValues.sumo.str), (uint8_t*)"Start/Stop");
//        } else {
//          UTIL1_strcpy(remoteValues.sumo.str, sizeof(remoteValues.sumo.str), (uint8_t*)"Start/Stop");
//        }
      } else { /* request values */
//        (void)RNETA_SendIdValuePairMessage(RAPP_MSG_TYPE_QUERY_VALUE, RAPP_MSG_TYPE_DATA_ID_START_STOP, 0, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
        /* use ??? for now until we get the response */
//        UTIL1_strcpy(remoteValues.sumo.str, sizeof(remoteValues.sumo.str), (uint8_t*)"Start/Stop?");
      }
      *dataP = remoteValues.sumo.str;
      flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
    } else if (item->id==LCD_MENU_ID_BATTERY_VOLTAGE) {
      UTIL1_strcpy(remoteValues.battVoltage.str, sizeof(remoteValues.battVoltage.str), (uint8_t*)"Batt: ");
      if (remoteValues.battVoltage.dataValid) { /* use valid data */
        UTIL1_strcatNum32sDotValue100(remoteValues.battVoltage.str, sizeof(remoteValues.battVoltage.str), remoteValues.battVoltage.centiV);
      } else { /* request value from robot */
        (void)RNETA_SendIdValuePairMessage(RAPP_MSG_TYPE_QUERY_VALUE, RAPP_MSG_TYPE_DATA_ID_BATTERY_V, 0, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
        /* use ??? for now until we get the response */
        UTIL1_strcat(remoteValues.battVoltage.str, sizeof(remoteValues.battVoltage.str), (uint8_t*)"?.??");
      }
      UTIL1_strcat(remoteValues.battVoltage.str, sizeof(remoteValues.battVoltage.str), (uint8_t*)"V");
      *dataP = remoteValues.battVoltage.str;
      flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
    }
  } else if (event==LCDMENU_EVENT_ENTER || event==LCDMENU_EVENT_RIGHT) { /* force update */
    uint16_t dataID = RAPP_MSG_TYPE_DATA_ID_NONE; /* default value, will be overwritten below */
    uint8_t msgType = 0;
    uint32_t value = 0;

    switch(item->id) {
      case LCD_MENU_ID_SUMO_START_STOP:
//        if (event==LCDMENU_EVENT_ENTER) {
//          msgType = RAPP_MSG_TYPE_REQUEST_SET_VALUE;
//          value = 1; /* start/stop */
//        } else {
//          msgType = RAPP_MSG_TYPE_QUERY_VALUE;
//          value = 0; /* don't care */
//        }
//        dataID = RAPP_MSG_TYPE_DATA_ID_START_STOP;
        break;
      case LCD_MENU_ID_MINT_TOF_SENSOR:
        remoteValues.tof.dataValid = FALSE;
        msgType = RAPP_MSG_TYPE_QUERY_VALUE;
        dataID = RAPP_MSG_TYPE_DATA_ID_TOF_VALUES;
        value = 0; /* don't care */
        break;
      case LCD_MENU_ID_BATTERY_VOLTAGE:
        remoteValues.battVoltage.dataValid = FALSE;
        msgType = RAPP_MSG_TYPE_QUERY_VALUE;
        dataID = RAPP_MSG_TYPE_DATA_ID_BATTERY_V;
        value = 0; /* don't care */
        break;
      case LCD_MENU_ID_SUMO_TRAP:
		  msgType = RAPP_MSG_TYPE_NOTIFY_VALUE;
		  dataID = RAPP_MSG_TYPE_DATA_ID_START_TRAP;
		  value = 0; /* don't care */
		  break;
      case LCD_MENU_ID_SUMO_VOLLGAS:
      		  msgType = RAPP_MSG_TYPE_NOTIFY_VALUE;
      		  dataID = RAPP_MSG_TYPE_DATA_ID_START_VOLLGAS;
      		  value = 0; /* don't care */
      		  break;
      case LCD_MENU_ID_SUMO_DUMMY:
      		  msgType = RAPP_MSG_TYPE_NOTIFY_VALUE;
      		  dataID = RAPP_MSG_TYPE_DATA_ID_START_DUMMY;
      		  value = 0; /* don't care */
      		  break;
      case LCD_MENU_ID_SUMO_STOP:
      		  msgType = RAPP_MSG_TYPE_NOTIFY_VALUE;
      		  dataID = RAPP_MSG_TYPE_DATA_ID_STOP;
      		  value = 0; /* don't care */
      		  break;
    }
    if (dataID!=RAPP_MSG_TYPE_DATA_ID_NONE) { /* request data */
      (void)RNETA_SendIdValuePairMessage(msgType, dataID, value, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
      flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
    }
  }
  return flags;
}
#endif

static LCDMenu_StatusFlags ValueChangeHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
  static int value = 0;
  static uint8_t valueBuf[16];
  LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

  (void)item;
  if (event==LCDMENU_EVENT_GET_TEXT) {
    UTIL1_strcpy(valueBuf, sizeof(valueBuf), (uint8_t*)"Val: ");
    UTIL1_strcatNum32s(valueBuf, sizeof(valueBuf), value);
    *dataP = valueBuf;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_GET_EDIT_TEXT) {
    UTIL1_strcpy(valueBuf, sizeof(valueBuf), (uint8_t*)"[-] ");
    UTIL1_strcatNum32s(valueBuf, sizeof(valueBuf), value);
    UTIL1_strcat(valueBuf, sizeof(valueBuf), (uint8_t*)" [+]");
    *dataP = valueBuf;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_DECREMENT) {
    value--;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_INCREMENT) {
    value++;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  }
  return flags;
}

static LCDMenu_StatusFlags BackLightMenuHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
  LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

  (void)item;
  if (event==LCDMENU_EVENT_GET_TEXT && dataP!=NULL) {
    if (LedBackLightisOn) {
      *dataP = "Backlight is ON";
    } else {
      *dataP = "Backlight is OFF";
    }
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  } else if (event==LCDMENU_EVENT_ENTER) { /* toggle setting */
    LedBackLightisOn = !LedBackLightisOn;
    flags |= LCDMENU_STATUS_FLAGS_HANDLED|LCDMENU_STATUS_FLAGS_UPDATE_VIEW;
  }
  return flags;
}

static LCDMenu_StatusFlags SnakeMenuHandler(const struct LCDMenu_MenuItem_ *item, LCDMenu_EventType event, void **dataP) {
	LCDMenu_StatusFlags flags = LCDMENU_STATUS_FLAGS_NONE;

	(void)item;
	if (event==LCDMENU_EVENT_ENTER) {
		SNAKE_Init();
		flags |= LCDMENU_STATUS_FLAGS_HANDLED;
	}
	return flags;
}

static const LCDMenu_MenuItem menus[] =
{/* id,                                    grp,pos,   up,                       down,                         text,           callback                      flags                  */
    {LCD_MENU_ID_MAIN,                      0,  0,  LCD_MENU_ID_NONE,       	LCD_MENU_ID_BACKLIGHT,        "General",      NULL,                       LCDMENU_MENU_FLAGS_NONE},
      {LCD_MENU_ID_BACKLIGHT,               1,  0,  LCD_MENU_ID_MAIN,       	LCD_MENU_ID_NONE,             NULL,           BackLightMenuHandler,       LCDMENU_MENU_FLAGS_NONE},
      {LCD_MENU_ID_NUM_VALUE,               1,  1,  LCD_MENU_ID_MAIN,       	LCD_MENU_ID_NONE,             NULL,           ValueChangeHandler,         LCDMENU_MENU_FLAGS_EDITABLE},
	  {LCD_MENU_ID_ROBOT,                   0,  1,  LCD_MENU_ID_NONE,         	LCD_MENU_ID_SUMO_START_STOP,  "Robot",        NULL,                       LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_SUMO_START_STOP,         2,  0,  LCD_MENU_ID_ROBOT,        	LCD_MENU_ID_SUMO_STOP,        "Sumo",         RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_SUMO_STOP,         		3,  0,  LCD_MENU_ID_SUMO_START_STOP,LCD_MENU_ID_NONE,             "STOP!!",       RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_SUMO_TRAP,         		3,  1,  LCD_MENU_ID_SUMO_START_STOP,LCD_MENU_ID_NONE,             "TRAP",         RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_SUMO_VOLLGAS,      		3,  2,  LCD_MENU_ID_SUMO_START_STOP,LCD_MENU_ID_NONE,             "VOLLGAS",      RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_SUMO_DUMMY,         		3,  3,  LCD_MENU_ID_SUMO_START_STOP,LCD_MENU_ID_NONE,             "DUMMY",        RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_BATTERY_VOLTAGE,         2,  1,  LCD_MENU_ID_ROBOT,        	LCD_MENU_ID_NONE,             NULL,           RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  {LCD_MENU_ID_MINT_TOF_SENSOR,         2,  2,  LCD_MENU_ID_ROBOT,        	LCD_MENU_ID_NONE,             NULL,           RobotRemoteMenuHandler,     LCDMENU_MENU_FLAGS_NONE},
	  #if PL_CONFIG_HAS_SNAKE_GAME
	{LCD_MENU_ID_SNAKE,						0,	1,	LCD_MENU_ID_NONE,		LCD_MENU_ID_NONE,				"Snake",		SnakeMenuHandler,			LCDMENU_MENU_FLAGS_NONE},
#endif
};

uint8_t LCD_HandleRemoteRxMessage(RAPP_MSG_Type type, uint8_t size, uint8_t *data, RNWK_ShortAddrType srcAddr, bool *handled, RPHY_PacketDesc *packet) {
  uint16_t msgID;
  uint32_t msgValue;

  (void)size;
  (void)packet;
  (void)srcAddr;
  switch(type) {
    case RAPP_MSG_TYPE_QUERY_VALUE_RESPONSE: /* receive data value */
      msgID = UTIL1_GetValue16LE(&data[0]); /* ID in little endian format */
      if (msgID==RAPP_MSG_TYPE_DATA_ID_TOF_VALUES) {
        *handled = TRUE;
        msgValue = UTIL1_GetValue32LE(&data[2]);
        remoteValues.tof.mm[3] = (msgValue>>24)&0xff;
        remoteValues.tof.mm[2] = (msgValue>>16)&0xff;
        remoteValues.tof.mm[1] = (msgValue>>8)&0xff;
        remoteValues.tof.mm[0] = msgValue&0xff;
        requestLCDUpdate = TRUE;
        remoteValues.tof.dataValid = TRUE;
//      } else if (msgID==RAPP_MSG_TYPE_DATA_ID_START_STOP){
//        *handled = TRUE;
//        msgValue = UTIL1_GetValue32LE(&data[2]);
//        remoteValues.sumo.isRunning = msgValue;
//        requestLCDUpdate = TRUE;
//        remoteValues.sumo.dataValid = TRUE;
      } else if (msgID==RAPP_MSG_TYPE_DATA_ID_BATTERY_V){
        *handled = TRUE;
        msgValue = UTIL1_GetValue32LE(&data[2]);
        remoteValues.battVoltage.centiV = msgValue;
        requestLCDUpdate = TRUE;
        remoteValues.battVoltage.dataValid = TRUE;
      }
      break;

    case RAPP_MSG_TYPE_NOTIFY_VALUE: /* receive notification value */
      msgID = UTIL1_GetValue16LE(&data[0]); /* ID in little endian format */
      if (msgID==RAPP_MSG_TYPE_DATA_ID_START_TRAP) {
//        *handled = FALSE;
//        msgValue = UTIL1_GetValue32LE(&data[2]);
//        remoteValues.sumo.isRunning = msgValue; /* 1: running, 0: not running */
//        requestLCDUpdate = TRUE;
//        remoteValues.sumo.dataValid = TRUE;

      }
      break;

    default:
      break;
  } /* switch */
  return ERR_OK;
}
#endif /* PL_CONFIG_HAS_LCD_MENU */


static void ShowTextOnLCD(unsigned char *text) {
  FDisp1_PixelDim x, y;

  GDisp1_Clear();
  x = 0;
  y = 10;
  FDisp1_WriteString(text, GDisp1_COLOR_BLACK, &x, &y, GFONT1_GetFont());
  GDisp1_UpdateFull();
}

static void LCD_Task(void *param) {
  (void)param; /* not used */
#if 1
  ShowTextOnLCD("Press a key!");
  GDisp1_DrawBox(0, 0,
		  	  	  GDisp1_GetWidth(),
				  GDisp1_GetHeight(),
				  1, GDisp1_COLOR_BLACK);
  GDisp1_UpdateFull();
#endif
#if PL_CONFIG_HAS_LCD_MENU
  LCDMenu_InitMenu(menus, sizeof(menus)/sizeof(menus[0]), 1);
  LCDMenu_OnEvent(LCDMENU_EVENT_DRAW, NULL);
#endif
  for(;;) {
    if (LedBackLightisOn) {
      LCD_LED_On(); /* LCD backlight on */
    } else {
      LCD_LED_Off(); /* LCD backlight off */
    }
#if PL_CONFIG_HAS_LCD_MENU
    if (requestLCDUpdate) {
      requestLCDUpdate = FALSE;
      LCDMenu_OnEvent(LCDMENU_EVENT_DRAW, NULL);
    }
#if 1 /*! \todo Change this to for your own needs, or use direct task notification */
    if (EVNT_EventIsSetAutoClear(LCD_BTN_LEFT)) { /* left */
      LCDMenu_OnEvent(LCDMENU_EVENT_LEFT, NULL);
//      ShowTextOnLCD("left");
    }
    if (EVNT_EventIsSetAutoClear(LCD_BTN_RIGHT)) { /* right */
      LCDMenu_OnEvent(LCDMENU_EVENT_RIGHT, NULL);
//      ShowTextOnLCD("right");
    }
    if (EVNT_EventIsSetAutoClear(LCD_BTN_UP)) { /* up */
      LCDMenu_OnEvent(LCDMENU_EVENT_UP, NULL);
//      ShowTextOnLCD("up");
    }
    if (EVNT_EventIsSetAutoClear(LCD_BTN_DOWN)) { /* down */
      LCDMenu_OnEvent(LCDMENU_EVENT_DOWN, NULL);
//      ShowTextOnLCD("down");
    }
    if (EVNT_EventIsSetAutoClear(LCD_BTN_CENTER)) { /* center */
      LCDMenu_OnEvent(LCDMENU_EVENT_ENTER, NULL);
//      ShowTextOnLCD("center");
    }
    if (EVNT_EventIsSetAutoClear(LCD_SIDE_BTN_UP)) { /* side up */
      LCDMenu_OnEvent(LCDMENU_EVENT_UP, NULL);
//      ShowTextOnLCD("side up");
    }
    if (EVNT_EventIsSetAutoClear(LCD_SIDE_BTN_DOWN)) { /* side down */
      LCDMenu_OnEvent(LCDMENU_EVENT_DOWN, NULL);
//      ShowTextOnLCD("side down");
    }
#endif
#endif /* PL_CONFIG_HAS_LCD_MENU */
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void LCD_Deinit(void) {
#if PL_CONFIG_HAS_LCD_MENU
  LCDMenu_Deinit();
#endif
}

void LCD_Init(void) {
  LedBackLightisOn =  TRUE;
  if (xTaskCreate(LCD_Task, "LCD", configMINIMAL_STACK_SIZE+100, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error! probably out of memory */
  }
#if PL_CONFIG_HAS_LCD_MENU
  LCDMenu_Init();
#endif
}

void LCD_SetEvent(LCD_BTN_Events event){
	switch(event){
	case LCD_BTN_LEFT:
#if PL_CONFIG_HAS_LCD_MENU
		LCDMenu_OnEvent(LCDMENU_EVENT_LEFT,NULL);
#endif
		break;

	case LCD_BTN_RIGHT:
#if PL_CONFIG_HAS_LCD_MENU
		LCDMenu_OnEvent(LCDMENU_EVENT_RIGHT,NULL);
#endif
		break;

	case LCD_BTN_UP:
#if PL_CONFIG_HAS_LCD_MENU
		LCDMenu_OnEvent(LCDMENU_EVENT_UP,NULL);
#endif
		break;

	case LCD_BTN_DOWN:
#if PL_CONFIG_HAS_LCD_MENU
		LCDMenu_OnEvent(LCDMENU_EVENT_DOWN,NULL);
#endif
		break;

	case LCD_BTN_CENTER:
#if PL_CONFIG_HAS_LCD_MENU
		LCDMenu_OnEvent(LCDMENU_EVENT_ENTER,NULL);
#endif
		break;
	}
}

#endif /* PL_CONFIG_HAS_LCD */
