#include "trapezoid.h"
double integral(double a, double b, int step, func f) {
  double integral_value;
  if (step <= 0) {
    integral_value = 0;
    return integral_value;
  } else {
    double h = (b - a) / step;
    double sum = 0;
    sum += f(a);
    for (int i = 1; i < step; i++) {
      sum += 2 * f(a + h * i);
    }
    sum += f(b);
    integral_value = 0.5 * h * sum;
    return integral_value;
  }
}
