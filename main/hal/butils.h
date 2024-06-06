///
/// @file butils.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-07-16
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define PACKED_FMT __attribute__((packed))

#define STRUCT_MEMBER_OFFSET(type, member) \
    ((uint32_t) & (((type *)NULL)->member))

#define min(x, y) \
    (((x) < (y)) ? (x) : (y))

#define max(x, y) \
    ((x) > (y) ? (x) : (y))

#define WaitForConditionOrTimeout(x, timeout, on_timeout) \
    {                                                     \
        int i = (timeout);                                \
        for (; (i > 0) && (!(x)); i--) {                  \
            CyDelay(1);                                   \
        }                                                 \
        if (i == 0) {                                     \
            on_timeout;                                   \
        }                                                 \
    }

#define _bswap16(n) ((uint16_t)((((n) & 0x00FF) << 8) | \
                                (((n) & 0xFF00) >> 8)))

#define _bswap32(n) ((uint32_t)((((n) & 0x000000FF) << 24) | \
                                (((n) & 0x0000FF00) << 8) |  \
                                (((n) & 0x00FF0000) >> 8) |  \
                                (((n) & 0xFF000000) >> 24)))
