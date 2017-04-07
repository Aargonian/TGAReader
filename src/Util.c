#include <stdint.h>
#include <stdbool.h>

#include <Util.h>

bool read64(uint64_t *data, NY_ENDIANESS endian, FILE *file)
{
    uint8_t buffer[8];
    uint64_t value = 0;
    check(file, "Invalid File Pointer Passed.");
    check(data, "Invalid data pointer passed.");
    if(endian == NY_LITTLE_ENDIAN)
    {
        check(fread(buffer, sizeof(buffer), 1, file) == 1, 
                "Unable to read file.");
        value += buffer[0];
        value += ((uint64_t)buffer[1]) << 8;
        value += ((uint64_t)buffer[2]) << 16;
        value += ((uint64_t)buffer[3]) << 24;
        value += ((uint64_t)buffer[4]) << 32;
        value += ((uint64_t)buffer[5]) << 40;
        value += ((uint64_t)buffer[6]) << 48;
        value += ((uint64_t)buffer[7]) << 56;
        *data = value;
        return true;
    }
    else
    {
        check(fread(buffer, sizeof(buffer), 1, file) == 1, 
                "Unable to read file.");
        value += buffer[7];
        value += ((uint64_t)buffer[6]) << 8;
        value += ((uint64_t)buffer[5]) << 16;
        value += ((uint64_t)buffer[4]) << 24;
        value += ((uint64_t)buffer[3]) << 32;
        value += ((uint64_t)buffer[2]) << 40;
        value += ((uint64_t)buffer[1]) << 48;
        value += ((uint64_t)buffer[0]) << 56;
        *data = value;
        return true;
    }

error:
    return false;
}

bool read32(uint32_t *data, NY_ENDIANESS endian, FILE *file)
{
    uint8_t buffer[4];
    uint32_t value = 0;
    check(file, "Invalid File Pointer Passed.");
    check(data, "Invalid data pointer passed.");
    if(endian == NY_LITTLE_ENDIAN)
    {
        check(fread(buffer, sizeof(buffer), 1, file) == 1, 
                "Unable to read file.");
        value += buffer[0];
        value += ((uint32_t)buffer[1]) << 8;
        value += ((uint32_t)buffer[2]) << 16;
        value += ((uint32_t)buffer[3]) << 24;
        *data = value;
        return true;
    }
    else
    {
        check(fread(buffer, sizeof(buffer), 1, file) == 1, 
                "Unable to read file.");
        value += buffer[3];
        value += ((uint32_t)buffer[2]) << 8;
        value += ((uint32_t)buffer[1]) << 16;
        value += ((uint32_t)buffer[0]) << 24;
        *data = value;
        return true;
    }

error:
    return false;
}

bool read16(uint16_t *data, NY_ENDIANESS endian, FILE *file)
{
    uint8_t buffer[2];
    uint16_t value = 0;
    check(file, "Invalid File Pointer Passed.");
    check(data, "Invalid data pointer passed.");
    if(endian == NY_LITTLE_ENDIAN)
    {
        check(fread(buffer, sizeof(buffer), 1, file) == 1, 
                "Unable to read file.");
        value += buffer[0];
        value += ((uint16_t)buffer[1]) << 8;
        *data = value;
        return true;
    }
    else
    {
        check(fread(buffer, sizeof(buffer), 1, file) == 1, 
                "Unable to read file.");
        value += buffer[0];
        value += ((uint16_t)buffer[1]) << 8;
        *data = value;
        return true;
    }

error:
    return false;
}
