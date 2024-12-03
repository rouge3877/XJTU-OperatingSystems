#ifndef _TYPE_H_
#define _TYPE_H_

#define NULL ((void *)0)

#define EOS '\0'

#define EOF -1

#define bool _Bool
#define true 1
#define false 0

#define _packed __attribute__((packed))

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned long size_t;

#endif