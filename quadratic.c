#include "quadratic.h"
#include <math.h>
#include <stdbool.h>

int sign(double x, double epsilon) {
  if (fabs(x) <= epsilon) {
    return 1;
  }
  if (x >= epsilon) {
    return 1;
  }
  return -1;
}
struct Result Roots(double a, double b, double c, double eps) {
  double epsilon = eps;
  struct Result result;
  int ret;
  result.has_real_roots = true;
  result.is_quadratic = true;
  if (fabs(a) <= eps) {
    result.is_quadratic = false;
    return result;
  }
  double D = pow(b, 2) - (4 * a * c);
  if (D > eps) {
    result.roots[0] = -(b + sign(b, epsilon) * sqrt(D)) / (2 * a);
    result.roots[1] = c / (a * result.roots[0]);
    if (result.roots[0] > result.roots[1]) {
      double temp;
      temp = result.roots[0];
      result.roots[0] = result.roots[1];
      result.roots[1] = temp;
    }
    return result;
  }
  if (fabs(D) < eps) {
    result.roots[0] = -b / (2 * a);
    result.roots[1] = result.roots[0];
    return result;
  }
  result.has_real_roots = false;
  return result;
}
