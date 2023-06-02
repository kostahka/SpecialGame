#pragma once

#include "transform3d.hxx"
#include <string>
namespace Kengine
{

class texture_object
{
public:
    virtual ~texture_object() = default;

    virtual void         bind()           = 0;
    virtual itransform2d get_size() const = 0;
};

texture_object* create_texture(std::string texture_path);

} // namespace Kengine
