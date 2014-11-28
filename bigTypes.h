/* 
 * File:   bigTypes.h
 * Author: chtacklind
 *
 * Created on December 14, 2011, 5:18 PM
 */

#ifndef BIGTYPES_H
#define	BIGTYPES_H

#include "basicTypes.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef union {
 struct {
  u1 A;
  u1 B;
 };
 u2 WORD;
} uf2;

typedef union {
 struct {
  u1 A;
  u1 B;
 };
 s2 WORD;
} sf2;

typedef union {
 struct {
  u1 A;
  u1 B;
  u1 C;
  u1 D;
 };
 u4 LONG;
} uf4;

#ifdef	__cplusplus
}
#endif

#endif	/* BIGTYPES_H */

