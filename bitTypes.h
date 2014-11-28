/*
* File: bitTypes.h
* Author: chtacklind
*
* Created on December 13, 2011, 4:00 PM
*/

#ifndef BITTYPES_H
#define BITTYPES_H

#include "basicTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u1 b1;
typedef u1 b2;
typedef u1 b3;
typedef u1 b4;
typedef u1 b5;
typedef u1 b6;
typedef u1 b7;

//#define BITFIELD u1 BYTE; struct {b1 B0:1; b1 B1:1; b1 B2:1;
// b1 B3:1; b1 B4:1; b1 B5:1; b1 B6:1; b1 B7:1;}
typedef union {
u1 byte;
struct {b2 bits:2;};
struct {b1 B0:1; b1 B1:1;};
} bf2;
typedef union {
u1 byte;
struct {b3 bits:3;};
struct {b1 B0:1; b1 B1:1; b1 B2:1;};
} bf3;
typedef union {
u1 byte;
struct {b4 bits:4;};
struct {b1 B0:1; b1 B1:1; b1 B2:1; b1 B3:1;};
} bf4;
typedef union {
u1 byte;
struct {b5 bits:5;};
struct {b1 B0:1; b1 B1:1; b1 B2:1; b1 B3:1; b1 B4:1;};
} bf5;
typedef union {
u1 byte;
struct {b6 bits:6;};
struct {b1 B0:1; b1 B1:1; b1 B2:1; b1 B3:1; b1 B4:1; b1 B5:1;};
} bf6;
typedef union {
u1 byte;
struct {b7 bits:7;};
struct {b1 B0:1; b1 B1:1; b1 B2:1; b1 B3:1; b1 B4:1; b1 B5:1; b1 B6:1;};
} bf7;
typedef union {
u1 byte;
u1 bits;
struct {b1 B0:1; b1 B1:1; b1 B2:1; b1 B3:1; b1 B4:1; b1 B5:1; b1 B6:1; b1 B7:1;};
} bf8;

#ifdef __cplusplus
}
#endif

#endif /* BITTYPES_H */
