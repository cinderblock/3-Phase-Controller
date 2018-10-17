/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RotationalQuantity.h
 * Author: Cameron
 *
 * Created on September 13, 2018, 11:01 AM
 */

#ifndef ROTATIONALQUANTITY_H
#define ROTATIONALQUANTITY_H

template<class T, T MAX, T MIN = 0>
class RotationalQuantity {
  static constexpr T RANGE = MAX + 1 - MIN;
  
  static_assert(MAX > MIN, "MAX must be greater than MIN");
  static_assert((RANGE & 1) == 0, "RANGE (MAX - MIN + 1) must be even");

  T val;

  static T wrap(T v);

public:
  typedef RotationalQuantity<T, MAX - (RANGE / 2), MIN - (RANGE / 2)> Half;

  RotationalQuantity();
  RotationalQuantity(const T n);
  RotationalQuantity(const RotationalQuantity<T, MAX, MIN>& other);

  RotationalQuantity<T, MAX, MIN>& operator+(RotationalQuantity<T, MAX, MIN>& that);

  RotationalQuantity<T, MAX, MIN>& operator-(RotationalQuantity<T, MAX, MIN>& that);

  RotationalQuantity<T, MAX, MIN>& operator+(T& that);

  RotationalQuantity<T, MAX, MIN>& operator-(T& that);

  Half toDifferenceRange();

  inline T getRaw() {return val;}

private:

};

#endif /* ROTATIONALQUANTITY_H */

