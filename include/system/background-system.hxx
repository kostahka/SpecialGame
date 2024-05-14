#pragma once

#include "Kengine/graphics/element-buffer.hxx"
#include "Kengine/graphics/vertex-array.hxx"
#include "Kengine/graphics/vertex-buffer.hxx"
#include "Kengine/resources/material-resource.hxx"
#include "Kengine/resources/res-ptr.hxx"
#include "Kengine/system/system.hxx"

#include <memory>

class background_system : public Kengine::system
{
public:
    static constexpr auto name = "background_system";

    background_system(Kengine::scene&);

    ~background_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;

private:
    std::shared_ptr<Kengine::graphics::vertex_element_array> vao;
    std::shared_ptr<Kengine::graphics::vertex_element_array> planet_vao;
    Kengine::res_ptr<Kengine::material_resource>             material;
    Kengine::res_ptr<Kengine::material_resource>             planet_material;
};
