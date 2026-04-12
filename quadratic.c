#include "quadratic.h"
#include <math.h>
#include <stdbool.h>

int sign(double x, double epsilon) {
  if (x <= epsilon && x >= -epsilon) {
    return 1;
  } else if (x >= epsilon) {
    return 1;
  } else if (x <= -epsilon) {
    return -1;
  }
  return 1;
}

struct Result Roots(double a, double b, double c) {
  double epsilon = 1e-11;
  double small_epsilon = 1e-7;
  struct Result result;
  int ret;
  result.has_real_roots = true;
  result.is_quadratic = true;
  if (a == 0) {
    result.is_quadratic = false;
    return result;
  } else {
    double D = pow(b, 2) - (4 * a * c);
    if (D > small_epsilon) {
      result.roots[0] = -(b + sign(b, epsilon) * sqrt(D)) / (2 * a);
      result.roots[1] = c / (a * result.roots[0]);
      if (result.roots[0] > result.roots[1]) {
        double temp;
        temp = result.roots[0];
        result.roots[0] = result.roots[1];
        result.roots[1] = temp;
      }
      return result;
    } else if (D <= small_epsilon && D >= -small_epsilon) {
      result.roots[0] = -b / (2 * a);
      return result;
    } else if (D < small_epsilon) {
      result.has_real_roots = false;
      return result;
    }
  }
}
