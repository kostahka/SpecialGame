#include "system/background-system.hxx"

#include "Kengine/scene/scene.hxx"
#include "Kengine/units/rect.hxx"

#include "resources/resources.hxx"

struct planet_data
{
    Kengine::vec2 pos;
    Kengine::vec2 t_pos;
};

background_system::background_system(Kengine::scene& sc)
    : system(name, Kengine::system_render_type)
    , sc(sc)

{
    material = resources::background_material;
    if (material)
    {
        material->take_data();
    }
    planet_material = resources::planet_material;
    if (planet_material)
    {
        planet_material->take_data();
    }

    vao        = std::make_shared<Kengine::graphics::vertex_element_array>();
    planet_vao = std::make_shared<Kengine::graphics::vertex_element_array>();

    auto vbo =
        std::make_shared<Kengine::graphics::vertex_buffer<Kengine::vec2>>();

    static std::vector<Kengine::vec2> pos_vertices{
        {-1,  -1},
        { -1, 1 },
        { 1,  1 },
        { 1,  -1},
    };

    vbo->bind();
    vbo->allocate_vertices(pos_vertices.data(), 4, false);
    vbo->add_attribute_pointer(Kengine::graphics::vertex_attribute_pointer(
        Kengine::graphics::g_float, 2, 0, sizeof(Kengine::vec2)));

    auto ebo = std::make_shared<Kengine::graphics::element_buffer>();
    ebo->bind();

    static std::vector<uint32_t> indexes{ 0, 2, 1, 0, 3, 2 };

    ebo->allocate_indexes(indexes.data(), 6, false);

    vao->bind();
    vao->add_vertex_buffer(vbo);
    vao->set_elements(ebo);
    vao->unbind();

    auto planet_vbo =
        std::make_shared<Kengine::graphics::vertex_buffer<planet_data>>();

    static std::vector<planet_data> planet_pos_vertices{
        {{ -750.f, 450.f },  { 0.2f, 0.5f }},
        { { -750.f, 750 },   { 0.2f, 0.3f }},
        { { -450.f, 750.f }, { 0.4f, 0.3f }},
        { { -450.f, 450.f }, { 0.4f, 0.5f }},
    };

    planet_vbo->bind();
    planet_vbo->allocate_vertices(planet_pos_vertices.data(), 4, false);
    planet_vbo->add_attribute_pointer(
        Kengine::graphics::vertex_attribute_pointer(
            Kengine::graphics::g_float, 2, 0, sizeof(planet_data)));
    planet_vbo->add_attribute_pointer(
        Kengine::graphics::vertex_attribute_pointer(
            Kengine::graphics::g_float,
            2,
            offsetof(planet_data, t_pos),
            sizeof(planet_data)));

    planet_vao->bind();
    planet_vao->add_vertex_buffer(planet_vbo);
    planet_vao->set_elements(ebo);
    planet_vao->unbind();
}

background_system::~background_system()
{
    if (material)
    {
        material->free_data();
    }
}

std::size_t background_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t background_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t background_system::serialize_size() const
{
    return 0;
}

void background_system::on_start(Kengine::scene&) {}

void background_system::on_render(Kengine::scene& sc, int delta_ms)
{
    Kengine::graphics::render_packet packet(material);
    packet.instances_count = 1;
    packet.layer           = -90;
    packet.mode            = Kengine::graphics::draw_mode::triangles;
    packet.vao             = vao;
    packet.vertices_count  = 6;
    packet.vertices_start  = 0;
    sc.get_main_pass().add_render_packet(packet);

    Kengine::graphics::render_packet planet_packet(planet_material);
    planet_packet.instances_count = 1;
    planet_packet.layer           = -89;
    planet_packet.mode            = Kengine::graphics::draw_mode::triangles;
    planet_packet.vao             = planet_vao;
    planet_packet.vertices_count  = 6;
    planet_packet.vertices_start  = 0;
    sc.get_main_pass().add_render_packet(planet_packet);
}

void background_system::on_update(Kengine::scene&, int delta_ms) {}