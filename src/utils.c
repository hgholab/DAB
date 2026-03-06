#include <stdint.h>

#include "../include/utils.h"

// static uint32_t str_to_uint32(const char *str);
// static int32_t str_to_int32(const char *str);

#include <ctype.h>
#include <stdint.h>

// Forward declarations for internal functions
static uint32_t str_to_uint32(const uint16_t *str);
static int32_t str_to_int32(const uint16_t *str);

/**
 * Converts a 16-bit "string" to a float.
 */
float str_to_float(const uint16_t *str)
{
        float result                = 0.0f;
        const uint16_t *p           = str;
        int16_t decimal_point_index = -1;
        int16_t i                   = 0;

        // Find the position of the decimal point
        while (str[i] != 0U)
        {
                if (str[i] == (uint16_t)'.')
                {
                        decimal_point_index = i;
                        break;
                }
                i++;
        }

        // No decimal point => straight integer conversion
        if (decimal_point_index == -1)
        {
                return (float)str_to_int32(p);
        }
        else
        {
                // Get the integer part
                int32_t integer_part = str_to_int32(p);

                // Fractional part starts after '.'
                const uint16_t *frac_str = p + decimal_point_index + 1;

                // Calculate fractional length
                int16_t frac_len = 0;
                while (frac_str[frac_len] != 0U)
                {
                        frac_len++;
                }

                uint32_t frac_digits = 0U;
                if (frac_len > 0)
                {
                        frac_digits = str_to_uint32(frac_str);
                }

                // Scale fractional digits by 10^(-frac_len)
                float scale      = 1.0f;
                int16_t temp_len = frac_len;
                while (temp_len > 0)
                {
                        scale *= 10.0f;
                        temp_len--;
                }

                float frac_part = (float)frac_digits / scale;

                // Combine parts carefully handling the sign
                if (integer_part < 0 || (*p == (uint16_t)'-'))
                {
                        result = (float)integer_part - frac_part;
                }
                else
                {
                        result = (float)integer_part + frac_part;
                }

                return result;
        }
}

/**
 * Converts 16-bit characters to lowercase.
 */
void str_to_lower(uint16_t *str)
{
        while (*str != 0U)
        {
                // Mask with 0xFF to ensure we only touch the bottom 8 bits if necessary,
                // though tolower usually handles this.
                *str = (uint16_t)tolower((int16_t)(*str & 0xFFU));
                str++;
        }
}

/**
 * Internal: String to Unsigned 32-bit Integer
 */
static uint32_t str_to_uint32(const uint16_t *str)
{
        const uint16_t *start = str;
        uint32_t result       = 0;
        uint32_t place        = 1U;

        // Move pointer to the end of the numeric part or decimal
        while (*str != 0U && *str != (uint16_t)'.')
        {
                str++;
        }

        // Move back to the last digit
        str--;

        // Process digits from right to left (ones, tens, hundreds...)
        while (str >= start)
        {
                // Only process if it's a digit (ASCII 48-57)
                if (*str >= (uint16_t)'0' && *str <= (uint16_t)'9')
                {
                        result += (uint32_t)(*str - (uint16_t)'0') * place;
                        place *= 10U;
                }
                str--;
        }

        return result;
}

/**
 * Internal: String to Signed 32-bit Integer
 */
static int32_t str_to_int32(const uint16_t *str)
{
        if (*str == (uint16_t)'-')
        {
                return -(int32_t)str_to_uint32(str + 1);
        }
        else if (*str == (uint16_t)'+')
        {
                return (int32_t)str_to_uint32(str + 1);
        }

        return (int32_t)str_to_uint32(str);
}
