/// 
/// @file util_macro.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief 
/// @version 0.1
/// @date 2024-03-22
/// 
/// @copyright Copyright (c) 2024
/// 
/// 
#pragma once

#ifndef BIT
#define BIT(bit)                 (1UL << (bit))
#endif

#ifndef WRITE_BIT
#define WRITE_BIT(var, bit, val)      (var = (val) ? (var | BIT(bit)) : (var & (~BIT(bit))) )
#endif

#ifndef ARRAY_LEN
#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))
#endif
