#pragma once

#include "Kengine/resources/audio-resource.hxx"
#include "Kengine/resources/material-resource.hxx"
#include "Kengine/resources/res-ptr.hxx"

namespace resources
{
    void                                                initialize();
    extern Kengine::res_ptr<Kengine::material_resource> landscape_material;
    extern Kengine::res_ptr<Kengine::audio_resource>    landscape_damage_audio;
} // namespace resources