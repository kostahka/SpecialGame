#include "resources/resources.hxx"

#include "Kengine/resources/resource-manager.hxx"
#include "Kengine/resources/shader-resource.hxx"

namespace resources
{
    Kengine::res_ptr<Kengine::audio_resource> landscape_damage_audio = nullptr;
    Kengine::res_ptr<Kengine::material_resource> landscape_material  = nullptr;
    Kengine::res_ptr<Kengine::material_resource> background_material = nullptr;
    Kengine::res_ptr<Kengine::material_resource> planet_material     = nullptr;

    void initialize()
    {
        landscape_material =
            Kengine::static_resource_cast<Kengine::material_resource>(
                Kengine::resource_manager::load_resource(
                    "resources/materials/landscape-material.kpkg"));
        landscape_damage_audio =
            Kengine::static_resource_cast<Kengine::audio_resource>(
                Kengine::resource_manager::load_resource(
                    "resources/audio/ground-damage-sound.kpkg"));
        background_material =
            Kengine::static_resource_cast<Kengine::material_resource>(
                Kengine::resource_manager::load_resource(
                    "resources/materials/background-material.kpkg"));
        planet_material =
            Kengine::static_resource_cast<Kengine::material_resource>(
                Kengine::resource_manager::load_resource(
                    "resources/materials/planet-material.kpkg"));
    }
} // namespace resources