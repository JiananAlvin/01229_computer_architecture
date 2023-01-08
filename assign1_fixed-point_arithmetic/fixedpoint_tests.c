#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
  Fixedpoint zero;
  Fixedpoint one;
  Fixedpoint one_half;
  Fixedpoint one_fourth;
  Fixedpoint three_fourth;
  Fixedpoint large1;
  Fixedpoint large2;
  Fixedpoint pos_min;
  Fixedpoint max;

} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_is_zero(TestObjs *object);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_double(TestObjs *objs);
void test_halve(TestObjs *objs);
void test_is_overflow_pos(TestObjs *objs);
void test_is_overflow_neg(TestObjs *objs);
void test_is_underflow_pos(TestObjs *objs);
void test_is_underflow_neg(TestObjs *objs);
void test_is_err(TestObjs *objs);
void test_compare(TestObjs *objs);

int main(int argc, char **argv) {
  // if a testname was specified on the command line, only that
  // test function will be executed
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_whole_part);
  TEST(test_frac_part);
  TEST(test_is_zero);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_negate);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_double);
  TEST(test_halve);
  TEST(test_is_overflow_pos);
  TEST(test_is_overflow_neg);
  TEST(test_is_underflow_pos);
  TEST(test_is_underflow_neg);
  TEST(test_is_err);
  TEST(test_compare);

  // IMPORTANT: if you add additional test functions (which you should!),
  // make sure they are included here.  E.g., if you add a test function
  // "my_awesome_tests", you should add
  //
  //   TEST(my_awesome_tests);
  //
  // here. This ensures that your test function will actually be executed.

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));

  objs->zero = fixedpoint_create(0UL);
  objs->one = fixedpoint_create(1UL);
  objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
  objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
  objs->three_fourth = fixedpoint_create2(0UL, 0xc000000000000000UL);
  objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
  objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  objs->pos_min = fixedpoint_create2(0UL, 0x0000000000000001UL);
  objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_whole_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0UL == fixedpoint_whole_part(objs->three_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
}

void test_frac_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half)); 
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xc000000000000000UL == fixedpoint_frac_part(objs->three_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_is_zero(TestObjs *objs) {
  ASSERT(1 == fixedpoint_is_zero(objs->zero));
  ASSERT(0 == fixedpoint_is_zero(objs->one));
  ASSERT(0 == fixedpoint_is_zero(objs->one_half));
  ASSERT(0 == fixedpoint_is_zero(objs->three_fourth));
  ASSERT(0 == fixedpoint_is_zero(objs->large2));
}

void test_create_from_hex(TestObjs *objs) {
  (void) objs;

  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val1));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));
  ASSERT(val1.sign == 0);

  Fixedpoint val2 = fixedpoint_create_from_hex("-f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val2));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val2));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val2));
  ASSERT(val2.sign == 1);

  Fixedpoint val3 = fixedpoint_create_from_hex("-0");
  ASSERT(fixedpoint_is_valid(val3));
  ASSERT(0 == fixedpoint_whole_part(val3));
  ASSERT(0 == fixedpoint_frac_part(val3));
  ASSERT(val3.sign == 1);

  Fixedpoint val4 = fixedpoint_create_from_hex("0");
  ASSERT(fixedpoint_is_valid(val4));
  ASSERT(0 == fixedpoint_whole_part(val4));
  ASSERT(0 == fixedpoint_frac_part(val4));
  ASSERT(val4.sign == 0);

  Fixedpoint val5 = fixedpoint_create_from_hex("8");
  ASSERT(fixedpoint_is_valid(val5));
  ASSERT(8 == fixedpoint_whole_part(val5));
  ASSERT(0 == fixedpoint_frac_part(val5));
  ASSERT(val5.sign == 0);

  Fixedpoint val6 = fixedpoint_create_from_hex("0.00f200000000");
  ASSERT(fixedpoint_is_valid(val6));
  ASSERT(0 == fixedpoint_whole_part(val6));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val6));
  ASSERT(val6.sign == 0);

  Fixedpoint val7 = fixedpoint_create_from_hex("-0.00f25cb00000");
  ASSERT(fixedpoint_is_valid(val7));
  ASSERT(0 == fixedpoint_whole_part(val7));
  ASSERT(0x00f25cb000000000UL == fixedpoint_frac_part(val7));
  ASSERT(val7.sign == 1);

  Fixedpoint val8 = fixedpoint_create_from_hex("-8");
  ASSERT(fixedpoint_is_valid(val8));
  ASSERT(8 == fixedpoint_whole_part(val8));
  ASSERT(0 == fixedpoint_frac_part(val8));
  ASSERT(val8.sign == 1);
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = fixedpoint_format_as_hex(objs->zero);
  ASSERT(0 == strcmp(s, "0"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one);
  ASSERT(0 == strcmp(s, "1"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_half);
  ASSERT(0 == strcmp(s, "0.8"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_fourth);
  ASSERT(0 == strcmp(s, "0.4"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large1);
  ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large2);
  ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
  free(s);
}

void test_negate(TestObjs *objs) {
  // none of the test fixture objects are negative
  ASSERT(!fixedpoint_is_neg(objs->zero));
  ASSERT(!fixedpoint_is_neg(objs->one));
  ASSERT(!fixedpoint_is_neg(objs->one_half));
  ASSERT(!fixedpoint_is_neg(objs->one_fourth));
  ASSERT(!fixedpoint_is_neg(objs->large1));
  ASSERT(!fixedpoint_is_neg(objs->large2));

  // negate the test fixture values
  Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
  Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
  Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
  Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

  // zero does not become negative when negated
  ASSERT(!fixedpoint_is_neg(zero_neg));
  ASSERT(!fixedpoint_is_neg(fixedpoint_create_from_hex("-0.0")));  // Test -0.0 is non-negative.

  // all of the other values should have become negative when negated
  ASSERT(fixedpoint_is_neg(one_neg));
  ASSERT(fixedpoint_is_neg(one_half_neg));
  ASSERT(fixedpoint_is_neg(one_fourth_neg));
  ASSERT(fixedpoint_is_neg(large1_neg));
  ASSERT(fixedpoint_is_neg(large2_neg));

  // magnitudes should stay the same
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, sum;

  lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  rhs = fixedpoint_create_from_hex("d09079.1e6d601");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
  ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));

  lhs = fixedpoint_create_from_hex("-e8a5cd.f474d");
  rhs = fixedpoint_create_from_hex("-822c4f.c8ba2");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0x16ad21dUL == fixedpoint_whole_part(sum));
  ASSERT(0xbd2ef00000000000UL == fixedpoint_frac_part(sum));

  lhs = fixedpoint_create_from_hex("55840398.9ef2a");
  rhs = fixedpoint_create_from_hex("-99adce43f6a.1469d");       
  sum = fixedpoint_add(lhs, rhs);                             
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0x99a87603bd1UL == fixedpoint_whole_part(sum));
  ASSERT(0x7577300000000000UL == fixedpoint_frac_part(sum));

  lhs = objs->pos_min;
  rhs = objs->max;      
  sum = fixedpoint_add(lhs, rhs);          
  ASSERT(fixedpoint_is_overflow_pos(sum));
   
  lhs = fixedpoint_create_from_hex("8"); 
  rhs = fixedpoint_create_from_hex("7"); 
  sum = fixedpoint_add(lhs, rhs); 
  ASSERT(!fixedpoint_is_neg(sum));
  ASSERT(0xfUL == fixedpoint_whole_part(sum));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(sum)); 
 
  lhs = fixedpoint_create_from_hex("8"); 
  rhs = fixedpoint_create_from_hex("-7"); 
  sum = fixedpoint_add(lhs, rhs); 
  ASSERT(!fixedpoint_is_neg(sum));
  ASSERT(0x1UL == fixedpoint_whole_part(sum));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(sum)); 

  lhs = fixedpoint_create_from_hex("-8"); 
  rhs = fixedpoint_create_from_hex("-7"); 
  sum = fixedpoint_add(lhs, rhs); 
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0xfUL == fixedpoint_whole_part(sum));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(sum)); 
}

void test_sub(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, diff;

  lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
  rhs = fixedpoint_create_from_hex("f676e8.58");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff));
  ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff));
}

void test_double(TestObjs *objs) {
  (void) objs;

  Fixedpoint val, dbl;

  val = fixedpoint_create_from_hex("fbbce8.58"); 
  dbl = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_neg(dbl));
  ASSERT(0x1f779d0UL == fixedpoint_whole_part(dbl));
  ASSERT(0xb000000000000000UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("8"); 
  dbl = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_neg(dbl));
  ASSERT(0x10UL == fixedpoint_whole_part(dbl));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(dbl));
  
  val = fixedpoint_create_from_hex("0.8"); 
  dbl = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_neg(dbl));
  ASSERT(0x1UL == fixedpoint_whole_part(dbl));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("0"); 
  dbl = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_neg(dbl));
  ASSERT(0UL == fixedpoint_whole_part(dbl));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("-0"); 
  dbl = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_neg(dbl));
  ASSERT(0UL == fixedpoint_whole_part(dbl));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("-5"); 
  dbl = fixedpoint_double(val);
  ASSERT(fixedpoint_is_neg(dbl));
  ASSERT(0xaUL == fixedpoint_whole_part(dbl));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("-5.5"); 
  dbl = fixedpoint_double(val);
  ASSERT(fixedpoint_is_neg(dbl));
  ASSERT(0xaUL == fixedpoint_whole_part(dbl));
  ASSERT(0xa000000000000000UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("8888888888888888"); 
  dbl = fixedpoint_double(val);
  ASSERT(fixedpoint_is_overflow_pos(dbl));

  val = fixedpoint_create_from_hex("0.8888888888888888"); 
  dbl = fixedpoint_double(val);
  ASSERT(!fixedpoint_is_neg(dbl));
  ASSERT(!fixedpoint_is_overflow_pos(dbl));
  ASSERT(0x1UL == fixedpoint_whole_part(dbl));
  ASSERT(0x1111111111111110UL == fixedpoint_frac_part(dbl));

  val = fixedpoint_create_from_hex("-8888888888888888.888888888888888a"); 
  dbl = fixedpoint_double(val);
  ASSERT(fixedpoint_is_overflow_neg(dbl));

  val = fixedpoint_create_from_hex("8888888888888888.f88888888888888a"); 
  dbl = fixedpoint_double(val);
  ASSERT(fixedpoint_is_overflow_pos(dbl));

  dbl = fixedpoint_double(objs->max);
  ASSERT(fixedpoint_is_overflow_pos(dbl));

  val = objs->max; 
  dbl = fixedpoint_double(val);
  ASSERT(fixedpoint_is_overflow_pos(dbl));
}

void test_halve(TestObjs *objs) {
  (void) objs;

  Fixedpoint val, half;

  val = fixedpoint_create_from_hex("1f779d0.b"); 
  half = fixedpoint_halve(val);
  ASSERT(0xfbbce8UL == fixedpoint_whole_part(half));
  ASSERT(0x5800000000000000UL == fixedpoint_frac_part(half));

  val = fixedpoint_create2(0xac5UL, 0x7000000000000000UL);
  half = fixedpoint_halve(val);
  ASSERT(0x562UL == fixedpoint_whole_part(half));
  ASSERT(0xb800000000000000UL == fixedpoint_frac_part(half));

  half = fixedpoint_halve(objs->one);
  ASSERT(fixedpoint_whole_part(objs->one_half) == fixedpoint_whole_part(half));
  ASSERT(fixedpoint_frac_part(objs->one_half) == fixedpoint_frac_part(half));
}

void test_is_overflow_pos(TestObjs *objs) {
  Fixedpoint sum;

  sum = fixedpoint_add(objs->max, objs->one);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_add(objs->one, objs->max);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  Fixedpoint negative_one = fixedpoint_negate(objs->one);

  sum = fixedpoint_sub(objs->max, negative_one);
  ASSERT(fixedpoint_is_overflow_pos(sum));
}

void test_is_overflow_neg(TestObjs *objs) {
  Fixedpoint sub;

  Fixedpoint neg_max = fixedpoint_negate(objs->max);

  sub = fixedpoint_sub(neg_max, objs->pos_min);
  ASSERT(fixedpoint_is_overflow_neg(sub));
}

void test_is_underflow_pos(TestObjs *objs) {
  Fixedpoint val, half;

  val = fixedpoint_create2(0UL, 0x0000000000000001UL);
  half = fixedpoint_halve(val);
  ASSERT(fixedpoint_is_underflow_pos(half));

  half = fixedpoint_halve(objs->one_fourth);
  ASSERT(!fixedpoint_is_underflow_pos(half));

  val = fixedpoint_create2(0xac5UL, 0x7000000000000000UL);
  half = fixedpoint_halve(val);
  ASSERT(!fixedpoint_is_underflow_pos(half));
}

void test_is_underflow_neg(TestObjs *objs) {
  Fixedpoint val, half;

  val = fixedpoint_create_from_hex("-0.0000000000000001");
  half = fixedpoint_halve(val);
  ASSERT(fixedpoint_is_underflow_neg(half));

  val = fixedpoint_negate(objs->one_fourth);
  half = fixedpoint_halve(val);
  ASSERT(!fixedpoint_is_underflow_neg(half));

  val = fixedpoint_create_from_hex("-ac5.700000000000000b");
  half = fixedpoint_halve(val);
  ASSERT(fixedpoint_is_underflow_neg(half));
}

void test_is_err(TestObjs *objs) {
  (void) objs;

  // too many characters
  Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
  ASSERT(fixedpoint_is_err(err1));

  // too many characters
  Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
  ASSERT(fixedpoint_is_err(err2));

  // this one is actually fine
  Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
  ASSERT(fixedpoint_is_valid(err3));
  ASSERT(!fixedpoint_is_err(err3));

  // whole part is too large
  Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
  ASSERT(fixedpoint_is_err(err4));

  // fractional part is too large
  Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
  ASSERT(fixedpoint_is_err(err5));

  // invalid hex digits in whole part
  Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
  ASSERT(fixedpoint_is_err(err6));

  // invalid hex digits in fractional part
  Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
  ASSERT(fixedpoint_is_err(err7));
}

void test_compare(TestObjs *objs) {
  Fixedpoint lhs, rhs;
  
  Fixedpoint neg_one = fixedpoint_negate(objs->one); 
  ASSERT(1 == fixedpoint_compare(objs->one, neg_one));

  
  lhs = fixedpoint_create_from_hex("8"); 
  rhs = fixedpoint_create_from_hex("7"); 
  ASSERT(1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("-8"); 
  rhs = fixedpoint_create_from_hex("7"); 
  ASSERT(-1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("-8"); 
  rhs = fixedpoint_create_from_hex("-7"); 
  ASSERT(-1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("55840398.9ef2a");
  rhs = fixedpoint_create_from_hex("-99adce43f6a.1469d");  
  ASSERT(1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("99adce43f6a.1469");
  rhs = fixedpoint_create_from_hex("99adce43f6a.1469d");  
  ASSERT(-1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("-99adce43f6a.1469");
  rhs = fixedpoint_create_from_hex("-99adce43f6a.1469d");  
  ASSERT(1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("0.9ef2a");
  rhs = fixedpoint_create_from_hex("0.1469d");  
  ASSERT(1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("0.9ef2a");
  rhs = fixedpoint_create_from_hex("-0.1469d");  
  ASSERT(1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("-0.9ef2a");
  rhs = fixedpoint_create_from_hex("-0.1469d");  
  ASSERT(-1 == fixedpoint_compare(lhs, rhs));

  lhs = fixedpoint_create_from_hex("-0"); 
  ASSERT(0 == fixedpoint_compare(lhs, objs->zero));
}
