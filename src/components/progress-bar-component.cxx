#include "components/progress-bar-component.hxx"

#include "imgui.h"

progress_bar_component::progress_bar_component()
    : component(name)
{
}

std::size_t progress_bar_component::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, uv_size_start.x);
    size += Kengine::serialization::write(os, uv_size_start.y);
    size += Kengine::serialization::write(os, uv_size_end.x);
    size += Kengine::serialization::write(os, uv_size_end.y);
    size += Kengine::serialization::write(os, scale_start.x);
    size += Kengine::serialization::write(os, scale_start.y);
    size += Kengine::serialization::write(os, scale_end.x);
    size += Kengine::serialization::write(os, scale_end.y);

    return size;
}

std::size_t progress_bar_component::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, uv_size_start.x);
    size += Kengine::serialization::read(is, uv_size_start.y);
    size += Kengine::serialization::read(is, uv_size_end.x);
    size += Kengine::serialization::read(is, uv_size_end.y);
    size += Kengine::serialization::read(is, scale_start.x);
    size += Kengine::serialization::read(is, scale_start.y);
    size += Kengine::serialization::read(is, scale_end.x);
    size += Kengine::serialization::read(is, scale_end.y);

    return size;
}

std::size_t progress_bar_component::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(uv_size_start.x);
    size += Kengine::serialization::size(uv_size_start.y);
    size += Kengine::serialization::size(uv_size_end.x);
    size += Kengine::serialization::size(uv_size_end.y);
    size += Kengine::serialization::size(scale_start.x);
    size += Kengine::serialization::size(scale_start.y);
    size += Kengine::serialization::size(scale_end.x);
    size += Kengine::serialization::size(scale_end.y);

    return size;
}

bool progress_bar_component::imgui_editable_render()
{
    bool edited = false;

    ImGui::PushID(this);

    edited = edited ||
             ImGui::DragFloat2("Uv size start", (float*)&uv_size_start, 0.01f);

    edited =
        edited || ImGui::DragFloat2("Uv size end", (float*)&uv_size_end, 0.01f);

    edited =
        edited || ImGui::DragFloat2("Scale start", (float*)&scale_start, 0.01f);

    edited =
        edited || ImGui::DragFloat2("Scale end", (float*)&scale_end, 0.01f);

    ImGui::DragFloat("Progress", &progress, 0.01f);

    ImGui::PopID();

    return edited;
}