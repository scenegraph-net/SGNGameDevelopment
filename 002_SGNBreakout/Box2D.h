#pragma once

#include <glm/glm.hpp>


class Box2D
{
   public:
      Box2D() = default;

      Box2D(const glm::vec2& upperLeftCorner, const glm::vec2& lowerRightCorner);
      Box2D(const glm::vec2& centerPoint, float radius);

      const glm::vec2& GetUpperLeft() const;
      const glm::vec2& GetLowerRight() const;

      glm::vec2& GetUpperLeft();
      glm::vec2& GetLowerRight();

      Box2D operator+(const Box2D& op) const;

      bool Intersects(const Box2D& op) const;

   private:
      glm::vec2 UpperLeftCorner;
      glm::vec2 LowerRightCorner;
};
