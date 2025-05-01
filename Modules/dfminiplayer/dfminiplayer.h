#ifndef _DFPLAYERMINI_H_
#define _DFPLAYERMINI_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include <stdbool.h>
// Choose a microcontroller family
//#define STM32F0
//#define STM32F1
#define STM32F4
//#define STM32L0
//#define STM32L1
//#define STM32L4
//#define STM32F3
//#define STM32H7
//#define STM32F7
//#define STM32G0


#if defined(STM32WB)
#include "stm32wbxx_hal.h"
#elif defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32L5)
#include "stm32l5xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
#else
#error "Board not recognized"
#endif

#define DFPLAYER_EQ_NORMAL 0
#define DFPLAYER_EQ_POP 1
#define DFPLAYER_EQ_ROCK 2
#define DFPLAYER_EQ_JAZZ 3
#define DFPLAYER_EQ_CLASSIC 4
#define DFPLAYER_EQ_BASS 5

#define DFPLAYER_DEVICE_U_DISK 1
#define DFPLAYER_DEVICE_SD 2
#define DFPLAYER_DEVICE_AUX 3
#define DFPLAYER_DEVICE_SLEEP 4
#define DFPLAYER_DEVICE_FLASH 5

#define DFPLAYER_RECEIVED_LENGTH 10
#define DFPLAYER_SEND_LENGTH 10

#define _DEBUG

#define TimeOut 0
#define WrongStack 1
#define DFPlayerCardInserted 2
#define DFPlayerCardRemoved 3
#define DFPlayerCardOnline 4
#define DFPlayerPlayFinished 5
#define DFPlayerError 6
#define DFPlayerUSBInserted 7
#define DFPlayerUSBRemoved 8
#define DFPlayerUSBOnline 9
#define DFPlayerCardUSBOnline 10
#define DFPlayerFeedBack 11

#define Busy 1
#define Sleeping 2
#define SerialWrongStack 3
#define CheckSumNotMatch 4
#define FileIndexOut 5
#define FileMismatch 6
#define Advertise 7

#define Stack_Header 0
#define Stack_Version 1
#define Stack_Length 2
#define Stack_Command 3
#define Stack_ACK 4
#define Stack_Parameter 5
#define Stack_CheckSum 7
#define Stack_End 9


struct DFPlayerMiniData
{
  GPIO_TypeDef *dfBusyPort;
  uint16_t dfBusyPin;

  unsigned long _timeOutTimer;
  //unsigned long _timeOutDuration = 500;
  unsigned long _timeOutDuration;
  
  uint8_t _received[DFPLAYER_RECEIVED_LENGTH];
  //uint8_t _sending[DFPLAYER_SEND_LENGTH] = {0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
  uint8_t _sending[DFPLAYER_SEND_LENGTH];
  
  //uint8_t _receivedIndex=0;
  uint8_t _receivedIndex;
  //uint8_t device = DFPLAYER_DEVICE_SD;
  uint8_t device;
  UART_HandleTypeDef df_uart;
  UART_HandleTypeDef debug_uart;

  
  uint8_t _handleType;
  uint8_t _handleCommand;
  uint16_t _handleParameter;
  //bool _isAvailable = false;
  bool _isAvailable;
  //bool _isSending = false;
  bool _isSending;
};

uint16_t DFPLayerMini_read(struct DFPlayerMiniData *dfp);
uint8_t DFPLayerMini_readType(struct DFPlayerMiniData *dfp);
int DFPLayerMini_readVolume(struct DFPlayerMiniData *dfp);
int DFPLayerMini_readEQ(struct DFPlayerMiniData *dfp);
int DFPLayerMini_readFileCounts2(struct DFPlayerMiniData *dfp, uint8_t device);
int DFPLayerMini_readCurrentFileNumber2(struct DFPlayerMiniData *dfp, uint8_t device);
int DFPLayerMini_readFileCountsInFolder(struct DFPlayerMiniData *dfp, int folderNumber);
int DFPLayerMini_readFolderCounts(struct DFPlayerMiniData *dfp);
int DFPLayerMini_readFileCounts(struct DFPlayerMiniData *dfp);
int DFPLayerMini_readCurrentFileNumber(struct DFPlayerMiniData *dfp);
int DFPLayerMini_readState(struct DFPlayerMiniData *dfp);
void DFPLayerMini_loop(struct DFPlayerMiniData *dfp,int fileNumber);
void DFPLayerMini_outputDevice(struct DFPlayerMiniData *dfp,uint8_t device);
void DFPLayerMini_sleep(struct DFPlayerMiniData *dfp);
void DFPLayerMini_reset(struct DFPlayerMiniData *dfp);
void DFPLayerMini_start(struct DFPlayerMiniData *dfp);
void DFPLayerMini_pause(struct DFPlayerMiniData *dfp);
void DFPLayerMini_playFolder(struct DFPlayerMiniData *dfp,uint8_t folderNumber, uint8_t fileNumber);
void DFPLayerMini_outputSetting(struct DFPlayerMiniData *dfp,bool enable, uint8_t gain);
void DFPLayerMini_enableLoopAll(struct DFPlayerMiniData *dfp);
void DFPLayerMini_disableLoopAll(struct DFPlayerMiniData *dfp);
void DFPLayerMini_playMp3Folder(struct DFPlayerMiniData *dfp,int fileNumber);
void DFPLayerMini_advertise(struct DFPlayerMiniData *dfp,int fileNumber);
void DFPLayerMini_playLargeFolder(struct DFPlayerMiniData *dfp,uint8_t folderNumber, uint16_t fileNumber);
void DFPLayerMini_stopAdvertise(struct DFPlayerMiniData *dfp);
void DFPLayerMini_stop(struct DFPlayerMiniData *dfp);
void DFPLayerMini_loopFolder(struct DFPlayerMiniData *dfp,int folderNumber);
void DFPLayerMini_randomAll(struct DFPlayerMiniData *dfp);
void DFPLayerMini_enableLoop(struct DFPlayerMiniData *dfp);
void DFPLayerMini_disableLoop(struct DFPlayerMiniData *dfp);
void DFPLayerMini_enableDAC(struct DFPlayerMiniData *dfp);
void DFPLayerMini_disableDAC(struct DFPlayerMiniData *dfp);
void DFPLayerMini_EQ(struct DFPlayerMiniData *dfp , uint8_t eq);
void DFPLayerMini_next(struct DFPlayerMiniData *dfp);
void DFPLayerMini_previous(struct DFPlayerMiniData *dfp);
void DFPLayerMini_play(struct DFPlayerMiniData *dfp,int fileNumber);
void DFPLayerMini_volumeUp(struct DFPlayerMiniData *dfp);
void DFPLayerMini_volumeDown(struct DFPlayerMiniData *dfp);
void DFPLayerMini_volume(struct DFPlayerMiniData *dfp, uint8_t volume);
void DFPLayerMini_setTimeOut(struct DFPlayerMiniData *dfp, unsigned long timeOutDuration);
bool DFPLayerMini_handleMessage(struct DFPlayerMiniData *dfp, uint8_t type, uint16_t parameter);
bool DFPLayerMini_handleError(struct DFPlayerMiniData *dfp, uint8_t type, uint16_t parameter);
bool DFPLayerMini_validateStack(struct DFPlayerMiniData *dfp);
bool DFPLayerMini_begin(struct DFPlayerMiniData *dfp, bool isACK , bool doReset);
void DFPLayerMini_parseStack(struct DFPlayerMiniData *dfp);
bool DFPLayerMini_available(struct DFPlayerMiniData *dfp);
bool DFPLayerMini_waitAvailable(struct DFPlayerMiniData *dfp , unsigned long duration);
void DFPLayerMini_sendStack(struct DFPlayerMiniData *dfp);
void DFPLayerMini_sendStack2(struct DFPlayerMiniData *dfp, uint8_t command);
void DFPLayerMini_sendStack3(struct DFPlayerMiniData *dfp, uint8_t command, uint16_t argument);
void DFPLayerMini_sendStack4(struct DFPlayerMiniData *dfp, uint8_t command, uint8_t argumentHigh, uint8_t argumentLow);
void DFPLayerMini_enableACK(struct DFPlayerMiniData *dfp);
void DFPLayerMini_disableACK(struct DFPlayerMiniData *dfp);

#endif /* _DFPLAYERMINI_H_ */
