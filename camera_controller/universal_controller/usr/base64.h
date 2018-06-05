#ifndef __BASE64_H__
#define __BASE64_H__

// FIXME: ?? includes
#include <stdint.h>

/**
 * \return 
 */
uint32_t base64_encode(const uint8_t* bytes_to_encode, uint32_t in_len, uint8_t* encoded_buffer, uint32_t out_len_);
uint32_t base64_decode(const uint8_t* encoded_string, uint32_t in_len, uint8_t* decoded_buffer, uint32_t out_len_);

#endif /* __BASE64_H__ */
