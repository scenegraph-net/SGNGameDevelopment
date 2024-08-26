#include "Box2D.h"

#include <algorithm>


Box2D::Box2D(const glm::vec2& upperLeftCorner, const glm::vec2& lowerRightCorner)
   : UpperLeftCorner(upperLeftCorner), LowerRightCorner(lowerRightCorner)
{
}


Box2D::Box2D(const glm::vec2& centerPoint, float radius)
   : UpperLeftCorner({ centerPoint.x - radius, centerPoint.y - radius }),
   LowerRightCorner({ centerPoint.x + radius, centerPoint.y + radius })
{
}


const glm::vec2& Box2D::GetUpperLeft() const
{
   return UpperLeftCorner;
}


const glm::vec2& Box2D::GetLowerRight() const
{
   return LowerRightCorner;
}


glm::vec2& Box2D::GetUpperLeft()
{
   return UpperLeftCorner;
}


glm::vec2& Box2D::GetLowerRight()
{
   return LowerRightCorner;
}


Box2D Box2D::operator+(const Box2D& op) const
{
   return Box2D(
      glm::vec2(
         std::min(UpperLeftCorner.x, op.UpperLeftCorner.x),
         std::min(UpperLeftCorner.y, op.UpperLeftCorner.y)
      ),
      glm::vec2(
         std::max(LowerRightCorner.x, op.LowerRightCorner.x),
         std::max(LowerRightCorner.y, op.LowerRightCorner.y)
      )
   );
}


bool Box2D::Intersects(const Box2D& op) const
{
   return !(UpperLeftCorner.x > op.LowerRightCorner.x
      || UpperLeftCorner.y > op.LowerRightCorner.y
      || LowerRightCorner.x < op.UpperLeftCorner.x
      || LowerRightCorner.y < op.UpperLeftCorner.y
   );
}
