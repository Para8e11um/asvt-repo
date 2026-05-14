#include "quadratic.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>

bool root_comparison(double root, double expected, int precision) {
  return fabs(root - expected) < pow(10, -precision);
}

double eps = 1e-11;

void test_quadratic_1() {
  struct Result result = Roots(0, 1, 1, eps);
  assert(result.is_quadratic == false);
}

void test_quadratic_2() {
  struct Result result = Roots(1, 0, -1, eps);
  assert(root_comparison(result.roots[0], -1, 11) &&
         root_comparison(result.roots[1], 1, 11));
}

void test_quadratic_3() {
  struct Result result = Roots(1, 0, 0, eps);
  assert(root_comparison(result.roots[0], 0, 11));
  assert(root_comparison(result.roots[1], 0, 11));
}

void test_quadratic_4() {
  struct Result result = Roots(1, 0, 1, eps);
  assert(result.has_real_roots == false);
}

void test_quadratic_5() {
  struct Result result = Roots(1, 0, -1e-7, eps);
  assert(root_comparison(result.roots[0], -3e-4, 4) &&
         root_comparison(result.roots[1], 3e-4, 4));
}

void test_quadratic_6() {
  struct Result result = Roots(1, -1e+10, -1, eps);
  assert(root_comparison(result.roots[0], -1e-10, 11) &&
         root_comparison(result.roots[1], 1e+10, 11));
}

void test_quadratic_7() {
  struct Result result = Roots(1, 0, -1e-8, 1e-7);
  assert(root_comparison(result.roots[0], 0, 7));
  assert(root_comparison(result.roots[1], 0, 7));
}

int main() {
  test_quadratic_1();
  test_quadratic_2();
  test_quadratic_3();
  test_quadratic_4();
  test_quadratic_5();
  test_quadratic_6();
  test_quadratic_7();
  return 0;
}
