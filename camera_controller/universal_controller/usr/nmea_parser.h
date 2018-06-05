#ifndef __NMEA_PARSER_H__
#define __NMEA_PARSER_H__

// includes
#include "stdint.h"
//#include "stdio.h"
#include "math.h"

// interface here

#define PAYLOAD_ARGUMENTS_NUMBER      5
#define PAYLOAD_BASE64_STRING_SIZE    128

#define NMEA_START_SYMBOL             '$'
#define NMEA_STOP_SYMBOL              '\n'
#define NMEA_END_SYMBOL               '\0'
#define NMEA_CMD_START_SYMBOL         'E'
#define NMEA_SEPARATOR                ','

#define ASCII_ZERO_SHIFT              '0'
#define ASCII_A_CHAR_SHIFT            'A'

#define N_CYCLIC_NUM                  0
#define N_ARG_1                       1
#define N_ARG_2                       2
#define N_BAT                         1
#define N_UART_STRING                 2
#define N_BAT_HASH                    2
#define N_UART_HASH                   3

#define IS_ESP8266(x,y)               ((x=='P')&&(y=='E'))
#define IS_INTERPRETATOR(x, y)        ((x=='I')&&(y=='N'))
#define IS_CONTROLLER(x, y)           ((x=='C')&&(y=='1'))

#define IS_DIGIT(x)                   ( (x >= '0') & (x <= '9') )
#define DIGIT(x)                      (x - ASCII_ZERO_SHIFT)

typedef enum {
  SRC_NONE                            = 0,
  SRC_ESP8266                         = 1,
  SRC_INTERPRETER                     = 2,
  SRC_CONTROLLER                      = 3,
} SOURCE_t;

typedef enum {
  CMD_NONE                            = 0,
  CMD_UART_SPEED                      = 1,
  CMD_UART_TX                         = 2,
  CMD_PPM_SET                         = 3,
  CMD_UART_RX                         = 5,
  CMD_BATTERY                         = 6,
  CMD_POWER_SWITCH                    = 7,
  CMD_UART_TRASNSLATOR_START          = 8,
  CMD_UART_TRANSLATOR_STOP            = 9,
} COMMAND_t;

typedef struct {
  uint8_t str[PAYLOAD_BASE64_STRING_SIZE];
  uint8_t hash;
  uint8_t cyclic_num;
  uint32_t arg1;
  uint32_t arg2;
} PAYLOAD_t;

typedef struct {
  SOURCE_t src;
  COMMAND_t cmd;
  PAYLOAD_t data;
} MESSAGE_t;

// functions
void nmea_parse_msg(uint8_t *str);

/*
 * \return 8 bit XOR sum of the input string
 * */
uint8_t hash(uint8_t *str, uint32_t begin, uint32_t end);

// FIXME: local functions ??
uint32_t str2int(uint8_t *str);
uint8_t str2hash(uint8_t a, uint8_t b);
void int2str(uint32_t value, uint8_t *str);

#endif /* __NMEA_PARSER_H__ */
