#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint fp;
  fp.tag = 1;
  fp.sign = 0;
  fp.whole = whole;
  fp.frac = 0; 
  return fp;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint fp;
  fp.tag = 1;
  fp.sign = 0;
  fp.whole = whole;
  fp.frac = frac;
  return fp;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  Fixedpoint fp;
   char *ptr_point;
   char *ptr_end;
  
  // Determine the sign of the FixedPoint.
   if (hex[0] == '-') {
    fp.whole = strtoul(hex + 1, &ptr_point, 16); 
    if ((ptr_point - hex) < 18) {
      fp.tag = 1;
      fp.sign = 1;
    } else {
      fp.tag = 0;
    }
   } else {
    fp.whole = strtoul(hex, &ptr_point, 16);
    if ((ptr_point - hex) < 17) {
      fp.tag = 1;
      fp.sign = 0;
    } else {
      fp.tag = 0;
    }
   }

  // Create the Fixedpoint based on its form.
  if (ptr_point[0] == '.' && fp.tag == 1) {
    fp.frac = strtoul(ptr_point + 1, &ptr_end, 16);
    int frac_length = ptr_end - (ptr_point + 1);  // Get the length of the hex/fractional part.
    if (ptr_end[0] == '\0' && frac_length < 17) {
      fp.frac = fp.frac << 4*(16 - frac_length);  // Zero padding. It makes sure that the fractional part is always 64bits.
    } else {
      fp.tag = 0;
    }
  } else if (ptr_point[0] == '\0' && fp.tag == 1) {
    fp.frac = 0;
  } else {
    fp.tag = 0;
  }
  return fp;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  Fixedpoint fp;
  // Here we consider (+) + (+) = (+) and
  //                  (-) + (-) = (-)
  if (left.sign == right.sign) {
    fp.whole = left.whole + right.whole;
    fp.frac = left.frac + right.frac;
    fp.tag = 1;
    fp.sign = left.sign;
    if (fp.frac < left.frac || fp.frac < right.frac) { 
      fp.whole++;  // If the fractional part overflows, the whole part adds 1.
      // Special case: ffffffffffffffff.ffffffffffffffff * 2 = ffffffffffffffff.fffffffffffffffe
      // Overflow
      uint64_t max = 0xffffffffffffffffUL;
      if (left.whole == max && right.whole == max && fp.whole == max) { 
        fp.tag = 2; 
        return fp;
      }
    }

    if (fp.whole < left.whole || fp.whole < right.whole) {  // The sum must be greater than or equal to the addends, otherwise the sum will be overflow.
      fp.tag = 2; 
    }

  // Here we consider (+) + (-) = (sum_sign) and
  //                  (-) + (+) = (sum_sign)
  // The sum_sign is as the same as the sign of the addend that has a greater absolute value.
  } else {
    fp.tag = 1;
    if (left.whole < right.whole) {
      fp.sign = right.sign;
      fp.whole = right.whole - left.whole;
      fp.frac = right.frac - left.frac;
      if (right.frac < left.frac) { 
        fp.whole--; // Borrow 1 from the whole part.
      }
    } else if (left.whole > right.whole) {
      fp.sign = left.sign;
      fp.whole = left.whole - right.whole; 
      fp.frac = left.frac - right.frac; 
      if (left.frac < right.frac) {
        fp.whole--;
      }
    } else {
      fp.whole = 0; 
      if (left.frac < right.frac) {
        fp.sign = right.sign;
        fp.frac = right.frac - left.frac; 
      } else {
        fp.sign = left.sign;
        fp.frac = left.frac - right.frac; 
      }
    }
  }
  return fp;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  // a - b <=> a + (-b)
  return fixedpoint_add(left, fixedpoint_negate(right));
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  if (!fixedpoint_is_zero(val)) { // If the Fixedpoint is not 0, we negate the Fixedpoint.
    val.sign = (val.sign == 1 ? 0 : 1);
  }
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  Fixedpoint fp;
  fp.whole = val.whole / 2;
  fp.frac = val.frac / 2;
  fp.tag = 1;
  fp.sign = val.sign;
  // If the fractional part of the Fixedpoint is an odd, then halving the Fixedpoint results in underflow.
  if (val.frac % 2 != 0) { 
    fp.tag = 3;
  } 


  if (val.whole % 2 != 0) {
    // When the whole part is an odd, halving the whole part results in XXX.5,
    // and 0.5 is equivalent to add 0x8000000000000000UL to the halved fractional part.
    fp.frac = val.frac/2 + 0x8000000000000000UL;
  }
  return fp;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  // a*2 <=> a + a
  return fixedpoint_add(val, val);
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // -0 == 0
  if (fixedpoint_is_zero(left) && fixedpoint_is_zero(right)) {
    return 0;
  }
  // When a, b has the same sign :
  // a - b = 0 => a = b
  // a - b > 0 => a > b
  // a - b < 0 => a < b
  if (left.sign == right.sign) {
    Fixedpoint diff = fixedpoint_sub(left, right);
    if (fixedpoint_is_zero(diff)) {
      return 0;
    } else if (diff.sign == 0) {
      return 1;
    } else {
      return -1;
    }
  } else if (left.sign == 1 && right.sign == 0) { // If two numbers have different signs, the positive one is always greater.
    return -1;
  } else {
    return 1;
  }
}

int fixedpoint_is_zero(Fixedpoint val) {
  // If val is a valid Fixedpoint value equal to 0, both whole and fractional parts should be 0.
  return ((val.whole == 0 && val.frac == 0 && val.tag == 1) ? 1 : 0);
}

int fixedpoint_is_err(Fixedpoint val) {
  return (val.tag == 0 ? 1 : 0);
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (!fixedpoint_is_zero(val)) {
    return (val.sign == 1 ? 1 : 0);
  }  
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  return ((val.sign == 1 && val.tag == 2) ? 1 : 0);
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  return ((val.sign == 0 && val.tag == 2) ? 1 : 0);
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  return ((val.sign == 1 && val.tag == 3) ? 1 : 0);
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  return ((val.sign == 0 && val.tag == 3) ? 1 : 0);
}

int fixedpoint_is_valid(Fixedpoint val) {
  return (val.tag == 1 ? 1 : 0);
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  char *s = (char*) malloc(34);
  char temp[34] = "";
  if (val.tag != 1) {
    strcpy(s, "<invalid>");
  }
  
  if (val.sign == 1) {
    strcat(s, "-");
  }
  // Concatenate the whole part.
  sprintf(temp, "%lx", val.whole);
  strcat(s, temp);

  // Concatenate the fractional part, if there is one.
  if (val.frac != 0UL) {
    strcat(s, ".");
    sprintf(temp, "%016lx", val.frac); // Left-pad the number with zeroes to ensure the length of the string is 16.
    for (int i = 15; i > 0; i--) {
      if (temp[i] != '0') {
        break;
      } else {
        temp[i] = '\0';
      }
    }
    strcat(s, temp);
  }
  return s;
}
