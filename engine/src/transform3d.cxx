#include "Kengine/transform3d.hxx"

#include <cmath>

namespace Kengine
{
transform2d transform2d::operator+(const transform2d& t) const
{
    return { x + t.x, y + t.y };
}

transform2d transform2d::operator-(const transform2d& t) const
{
    return { x - t.x, y - t.y };
}

transform2d::operator b2Vec2() const
{
    return { x, y };
}

float transform2d::distance(const transform2d& t) const
{
    return std::sqrt(std::pow(t.x - x, 2) + std::pow(t.y - y, 2));
}

} // namespace Kengine
