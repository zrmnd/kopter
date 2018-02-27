#ifndef __BASE64_H__
#define __BASE64_H__

// FIXME: ?? includes
#include <stdint.h>

/**
 * \return 
 */
unsigned int base64_encode(const unsigned char* bytes_to_encode, unsigned int in_len, unsigned char* encoded_buffer, unsigned int out_len_);
unsigned int base64_decode(const unsigned char* encoded_string, unsigned int in_len, unsigned char* decoded_buffer, unsigned int out_len_);

#endif /* __BASE64_H__ */
