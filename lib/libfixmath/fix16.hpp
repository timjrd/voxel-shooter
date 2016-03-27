#pragma once

#include "fix16.h"

class Fix16 {
public:
   fix16_t value;

   Fix16() { value = 0; }
   Fix16(const Fix16 &inValue)  { value = inValue.value; }
   Fix16(const int inValue)     { value = fix16_from_int(inValue); }

   int   toInt()   const { return fix16_to_int(value);   }
   float toFloat() const { return fix16_to_float(value); }

   Fix16 & operator=(const Fix16 &rhs)  { value = rhs.value;             return *this; }
   Fix16 & operator=(const int rhs)     { value = fix16_from_int(rhs);   return *this; }

   Fix16 & operator+=(const Fix16 &rhs)  { value += rhs.value;             return *this; }
   Fix16 & operator+=(const int rhs)     { value += fix16_from_int(rhs);   return *this; }

   Fix16 & operator-=(const Fix16 &rhs)  { value -= rhs.value; return *this; }
   Fix16 & operator-=(const int rhs)     { value -= fix16_from_int(rhs); return *this; }

   Fix16 & operator*=(const Fix16 &rhs)  { value = fix16_mul(value, rhs.value); return *this; }
   Fix16 & operator*=(const int rhs)     { value *= rhs; return *this; }

   Fix16 & operator/=(const Fix16 &rhs)  { value = fix16_div(value, rhs.value); return *this; }
   Fix16 & operator/=(const int rhs)     { value /= rhs; return *this; }

   const Fix16 operator+(const Fix16 &other) const  { Fix16 ret = *this; ret += other; return ret; }
   const Fix16 operator+(const int other)    const  { Fix16 ret = *this; ret += other; return ret; }

#ifndef FIXMATH_NO_OVERFLOW
   const Fix16 sadd(const Fix16 &other)  const { Fix16 ret = wrap( fix16_sadd(value, other.value) );             return ret; }
   const Fix16 sadd(const int other)     const { Fix16 ret = wrap( fix16_sadd(value, fix16_from_int(other)) );   return ret; }
#endif

   const Fix16 operator-(const Fix16 &other) const  { Fix16 ret = *this; ret -= other; return ret; }
   const Fix16 operator-(const int other)    const  { Fix16 ret = *this; ret -= other; return ret; }

   Fix16 operator-() const { return Fix16(0) - *this; }

#ifndef FIXMATH_NO_OVERFLOW
   const Fix16 ssub(const Fix16 &other)  const { Fix16 ret = wrap( fix16_sadd(value, -other.value) );           return ret; }
   const Fix16 ssub(const int other)     const { Fix16 ret = wrap( fix16_sadd(value, -fix16_from_int(other)) ); return ret; }
#endif

   const Fix16 operator*(const Fix16 &other) const  { Fix16 ret = *this; ret *= other; return ret; }
   const Fix16 operator*(const int other)    const  { Fix16 ret = *this; ret *= other; return ret; }

#ifndef FIXMATH_NO_OVERFLOW
   const Fix16 smul(const Fix16 &other)  const { Fix16 ret = wrap( fix16_smul(value, other.value) );           return ret; }
   const Fix16 smul(const int other)     const { Fix16 ret = wrap( fix16_smul(value, fix16_from_int(other)) ); return ret; }
#endif

   const Fix16 operator/(const Fix16 &other) const  { Fix16 ret = *this; ret /= other; return ret; }
   const Fix16 operator/(const int other)    const  { Fix16 ret = *this; ret /= other; return ret; }

#ifndef FIXMATH_NO_OVERFLOW
   const Fix16 sdiv(const Fix16 &other)  const { Fix16 ret = wrap(fix16_sdiv(value, other.value)); return ret; }
   const Fix16 sdiv(const int other)     const { Fix16 ret = wrap(fix16_sdiv(value, fix16_from_int(other))); return ret; }
#endif

   const int operator==(const Fix16 &other)  const { return (value == other.value);             }
   const int operator==(const int other)     const { return (value == fix16_from_int(other));   }

   const int operator!=(const Fix16 &other)  const { return (value != other.value);             }
   const int operator!=(const int other)     const { return (value != fix16_from_int(other));   }

   const int operator<=(const Fix16 &other)  const { return (value <= other.value);             }
   const int operator<=(const int other)     const { return (value <= fix16_from_int(other));   }

   const int operator>=(const Fix16 &other)  const { return (value >= other.value);             }
   const int operator>=(const int other)     const { return (value >= fix16_from_int(other));   }

   const int operator< (const Fix16 &other)  const { return (value <  other.value);             }
   const int operator< (const int other)     const { return (value <  fix16_from_int(other));   }

   const int operator> (const Fix16 &other)  const { return (value >  other.value);             }
   const int operator> (const int other)     const { return (value >  fix16_from_int(other));   }

   Fix16  sin() const { return wrap( fix16_sin(value) );  }
   Fix16  cos() const { return wrap( fix16_cos(value) );  }
   Fix16  tan() const { return wrap( fix16_tan(value) );  }
   Fix16 asin() const { return wrap( fix16_asin(value) ); }
   Fix16 acos() const { return wrap( fix16_acos(value) ); }
   Fix16 atan() const { return wrap( fix16_atan(value) ); }
   Fix16 atan2(const Fix16 &inY) const { return wrap( fix16_atan2(value, inY.value) ); }
   Fix16 sqrt() const { return wrap( fix16_sqrt(value) ); }

   Fix16  abs() const { return wrap( fix16_abs(value) );  }
   Fix16  mod(const Fix16 m) const { return wrap( fix16_mod(value, m.value) ); }

   static inline Fix16 div(int a, int b) {
      return Fix16(a) / Fix16(b);
   };

   static inline Fix16 wrap(fix16_t v) {
      Fix16 res;
      res.value = v;
      return res;
   };
};

const Fix16 FIX_PI   = Fix16::wrap(fix16_pi);

// The maximum representable value is 32767.999 985. The minimum value is -32768.0.
// The smallest unit (machine precision) of the datatype is 1/65536 = 0.000 015
const Fix16 FIX_TINY = Fix16(4) / Fix16(10000) / Fix16(10); // Real: 1e-6f
   
