///
/// @file bioexp.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-03-17
///
/// @copyright Copyright (c) 2024
///
///
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <utils/sll_listeners.h>
#include <utils/util_macro.h>

enum bioexp_ios {
    BIOEXP_IO_0,
    BIOEXP_IO_1,
    BIOEXP_BTN_0,
    BIOEXP_BTN_1,
    BIOEXP_STAT_0,
    BIOEXP_STAT_1,
    BIOEXP_DETECT_1,
    BIOEXP_PG,
};

typedef void (*bioexp_change_callback)(enum bioexp_ios io_mask, uint32_t io_states);

struct bioexp_listeners {
    bioexp_change_callback on_change;
};

SLL_LISTENER_DECLARE(bioexp, struct bioexp_listeners);

extern int bioexp_init();

extern int bioexp_mode(enum bioexp_ios io, bool inp);

extern int bioexp_get(enum bioexp_ios io);

extern int bioexp_set(enum bioexp_ios io, int val);
