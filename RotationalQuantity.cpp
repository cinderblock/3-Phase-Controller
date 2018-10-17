/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RotationalQuantity.cpp
 * Author: Cameron
 * 
 * Created on September 13, 2018, 11:01 AM
 */

#include "RotationalQuantity.h"

template<class T, T MAX, T MIN>
T RotationalQuantity<T, MAX, MIN>::wrap(T v) {
  while (v > MAX) {
    v -= RANGE;
  }
  while (v < MIN) {
    v += RANGE;
  }
  return v;
}

template<class T, T MAX, T MIN>
RotationalQuantity<T, MAX, MIN>::RotationalQuantity() {
  val = MIN;
}

template<class T, T MAX, T MIN>
RotationalQuantity<T, MAX, MIN>::RotationalQuantity(T n) {
  val = wrap(n);
}

template<class T, T MAX, T MIN>
RotationalQuantity<T, MAX, MIN>& RotationalQuantity<T, MAX, MIN>::operator+(RotationalQuantity<T, MAX, MIN>& that) {
  val = wrap(val + that.val - MIN);
  return *this;
}

template<class T, T MAX, T MIN>
RotationalQuantity<T, MAX, MIN>& RotationalQuantity<T, MAX, MIN>::operator-(RotationalQuantity<T, MAX, MIN>& that) {
  val = wrap(val - that.val + MIN);
  return *this;
}

template<class T, T MAX, T MIN>
RotationalQuantity<T, MAX, MIN>& RotationalQuantity<T, MAX, MIN>::operator+(T& that) {
  val = wrap(val + that);
  return *this;
}

template<class T, T MAX, T MIN>
RotationalQuantity<T, MAX, MIN>& RotationalQuantity<T, MAX, MIN>::operator-(T& that) {
  val = wrap(val - that);
  return *this;
}

template<class T, T MAX, T MIN>
typename RotationalQuantity<T, MAX, MIN>::Half RotationalQuantity<T, MAX, MIN>::toDifferenceRange() {
  return val;
}


