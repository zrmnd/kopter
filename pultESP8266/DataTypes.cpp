#include "DataTypes.h"

extern volatile TDeviceConfiguration deviceConfiguration;

TParseResult VerifyIntervals() {
  return EPR_OK;
}


TParseResult ParseChannelProgram(String *program, uint8_t channel) {
  Serial.printf("Program %d:\n", channel);
  //Serial.println(*program);
  Serial.println("Lines:");
  uint16_t nextStrIndex = 0, beginStrIndex = 0, nextFreeIntervalsIndex = deviceConfiguration.elements_count; 
  String tmp = "";

  while(1) {
    nextStrIndex = GetLineEndIndex(program, beginStrIndex);
    //Serial.printf("nextStrIndex= %d, beginStrIndex=%d\n", nextStrIndex, beginStrIndex);
    if (nextStrIndex == beginStrIndex)
      break;
    tmp = program->substring(beginStrIndex, nextStrIndex+1); 
    beginStrIndex = nextStrIndex+1;
    RemoveFirstEndsSpacesCRLF(&tmp);
    if (tmp.length() > 0) { // в строке есть что-то кроме пробелов и переноса строки - пробуем парсить
      if (ParseLine(&tmp, channel, (TInterval*)&deviceConfiguration.intervals[deviceConfiguration.elements_count]) == EPR_OK)
        deviceConfiguration.elements_count++;
      else {
        configEEPROMread();
        return EPR_ERROR;
      }
    }
 
      
  }
  return EPR_OK;
}


TParseResult ParseLine(String *line, uint8_t channel, TInterval *element) {
  //Serial.println("Parse line call");
  Serial.println(line->c_str());
  uint16_t len = line->length(), i;
  const char *cstr = line->c_str();

  i = line->indexOf(' ');
  String cmd = line->substring(0, i);

  if (cmd == "INTERVAL") {

  }
  else 
  if (cmd == "ON") {

  }
  else {

  }


 
  










  
  return EPR_OK;
}



TParseResult ParseTime(String *str, TDate *d, TTime *t) {
  return EPR_OK;
}


int GetLineEndIndex(String *str, int startAt) {  //от начального индекса ищет символы переноса строки и возвращает номер последнего символа переноса строки ('\r' или '\n'). Если достигли конца str, возврат последнего индекса строки 
  uint16_t len = str->length(), i;
  const char *cstr = str->c_str();
  //Serial.printf("len= %d\n", len);
  if (startAt > len-1)
    return startAt;
  uint8_t flagFoundCRLF = 0;
  for (i = startAt; i < len; i++) {
    if (flagFoundCRLF) {  // конец строки уже обнаружен, ищем первый символ следующей строки
      //Serial.printf("flag=1, i= %d\n", i);
      if (cstr[i] != '\n' && cstr[i] != '\r') {
        if (i > 0) {
          //Serial.println("return i-1");
          return i-1;
        }
        else {
          //Serial.println("return 0");
          return 0; 
        }
      }     
    }
    else {
      //Serial.printf("flag=0, i= %d\n", i);
      if (cstr[i] == '\n' || cstr[i] == '\r') 
        flagFoundCRLF = 1;
    }
    
  }
  //Serial.println("return len-1");
  return len-1;
}


void RemoveFirstEndsSpacesCRLF(String *str) {
  uint16_t len = str->length(), i1, i2;
  const char *cstr = str->c_str();

  i1 = 0;
  while (cstr[i1] == ' ' && i1 < len) {
    i1++;
  }

  i2 = len - 1;
  while ((cstr[i2] == '\r' || cstr[i2] == '\n') && i2 > 0) {
    i2--;
  }

  *str = str->substring(i1, i2+1);
}






String GenerateStringFromIntervalsOfChannel(uint8_t channel) {
  return String("");
}

String GetStringFromDateTime() {
  TTime t; 
  TDate d;
  GetTime(&t);
  GetDate(&d);

  char nextupdatetimec[20];
  snprintf(nextupdatetimec, sizeof(nextupdatetimec),"%02d.%02d.%04d %02d:%02d:%02d", d.day, d.month, d.year, t.HH, t.MM, t.SS);
  String result(nextupdatetimec);
  return result;
}
void SetTime(TTime *t) {
  
}

void GetTime(TTime *t) {
  t->HH = 12;
  t->MM = 15;
  t->SS = 0;
}

void SetDate(TDate *date) {
  
}

void GetDate(TDate *date) {
  date->day = 5;
  date->month = 1;
  date->year = 2018;
}

TParseResult SetDateTimeFromString(String *str) { // "14.01.2018 18:43:56"
  return EPR_OK;
}


void ControlChannel(uint8_t channel, uint8_t state) { //0 - off, 1 - on
  
}


void stcpy(uint8_t *s1,const uint8_t *s2, uint8_t maxlen) {
uint8_t i = 0;
while (s2[i]!='\r' && s2[i] !='\n' && s2[i]!=0 && i < maxlen) {
  s1[i] = s2[i];
  i++;
}
if (i < maxlen)
  s1[i] = 0;
}

void print1(uint8_t *s, uint8_t maxlen) {
uint8_t i = 0;
while (s[i]!='\r' && s[i] !='\n' && s[i]!=0 && i < maxlen) {
  //Serial.print((uint8_t)s[i], HEX);
char charVal[4];
   sprintf(charVal, "%02X", s[i]);
   Serial.print(charVal);
  i++;
}
Serial.println("");

}

