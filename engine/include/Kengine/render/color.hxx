#pragma once

#include "box2d/box2d.h"

namespace Kengine
{

struct color
{
    float r;
    float g;
    float b;
    float a;
    operator b2Color() const { return { r, g, b, a }; };
};

} // namespace Kengine
