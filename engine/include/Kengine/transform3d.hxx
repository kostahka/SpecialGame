#pragma once

#include "box2d/box2d.h"

namespace Kengine
{
struct itransform2d
{
    int x;
    int y;
};

struct transform2d
{
    float              x;
    float              y;
    static transform2d blend(const transform2d& t1,
                             const transform2d& t2,
                             const float        a)
    {
        float x = (1.0f - a) * t1.x + a * t2.x;
        float y = (1.0f - a) * t1.y + a * t2.y;
        return { x, y };
    };

    float distance(const transform2d& t) const;

    transform2d operator+(const transform2d& t) const;
    transform2d operator-(const transform2d& t) const;
    operator b2Vec2() const;
};

struct transform3d
{
    float x;
    float y;
    float z;

    static transform3d blend(const transform3d& t1,
                             const transform3d& t2,
                             const float        a)
    {
        float x = (1.0f - a) * t1.x + a * t2.x;
        float y = (1.0f - a) * t1.y + a * t2.y;
        float z = (1.0f - a) * t1.z + a * t2.z;
        return { x, y, z };
    };
};
} // namespace Kengine
