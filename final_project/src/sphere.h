#ifndef SPHERE_H_
#define SPHERE_H_

#pragma once

#include <cmath>

namespace Sphere {
  double ConvertDegreesToRadians(double degrees);

  struct Point {
    double latitude;
    double longitude;

    static Point FromDegrees(double latitude, double longitude);
  };

  double Distance(Point lhs, Point rhs);
}

#endif /* SPHERE_H_ */
