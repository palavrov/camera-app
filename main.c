#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "logerr.h"
#include "omx_still.h"

uint8_t jpeg1[10000000];
uint8_t jpeg2[10000000];

size_t position1 = 0;
size_t position2 = 0;

void buffering1(const uint8_t * const buffer, size_t length)
{
    LOG_MESSAGE("Buffer 1 Received %d at position %d", length, position1);

    if(length > sizeof(jpeg1)-position1)
    {
        LOG_ERROR("Buffer1 overflow %d %d", position1, length);
        length = sizeof(jpeg1)-position1;
    }

    memcpy(&jpeg1[position1], buffer, length);

    position1 += length;
}

void buffering2(const uint8_t * const buffer, size_t length)
{
    LOG_MESSAGE("Buffer 2 Received %d at position %d", length, position2);

    if(length > sizeof(jpeg2)-position2)
    {
        LOG_ERROR("Buffer2 overflow %d %d", position2, length);
        length = sizeof(jpeg2)-position2;
    }

    memcpy(&jpeg2[position2], buffer, length);

    position2 += length;
}

void buffering(const uint32_t frame, const uint8_t * const buffer, size_t length)
{
    switch(frame)
    {
        case 0: buffering1(buffer, length); break;
        case 1: buffering2(buffer, length); break;
        default: LOG_ERROR("Unexpected frame %d", frame); break;
    }
}

WARN_UNUSED enum error_code write_file(const char * const filename, const uint8_t * const buffer, const size_t length)
{
    //Open the file
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0666);
    if(fd == -1)
    {
        LOG_ERRNO("open %s", filename);
        return ERROR;
    }

    //Append the buffer into the file
    if(write(fd, buffer, length) == -1)
    {
        LOG_ERRNO("write %s %zu", filename, length);
        return ERROR;
    }

    //Close the file
    if(close(fd))
    {
        LOG_ERRNO("close %s", filename);
        return ERROR;
    }

    return OK;
}

int main()
{
    enum error_code result;

    position1 = 0;
    position2 = 0;

    result = omx_still_open();              if(result!=OK) { return result; }
    result = omx_still_shoot(2, buffering); if(result!=OK) { return result; }
    result = omx_still_close();             if(result!=OK) { return result; }

    result = write_file("/tmp/1.jpg", jpeg1, position1); if(result!=OK) { return result; }
    result = write_file("/tmp/2.jpg", jpeg2, position2); if(result!=OK) { return result; }

    return OK;
}
