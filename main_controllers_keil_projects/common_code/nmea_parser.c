#include "nmea_parser.h"

// variables
volatile MESSAGE_t msg;

// local functions
void nmea_msg_deinit(void);

void nmea_msg_deinit(void) {
	static uint32_t i = 0;
	msg.src = SRC_NONE;
	msg.cmd = CMD_NONE;
	msg.data.arg1 = 0;
	msg.data.arg2 = 0;
	msg.data.cyclic_num = 0;
	msg.data.hash = 0;
	for (i = 0; i < PAYLOAD_BASE64_STRING_SIZE; i++)
		msg.data.str[i] = 0;
}

void nmea_parse_msg(uint8_t *str) {
	static uint32_t i;
	// 1. reset character index
	i = 0;
	// deinit msg
	nmea_msg_deinit();

	// 2. search first start symbol
	while (str[i++] != NMEA_START_SYMBOL);
	//printf("pos = %d\t", i); // FIXME: debug

	// 3. determine source
	msg.src = IS_ESP8266(str[i], str[i + 1]) ? SRC_ESP8266 :
			IS_INTERPRETATOR(str[i], str[i + 1]) ? SRC_INTERPRETER : SRC_CONTROLLER;
	i++;
	//printf("src = %d\t", msg.src);

	// 4. determine command
	if (str[++i] == NMEA_CMD_START_SYMBOL) {
		msg.cmd = (COMMAND_t)(DIGIT(str[i + 1]) + DIGIT(str[i + 2]));
		i++; i++;
		//printf("cmd = %d\t", msg.cmd);// FIXME: debug
	}
	//printf("\n");

	// fill arguments

	uint32_t j = 0, n = 0;
	uint8_t buf[11] = { 0 }; // 4294967295 + '\0' == 11 symbols
	i++;

	switch (msg.cmd) {
	case CMD_NONE:
	default:
		// do nothing
		//printf("something wrong");
		break;
	case CMD_UART_SPEED:
	case CMD_PPM_SET:
	case CMD_POWER_SWITCH:
	case CMD_UART_TRASNSLATOR_START:
	case CMD_UART_TRANSLATOR_STOP:
		while (str[i++] != NMEA_STOP_SYMBOL) {
			if (str[i] == NMEA_SEPARATOR) {
				if (n == N_CYCLIC_NUM) {
					msg.data.cyclic_num = str2int(buf);
					//printf("cyclic = %d (n = %d)\t", msg.data.cyclic_num, n);
				} else if (n == N_ARG_1) {
					msg.data.arg1 = str2int(buf);
					//printf("arg1 = %d (n = %d)\t", msg.data.arg1, n);
				} else if (n == N_ARG_2) {
					msg.data.arg2 = str2int(buf);
					//printf("arg2 = %d (n = %d)\t", msg.data.arg2, n);
				}
				for (uint32_t x = 0; x < j; x++)
					buf[x] = NMEA_END_SYMBOL;
				j = 0;
				n++;
				continue;
			}
			buf[j++] = str[i];
		}
		if (str[i-2] != NMEA_SEPARATOR) {
			msg.data.hash = str2hash(buf[0], buf[1]);
		}
		//printf("hash = %d\n", msg.data.hash);
		break;
	case CMD_UART_RX:
	case CMD_UART_TX:
		while (str[i++] != NMEA_STOP_SYMBOL) {
			if (str[i] == NMEA_SEPARATOR) {
				if (n == N_CYCLIC_NUM) {
					msg.data.cyclic_num = str2int(buf);
					for (uint32_t x = 0; x < j; x++)
						buf[x] = NMEA_END_SYMBOL;
					//printf("cyclic = %d (n = %d)\t", msg.data.cyclic_num, n);
				} else if (n == N_ARG_1) {
					msg.data.arg1 = str2int(buf);
					for (uint32_t x = 0; x < j; x++)
						buf[x] = NMEA_END_SYMBOL;
					//printf("arg1 = %d (n = %d)\t", msg.data.arg1, n);
				} else if (n == N_UART_HASH) {
					if (str[i - 2] != ',') {
						msg.data.hash = str2hash(buf[0], buf[1]);
					}
				}
				j = 0;
				n++;
				continue;
			}
			if ((n == N_CYCLIC_NUM) || (n == N_ARG_1))
				buf[j++] = str[i];
			else if (n == N_UART_STRING) {
				msg.data.str[j++] = str[i];
			} else if (n == N_UART_HASH) {
				buf[j++] = str[i];
			}
		}

		if (str[i - 2] != NMEA_SEPARATOR)
			msg.data.hash = str2hash(buf[0], buf[1]);

		//printf("string = ");
		//for (uint32_t x = 0; x < PAYLOAD_BASE64_STRING_SIZE; x++) {
		//	printf("%c", msg.data.str[x]);
		//}
		//printf("\thash = %d\n", msg.data.hash);
		break;
	case CMD_BATTERY:
		while (str[i++] != NMEA_STOP_SYMBOL) {
			if (str[i] == NMEA_SEPARATOR) {
				if (n == N_CYCLIC_NUM) {
					msg.data.cyclic_num = str2int(buf);
					for (uint32_t x = 0; x < j; x++)
						buf[x] = NMEA_END_SYMBOL;
					//printf("cyclic = %d (n = %d)\t", msg.data.cyclic_num, n);
				} else if (n == N_UART_HASH) {
					if (str[i - 2] != NMEA_SEPARATOR) {
						msg.data.hash = str2hash(buf[0], buf[1]);
					}
				}
				j = 0;
				n++;
				continue;
			} else if (n == N_BAT) {
				msg.data.str[j++] = str[i];
			} else if (n == N_BAT_HASH) {
				buf[j++] = str[i];
			}
		}

		if (str[i - 2] != NMEA_SEPARATOR)
			msg.data.hash = str2hash(buf[0], buf[1]);

		//printf("string = ");
		//for (uint32_t x = 0; x < PAYLOAD_BASE64_STRING_SIZE; x++) {
		//	printf("%c", msg.data.str[x]);
		//}
		//printf("\thash = %d\n", msg.data.hash);
		break;
	}
}

// UTILITIES functions
/**
 * convert char array into integer value
 */
uint32_t str2int(uint8_t *str) {
	static uint32_t i, value;
	if (str[0] == NMEA_END_SYMBOL)
		return 0;
	i = 0;
	value = DIGIT(str[i]);
	while (str[++i] != NMEA_END_SYMBOL) {
		value *= 10;
		value += DIGIT(str[i]);
	}
	return value;
}

/**
 * convert integer value into char array
 * TODO: rewrite, exclude <math.h>
 */
void int2str(uint32_t value, uint8_t *str) {
	static int32_t i, n;
	n = (uint32_t)log10(value) + 1;
    for (i = n - 1; i >= 0; --i, value /= 10) {
        str[i] = (value % 10) + ASCII_ZERO_SHIFT; // + 48
    }
    str[n] = NMEA_END_SYMBOL; // '\0'
}

/**
 * calc string hash
 */
uint8_t hash(uint8_t *str, uint32_t begin, uint32_t end) {
	static uint32_t i;
	static uint8_t result;
	result = str[begin];
	for (i = begin + 1; i <= end; i++)
		result ^= str[i];
	return result;
}

/*
 * \param a in {'0' ... '9', 'A' ...  'F'}
 * \param b in {'0' ... '9', 'A' ...  'F'}
 * \return 0x00 ... 0xFF
 * */
uint8_t str2hash(uint8_t a, uint8_t b) {
	a = ((IS_DIGIT(a)) ? (a - ASCII_ZERO_SHIFT) : (a - ASCII_A_CHAR_SHIFT + 10)) << 4;
	b = ((IS_DIGIT(b)) ? (b - ASCII_ZERO_SHIFT) : (b - ASCII_A_CHAR_SHIFT + 10)) << 0;
	return (a | b);
}
