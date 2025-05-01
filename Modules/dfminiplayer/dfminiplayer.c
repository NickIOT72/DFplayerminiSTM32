#include "dfminiplayer.h"
#include "Serial.h"

uint16_t calculateCheckSum(uint8_t *buffer);
void uint16ToArray(uint16_t value, uint8_t *array);
uint16_t arrayToUint16(uint8_t *array);

void uint16ToArray(uint16_t value, uint8_t *array){
  *array = (uint8_t)(value>>8);
  *(array+1) = (uint8_t)(value);
}

uint16_t calculateCheckSum(uint8_t *buffer){
  uint16_t sum = 0;
  for (int i=Stack_Version; i<Stack_CheckSum; i++) {
    sum += buffer[i];
  }
  return -sum;
}

uint16_t arrayToUint16(uint8_t *array){
  uint16_t value = *array;
  value <<=8;
  value += *(array+1);
  return value;
}

void DFPLayerMini_setTimeOut(struct DFPlayerMiniData *dfp, unsigned long timeOutDuration){
  dfp->_timeOutDuration = timeOutDuration;
}

void DFPLayerMini_sendStack(struct DFPlayerMiniData *dfp)
{
  
#ifdef _DEBUG
  Serial_print(&dfp->debug_uart ,"%s", "\nSending data: ");
  for( int i = 0; i < 10; i++ ){
  		Serial_print(&dfp->debug_uart,"%1X ",dfp->_sending[i] );
  }
  Serial_print(&dfp->debug_uart ,"%s", "\n");
#endif
  Serial_write(&dfp->df_uart ,dfp->_sending, DFPLAYER_SEND_LENGTH);
  dfp->_timeOutTimer = HAL_GetTick();
  dfp->_isSending = dfp->_sending[Stack_ACK];

  uint8_t counter_send = 0;
  if (dfp->_sending[Stack_ACK]) {  //if the ack mode is on wait until the last transmition
    while (dfp->_isSending && counter_send < 1) {
      HAL_Delay(5);
      DFPLayerMini_waitAvailable(dfp,0);
      counter_send += 1;
    }
  }

  if (!dfp->_sending[Stack_ACK]) { //if the ack mode is off wait 10 ms after one transmition.
    HAL_Delay(10);
  }

}

void DFPLayerMini_sendStack2(struct DFPlayerMiniData *dfp, uint8_t command){
	DFPLayerMini_sendStack3(dfp, command, 0);
}

void DFPLayerMini_sendStack3(struct DFPlayerMiniData *dfp, uint8_t command, uint16_t argument){
  dfp->_sending[Stack_Command] = command;
  uint16ToArray(argument, dfp->_sending+Stack_Parameter);
  uint16ToArray(calculateCheckSum(dfp->_sending), dfp->_sending+Stack_CheckSum);
  DFPLayerMini_sendStack(dfp);
}

void DFPLayerMini_sendStack4(struct DFPlayerMiniData *dfp, uint8_t command, uint8_t argumentHigh, uint8_t argumentLow){
  uint16_t buffer = argumentHigh;
  buffer <<= 8;
  DFPLayerMini_sendStack3(dfp, command, buffer | argumentLow);
}

uint16_t DFPLayerMini_read(struct DFPlayerMiniData *dfp){
  dfp->_isAvailable = false;
  return dfp->_handleParameter;
}

uint8_t DFPLayerMini_readType(struct DFPlayerMiniData *dfp){
  dfp->_isAvailable = false;
  return dfp->_handleType;
}

int DFPLayerMini_readVolume(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x43);
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    return DFPLayerMini_read(dfp);
  }
  else{
    return -1;
  }
}

int DFPLayerMini_readEQ(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x44);
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    if (DFPLayerMini_readType(dfp) == DFPlayerFeedBack) {
      return DFPLayerMini_read(dfp);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int DFPLayerMini_readFileCounts2(struct DFPlayerMiniData *dfp, uint8_t device){
  switch (device) {
    case DFPLAYER_DEVICE_U_DISK:
    	DFPLayerMini_sendStack2(dfp,0x47);
      break;
    case DFPLAYER_DEVICE_SD:
      DFPLayerMini_sendStack2(dfp,0x48);
      break;
    case DFPLAYER_DEVICE_FLASH:
    	DFPLayerMini_sendStack2(dfp,0x49);
      break;
    default:
      break;
  }
  
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    if (DFPLayerMini_readType(dfp) == DFPlayerFeedBack) {
      return DFPLayerMini_read(dfp);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int DFPLayerMini_readCurrentFileNumber2(struct DFPlayerMiniData *dfp, uint8_t device){
  switch (device) {
    case DFPLAYER_DEVICE_U_DISK:
    	DFPLayerMini_sendStack2(dfp, 0x4B);
      break;
    case DFPLAYER_DEVICE_SD:
    	DFPLayerMini_sendStack2(dfp,0x4C);
      break;
    case DFPLAYER_DEVICE_FLASH:
    	DFPLayerMini_sendStack2(dfp,0x4D);
      break;
    default:
      break;
  }
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    if (DFPLayerMini_readType(dfp) == DFPlayerFeedBack) {
      return DFPLayerMini_read(dfp);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int DFPLayerMini_readFileCountsInFolder(struct DFPlayerMiniData *dfp, int folderNumber){
	DFPLayerMini_sendStack3(dfp,0x4E, folderNumber);
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    if (DFPLayerMini_readType(dfp) == DFPlayerFeedBack) {
      return DFPLayerMini_read(dfp);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int DFPLayerMini_readFolderCounts(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x4F);
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    if (DFPLayerMini_readType(dfp) == DFPlayerFeedBack) {
      return DFPLayerMini_read(dfp);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

int DFPLayerMini_readFileCounts(struct DFPlayerMiniData *dfp){
  return DFPLayerMini_readFileCounts2(dfp,DFPLAYER_DEVICE_SD);
}

int DFPLayerMini_readCurrentFileNumber(struct DFPlayerMiniData *dfp){
  return DFPLayerMini_readCurrentFileNumber2(dfp, DFPLAYER_DEVICE_SD);
}

int DFPLayerMini_readState(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp , 0x42);
  if (DFPLayerMini_waitAvailable(dfp,0)) {
    if (DFPLayerMini_readType(dfp) == DFPlayerFeedBack) {
      return DFPLayerMini_read(dfp);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

void DFPLayerMini_loop(struct DFPlayerMiniData *dfp,int fileNumber) {
	DFPLayerMini_sendStack3(dfp, 0x08, fileNumber);
}

void DFPLayerMini_outputDevice(struct DFPlayerMiniData *dfp,uint8_t device) {
	DFPLayerMini_sendStack3(dfp,0x09, device);
  HAL_Delay(200);
}

void DFPLayerMini_sleep(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x0A);
}

void DFPLayerMini_start(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x0D);
}

void DFPLayerMini_pause(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x0E);
}

void DFPLayerMini_playFolder(struct DFPlayerMiniData *dfp,uint8_t folderNumber, uint8_t fileNumber){
	DFPLayerMini_sendStack4(dfp,0x0F, folderNumber, fileNumber);
}

void DFPLayerMini_outputSetting(struct DFPlayerMiniData *dfp,bool enable, uint8_t gain){
	DFPLayerMini_sendStack4(dfp,0x10, enable, gain);
}

void DFPLayerMini_enableLoopAll(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack3(dfp,0x11, 0x01);
}

void DFPLayerMini_disableLoopAll(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack3(dfp,0x11, 0x00);
}

void DFPLayerMini_playMp3Folder(struct DFPlayerMiniData *dfp,int fileNumber){
	DFPLayerMini_sendStack3(dfp,0x12, fileNumber);
}

void DFPLayerMini_advertise(struct DFPlayerMiniData *dfp,int fileNumber){
	DFPLayerMini_sendStack3(dfp,0x13, fileNumber);
}

void DFPLayerMini_playLargeFolder(struct DFPlayerMiniData *dfp,uint8_t folderNumber, uint16_t fileNumber){
	DFPLayerMini_sendStack3(dfp,0x14, (((uint16_t)folderNumber) << 12) | fileNumber);
}

void DFPLayerMini_stopAdvertise(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x15);
}

void DFPLayerMini_stop(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x16);
}

void DFPLayerMini_loopFolder(struct DFPlayerMiniData *dfp,int folderNumber){
	DFPLayerMini_sendStack3(dfp,0x17, folderNumber);
}

void DFPLayerMini_randomAll(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp,0x18);
}

void DFPLayerMini_enableLoop(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack3(dfp,0x19, 0x00);
}

void DFPLayerMini_disableLoop(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack3(dfp,0x19, 0x01);
}

void DFPLayerMini_enableDAC(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack3(dfp,0x1A, 0x00);
}

void DFPLayerMini_disableDAC(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack3(dfp,0x1A, 0x01);
}

void DFPLayerMini_EQ(struct DFPlayerMiniData *dfp , uint8_t eq) {
	DFPLayerMini_sendStack3(dfp , 0x07, eq);
}

void DFPLayerMini_next(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp, 0x01);
}

void DFPLayerMini_previous(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp, 0x02);
}

void DFPLayerMini_play(struct DFPlayerMiniData *dfp,int fileNumber){
	DFPLayerMini_sendStack3(dfp, 0x03, fileNumber);
}

void DFPLayerMini_volumeUp(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp, 0x04);
}

void DFPLayerMini_volumeDown(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2(dfp, 0x05);
}

void DFPLayerMini_volume(struct DFPlayerMiniData *dfp, uint8_t volume){
	DFPLayerMini_sendStack3(dfp, 0x06, volume);
}

bool DFPLayerMini_handleMessage(struct DFPlayerMiniData *dfp, uint8_t type, uint16_t parameter){
  dfp->_receivedIndex = 0;
  dfp->_handleType = type;
  dfp->_handleParameter = parameter;
  dfp->_isAvailable = true;
  return dfp->_isAvailable;
}

bool DFPLayerMini_handleError(struct DFPlayerMiniData *dfp, uint8_t type, uint16_t parameter){
	DFPLayerMini_handleMessage(dfp, type, parameter);
  dfp->_isSending = false;
  return false;
}

bool DFPLayerMini_validateStack(struct DFPlayerMiniData *dfp){
  return calculateCheckSum(dfp->_received) == arrayToUint16(dfp->_received+Stack_CheckSum);
}

bool DFPLayerMini_begin(struct DFPlayerMiniData *dfp, bool isACK , bool doReset)
{
  if (isACK) {
	  DFPLayerMini_enableACK(dfp);
  }
  else{
	  DFPLayerMini_disableACK(dfp);
  }
  if (doReset) {
	  DFPLayerMini_reset(dfp);
	  DFPLayerMini_waitAvailable(dfp , 2000);
    HAL_Delay(200);
  }
  else {
    // assume same state as with reset(): online
    dfp->_handleType = DFPlayerCardOnline;
  }
  return true;
}

void DFPLayerMini_parseStack(struct DFPlayerMiniData *dfp){
  uint8_t handleCommand = *(dfp->_received + Stack_Command);
  if (handleCommand == 0x41) { //handle the 0x41 ack feedback as a spcecial case, in case the pollusion of _handleCommand,dfp->_handleParameter, and _handleType.
    dfp->_isSending = false;
    return;
  }
  
  dfp->_handleCommand = handleCommand;
  dfp->_handleParameter =  arrayToUint16(dfp->_received + Stack_Parameter);

  switch (dfp->_handleCommand) {
    case 0x3C:
    case 0x3D:
    	DFPLayerMini_handleMessage(dfp,DFPlayerPlayFinished,dfp->_handleParameter);
      break;
    case 0x3F:
      if (dfp->_handleParameter & 0x01) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerUSBOnline,dfp->_handleParameter);
      }
      else if (dfp->_handleParameter & 0x02) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerCardOnline,dfp->_handleParameter);
      }
      else if (dfp->_handleParameter & 0x03) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerCardUSBOnline,dfp->_handleParameter);
      }
      break;
    case 0x3A:
      if (dfp->_handleParameter & 0x01) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerUSBInserted,dfp->_handleParameter);
      }
      else if (dfp->_handleParameter & 0x02) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerCardInserted,dfp->_handleParameter);
      }
      break;
    case 0x3B:
      if (dfp->_handleParameter & 0x01) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerUSBRemoved,dfp->_handleParameter);
      }
      else if (dfp->_handleParameter & 0x02) {
    	  DFPLayerMini_handleMessage(dfp,DFPlayerCardRemoved,dfp->_handleParameter);
      }
      break;
    case 0x40:
    	DFPLayerMini_handleMessage(dfp,DFPlayerError,dfp->_handleParameter);
      break;
    case 0x3E:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
    	DFPLayerMini_handleMessage(dfp,DFPlayerFeedBack,dfp->_handleParameter);
      break;
    default:
    	DFPLayerMini_handleError(dfp,WrongStack, 0);
      break;
  }
}


bool DFPLayerMini_available(struct DFPlayerMiniData *dfp){
  while (Serial_available()) {
    HAL_Delay(0);
    if (dfp->_receivedIndex == 0) {
    	dfp->_received[Stack_Header] = Serial_read();
#ifdef _DEBUG
    	Serial_print(&dfp->debug_uart ,"%s", "received:");
    	Serial_print(&dfp->debug_uart,"%1X",dfp->_received[dfp->_receivedIndex] );
    	Serial_print(&dfp->debug_uart ,"%s", " ");
#endif
      if (dfp->_received[Stack_Header] == 0x7E) {
    	  dfp->_receivedIndex += 1;
      }
    }
    else{
    	dfp->_received[dfp->_receivedIndex] = Serial_read();
#ifdef _DEBUG
    	Serial_print(&dfp->debug_uart,"%1X",dfp->_received[dfp->_receivedIndex] );
    	Serial_print(&dfp->debug_uart ,"%s", " ");
#endif
      switch (dfp->_receivedIndex) {
        case Stack_Version:
          if (dfp->_received[dfp->_receivedIndex] != 0xFF) {
            return DFPLayerMini_handleError(dfp,WrongStack,0);
          }
          break;
        case Stack_Length:
          if (dfp->_received[dfp->_receivedIndex] != 0x06) {
            return DFPLayerMini_handleError(dfp,WrongStack,0);
          }
          break;
        case Stack_End:
#ifdef _DEBUG
        	Serial_print(&dfp->debug_uart ,"%s", "\n");
#endif
          if (dfp->_received[dfp->_receivedIndex] != 0xEF) {
            return DFPLayerMini_handleError(dfp,WrongStack,0);
          }
          else{
            if (DFPLayerMini_validateStack(dfp)) {
            	dfp->_receivedIndex = 0;
            	DFPLayerMini_parseStack(dfp);
              return dfp->_isAvailable;
            }
            else{
              return DFPLayerMini_handleError(dfp,WrongStack,0);
            }
          }
          break;
        default:
          break;
      }
      dfp->_receivedIndex++;
    }
  }
  
  
  return dfp->_isAvailable;
}

bool DFPLayerMini_waitAvailable(struct DFPlayerMiniData *dfp , unsigned long duration){
  unsigned long timer = HAL_GetTick();
  if (!duration) {
    duration = dfp->_timeOutDuration;
  }
  while (!DFPLayerMini_available(dfp)){
    if (HAL_GetTick() - timer > duration) {
      return DFPLayerMini_handleError(dfp,TimeOut,0);
    }
    HAL_Delay(0);
  }
  /*uint8_t counter_f = 0;
  while( counter_f < 1 )
  {
    while (!DFPLayerMini_available(dfp)){
      if (HAL_GetTick() - timer > duration) {
        return DFPLayerMini_handleError(dfp,TimeOut,0);
      }
      HAL_Delay(0);
    }
    HAL_Delay(5);
    counter_f += 1;
    timer = HAL_GetTick();
  }*/
  
  return true;
}

void DFPLayerMini_reset(struct DFPlayerMiniData *dfp){
	DFPLayerMini_sendStack2( dfp , 0x0C);
}

void DFPLayerMini_enableACK(struct DFPlayerMiniData *dfp)
{
  dfp->_sending[Stack_ACK] = 0x01;
}

void DFPLayerMini_disableACK(struct DFPlayerMiniData *dfp)
{
  dfp->_sending[Stack_ACK] = 0x00;
}
