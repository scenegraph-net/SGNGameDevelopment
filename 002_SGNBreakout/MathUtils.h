#pragma once

#include <glm/glm.hpp>


template<typename T>
inline T lerp(T v0, T v1, double t)
{
   return v0 + static_cast<T>(t * (v1 - v0));
}


template<typename T>
inline double inverseLerp(T v0, T v1, T t)
{
   return (v0 == v1)
      ? .0
      : static_cast<double>(t - v0) / static_cast<double>(v1 - v0);
}


// solveQuadratic(): Solves a quadratic equation on the form Ax^2 + Bx + C = 0.
// The return value indicates how many solutions we found (0, 1 or 2)
// The actual solutions are returned through the out_solution1 and out_solution2 parameters.
template<typename T>
int solveQuadratic(T a, T b, T c, T& out_solution1, T& out_solution2)
{
   out_solution1 = out_solution2 = std::numeric_limits<T>::quiet_NaN();

   if (T(0) == a)             // If a is 0, we have a linear equation
   {
      if (T(0) == b)          // If b is also 0, we don't really have an equation at all
         return 0;

      out_solution1 = -c / b;
      return 1;
   }

   T discriminant = b * b - T(4) * a * c;

   if (discriminant < T(0))   // If the discriminant is negative, we have no real roots
      return 0;

   // Now we can calculate the first root
   T denominator = T(2) * a;
   T sqrexp = static_cast<T>(sqrt(discriminant));
   T result1 = (-b + sqrexp) / denominator;

   // If the discriminant is zero, we have only the one root that we already calculated
   if (T(0) == discriminant)
   {
      out_solution1 = result1;
      return 1;
   }

   // Otherwise we have two different roots - let's calculate the other one
   T result2 = (-b - sqrexp) / denominator;
   out_solution1 = (std::min)(result1, result2);
   out_solution2 = (std::max)(result1, result2);
   return 2;
}
