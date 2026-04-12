#include "trapezoid.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#define PI 3.1415926535

double test_func_1(double x) { return 3 * x + 1; }
double test_func_2(double x) { return pow(x, 2.0); }
double test_func_3(double x) { return sin(x); }
double test_func_4(double x) { return exp(x); }
double test_func_5(double x) { return pow(x, 3.0) - 4 * x; }

double epsilon = 1e-6;

void test_trapezoid_1() {
  double res = integral(0, 2, 100, test_func_1);
  printf("%.20f\n", res);
  assert(fabs(res - 8) < epsilon);
}

void test_trapezoid_2() {
  double res = integral(0, 1, 1500, test_func_2);
  printf("%.20f\n", res);
  assert(fabs(res - 0.33333333) < epsilon);
}

void test_trapezoid_3() {
  double res = integral(0, PI, 1500, test_func_3);
  printf("%.20f\n", res);
  assert(fabs(res - 2) < epsilon);
}

void test_trapezoid_4() {
  double res = integral(0, 1, 1500, test_func_4);
  printf("%.20f\n", res);
  assert(fabs(res - 1.718281) < epsilon);
}

void test_trapezoid_5() {
  double res = integral(0, 2, 2000, test_func_5);
  printf("%.20f\n", res);
  assert(fabs(res + 4) < epsilon);
}

int main() {
  test_trapezoid_1();
  test_trapezoid_2();
  test_trapezoid_3();
  test_trapezoid_4();
  test_trapezoid_5();
  return 0;
}
