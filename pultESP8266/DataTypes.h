#ifndef ftypes_h
#define ftypes_h
 

#include "Arduino.h"



typedef struct STime {
  uint8_t HH;
  uint8_t MM;
  uint8_t SS;
} TTime;

typedef struct SDate {
  uint8_t day;
  uint8_t month;
  uint16_t year;
} TDate;

typedef enum EBType {
  BY_INTERVAL,
  BY_CYCLIC_INTERVAL,
  BY_SENSOR
} TBType;



typedef struct SInterval {
  TBType type; 
  TTime times[4];
  uint8_t channel;
} TInterval;


typedef struct deviceConfig
{
  uint8_t initialized;
  uint8_t device_name[32*2];
  uint8_t device_login[16*2];
  uint8_t device_pass[16*2];
  uint8_t channels_name[4][40];
  uint8_t elements_count;
  uint16_t wifi_on_interval;
  TInterval intervals[80];
} TDeviceConfiguration;


typedef enum { EPR_OK, EPR_ERROR} TParseResult;


void stcpy(uint8_t *s1,const uint8_t *s2, uint8_t maxlen);



TParseResult VerifyIntervals();

TParseResult ParseChannelProgram(String *program, uint8_t channel);

TParseResult ParseLine(String *line, uint8_t channel, TInterval *element);


TParseResult ParseTime(String *str, TDate *d, TTime *t);

int GetLineEndIndex(String *str, int startAt);

void RemoveFirstEndsSpacesCRLF(String *str);


String GenerateStringFromIntervalsOfChannel(uint8_t channel);

String GetStringFromDateTime();
void SetTime(TTime *time) ;
void GetTime(TTime *time);

void SetDate(TDate *date);

void GetDate(TDate *date);

TParseResult SetDateTimeFromString(String *str);


void ControlChannel(uint8_t channel, uint8_t state);
void print1(uint8_t *s, uint8_t maxlen);


void configEEPROMread();
void configEEPROMwrite();
void configEEPROMinit();



#endif
