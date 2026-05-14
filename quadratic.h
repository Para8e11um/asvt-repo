#include <stdbool.h>

struct Result {
  double roots[2];
  bool is_quadratic;
  bool has_real_roots;
};

struct Result Roots(double a, double b, double c, double eps);
int sign(double x, double epsilon);
