#include "base64.h"

#include "string.h"
#include "ctype.h"

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline uint8_t is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

unsigned int base64_encode(const unsigned char* bytes_to_encode, unsigned int in_len, unsigned char* encoded_buffer, unsigned int out_len_)
{
		unsigned int out_len = out_len_;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3] = { 0, 0, 0 };
    unsigned char char_array_4[4] = { 0, 0, 0, 0 };

    out_len = 0;
    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4 ; i++)
            {
                encoded_buffer[out_len++] = base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
        {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < (i + 1); j++)
        {
            encoded_buffer[out_len++] = base64_chars[char_array_4[j]];
        }

        while (i++ < 3)
        {
            encoded_buffer[out_len++] = '=';
        }
    }

    return out_len;
}


uint8_t findch(char* str, char ch) {
	uint8_t i = 0;
	while (str[i] != ch && i < 255) {
		i++;
	}
	return i;
}

unsigned int base64_decode(const unsigned char* encoded_string, unsigned int in_len, unsigned char* decoded_buffer, unsigned int out_len_)
{
		unsigned int out_len = out_len_;
    size_t i = 0;
    size_t j = 0;
    int in_ = 0;
    unsigned char char_array_3[3] = { 0, 0, 0 };
    unsigned char char_array_4[4] = { 0, 0, 0, 0 };

    out_len = 0;
    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
            {
                char_array_4[i] = findch((char*)base64_chars, char_array_4[i]);// (base64_chars.find(char_array_4[i]));
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++)
            {
                decoded_buffer[out_len++] = char_array_3[i];
            }
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
        {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++)
        {
            char_array_4[j] = findch((char*)base64_chars, char_array_4[j]);//(unsigned char)(base64_chars.find(char_array_4[j]));
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
        {
            decoded_buffer[out_len++] = char_array_3[j];
        }
    }
    return out_len;
}