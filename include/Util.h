/*
 * Original macros written by Zed Shaw.
 * Modifications by Aaron Helton (a.k.a. aargonian)
 * Email: aargonian@gmail.com
 */
#ifndef __NY_UTIL_H
#define __NY_UTIL_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    NY_BIG_ENDIAN,
    NY_LITTLE_ENDIAN
} NY_ENDIANESS;

bool read64(uint64_t *data, NY_ENDIANESS endian, FILE *file);
bool read32(uint32_t *data, NY_ENDIANESS endian, FILE *file);
bool read16(uint16_t *data, NY_ENDIANESS endian, FILE *file);

#ifdef DEBUG
    #define debug(...)                          \
            fprintf(stderr, "DEBUG %s:%d: ");   \
            fprintf(stderr, __VA_ARGS__);       \
            fprintf(stderr, "\n")
#else
    #define debug(...)
#endif/*DEBUG*/

#define errno_str() (errno == 0 ? "None" : strerror(errno))


#ifndef NO_LOG
    #define __ERR_TEXT  "[ERROR] (%s:%d: errno: %s) "
    #define __WARN_TEXT "[WARNING] (%s:%d: errno: %s) "
    #define __INFO_TEXT "[INFO] (%s:%d) "
    #define log_err(...)                                                   \
            fprintf(stderr, __ERR_TEXT, __FILE__, __LINE__, errno_str());  \
            fprintf(stderr, __VA_ARGS__);                                  \
            fprintf(stderr, "\n")

    #define log_warn(...)                                                  \
            fprintf(stderr, __WARN_TEXT, __FILE__, __LINE__, errno_str()); \
            fprintf(stderr, __VA_ARGS__);                                  \
            fprintf(stderr, "\n")

    #define log_info(...)                                                  \
            fprintf(stderr, __INFO_TEXT, __FILE__, __LINE__);              \
            fprintf(stderr, __VA_ARGS__);                                  \
            fprintf(stderr, "\n")
#else /* Turn off logging, which is on by default even without
       * debug. This may be useful if you are writing a library. */
    #define log_err(...)
    #define log_warn(...)
    #define log_info(...)
#endif/*NO_LOG*/


#define check(A, ...)                   \
        do {                            \
            if(!(A)) {                  \
                log_err(__VA_ARGS__);   \
                errno=0;                \
                goto error;             \
            }                           \
        } while(0)

#define sentinel(...)                   \
        do {                            \
            log_err(__VA_ARGS__);       \
            errno = 0;                  \
            goto error;                 \
        } while(0)

#define check_debug(A, ...)             \
        do {                            \
            if(!A)) {                   \
                debug(__VA_ARGS__);     \
                errno = 0;              \
                goto error;             \
            }                           \
        } while(0)

#endif/*__NY_UTIL_H*/
