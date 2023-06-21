#include "Kengine/render/b2GLDraw.hxx"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

inline Kengine::transform3d vec_to_tr(const b2Vec2 v)
{
    return { v.x, v.y, 5 };
}

//
b2GLDraw::b2GLDraw()
    : r_points(nullptr)
    , r_lines(nullptr)
    , r_triangles(nullptr)
{
}

//
b2GLDraw::~b2GLDraw()
{
    r_points    = nullptr;
    r_lines     = nullptr;
    r_triangles = nullptr;
}

//
void b2GLDraw::Create()
{
    r_points =
        Kengine::create_primitive_render(Kengine::primitive_type::points);
    r_points->create();
    r_lines = Kengine::create_primitive_render(Kengine::primitive_type::lines);
    r_lines->create();
    r_triangles =
        Kengine::create_primitive_render(Kengine::primitive_type::triangles);
    r_triangles->create();
}

//
void b2GLDraw::Destroy()
{
    r_points->destroy();
    delete r_points;
    r_points = nullptr;

    r_lines->destroy();
    delete r_lines;
    r_lines = nullptr;

    r_triangles->destroy();
    delete r_triangles;
    r_triangles = nullptr;
}

//
void b2GLDraw::DrawPolygon(const b2Vec2*  vertices,
                           int32          vertexCount,
                           const b2Color& color)
{
    b2Vec2 p1 = vertices[vertexCount - 1];
    for (int32 i = 0; i < vertexCount; ++i)
    {
        b2Vec2 p2 = vertices[i];
        r_lines->vertex(vec_to_tr(p1), { color.r, color.g, color.b, color.a });
        r_lines->vertex(vec_to_tr(p2), { color.r, color.g, color.b, color.a });
        p1 = p2;
    }
}

//
void b2GLDraw::DrawSolidPolygon(const b2Vec2*  vertices,
                                int32          vertexCount,
                                const b2Color& color)
{
    Kengine::color fillColor{
        0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f
    };

    const b2Vec2& v1 = vertices[0];
    for (int32 i = 1; i < vertexCount - 1; ++i)
    {
        const b2Vec2& v2 = vertices[i];
        const b2Vec2& v3 = vertices[i + 1];

        r_triangles->vertex(vec_to_tr(v1), fillColor);
        r_triangles->vertex(vec_to_tr(v2), fillColor);
        r_triangles->vertex(vec_to_tr(v3), fillColor);
    }

    b2Vec2 p1 = vertices[vertexCount - 1];
    for (int32 i = 0; i < vertexCount; ++i)
    {
        b2Vec2 p2 = vertices[i];
        r_lines->vertex(vec_to_tr(p1), { color.r, color.g, color.b, color.a });
        r_lines->vertex(vec_to_tr(p2), { color.r, color.g, color.b, color.a });
        p1 = p2;
    }
}

//
void b2GLDraw::DrawCircle(const b2Vec2&  center,
                          float          radius,
                          const b2Color& color)
{
    const float k_segments  = 16.0f;
    const float k_increment = 2.0f * b2_pi / k_segments;
    float       sinInc      = sinf(k_increment);
    float       cosInc      = cosf(k_increment);
    b2Vec2      r1(1.0f, 0.0f);
    b2Vec2      v1 = center + radius * r1;
    for (int32 i = 0; i < k_segments; ++i)
    {
        // Perform rotation to avoid additional trigonometry.
        b2Vec2 r2;
        r2.x      = cosInc * r1.x - sinInc * r1.y;
        r2.y      = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        r_lines->vertex(vec_to_tr(v1), { color.r, color.g, color.b, color.a });
        r_lines->vertex(vec_to_tr(v2), { color.r, color.g, color.b, color.a });
        r1 = r2;
        v1 = v2;
    }
}

//
void b2GLDraw::DrawSolidCircle(const b2Vec2&  center,
                               float          radius,
                               const b2Vec2&  axis,
                               const b2Color& color)
{
    const float    k_segments  = 16.0f;
    const float    k_increment = 2.0f * b2_pi / k_segments;
    float          sinInc      = sinf(k_increment);
    float          cosInc      = cosf(k_increment);
    b2Vec2         v0          = center;
    b2Vec2         r1(cosInc, sinInc);
    b2Vec2         v1 = center + radius * r1;
    Kengine::color fillColor{
        0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f
    };
    for (int32 i = 0; i < k_segments; ++i)
    {
        // Perform rotation to avoid additional trigonometry.
        b2Vec2 r2;
        r2.x      = cosInc * r1.x - sinInc * r1.y;
        r2.y      = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        r_triangles->vertex(vec_to_tr(v0), fillColor);
        r_triangles->vertex(vec_to_tr(v1), fillColor);
        r_triangles->vertex(vec_to_tr(v2), fillColor);
        r1 = r2;
        v1 = v2;
    }

    r1.Set(1.0f, 0.0f);
    v1 = center + radius * r1;
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 r2;
        r2.x      = cosInc * r1.x - sinInc * r1.y;
        r2.y      = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        r_lines->vertex(vec_to_tr(v1), { color.r, color.g, color.b, color.a });
        r_lines->vertex(vec_to_tr(v2), { color.r, color.g, color.b, color.a });
        r1 = r2;
        v1 = v2;
    }

    // Draw a line fixed in the circle to animate rotation.
    b2Vec2 p = center + radius * axis;
    r_lines->vertex({ center.x, center.y, 0 },
                    { color.r, color.g, color.b, color.a });
    r_lines->vertex(vec_to_tr(p), { color.r, color.g, color.b, color.a });
}

//
void b2GLDraw::DrawSegment(const b2Vec2&  p1,
                           const b2Vec2&  p2,
                           const b2Color& color)
{
    r_lines->vertex(vec_to_tr(p1), { color.r, color.g, color.b, color.a });
    r_lines->vertex(vec_to_tr(p2), { color.r, color.g, color.b, color.a });
}

//
void b2GLDraw::DrawTransform(const b2Transform& xf)
{
    const float    k_axisScale = 0.4f;
    Kengine::color red{ 1.0f, 0.0f, 0.0f };
    Kengine::color green{ 0.0f, 1.0f, 0.0f };
    b2Vec2         p1 = xf.p, p2;

    r_lines->vertex(vec_to_tr(p1), red);
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    r_lines->vertex(vec_to_tr(p2), red);

    r_lines->vertex(vec_to_tr(p1), green);
    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    r_lines->vertex(vec_to_tr(p2), green);
}

//
void b2GLDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    r_points->vertex(
        vec_to_tr(p), { color.r, color.g, color.b, color.a }, size);
}

//
void b2GLDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
    b2Vec2 p1 = aabb->lowerBound;
    b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
    b2Vec2 p3 = aabb->upperBound;
    b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);

    r_lines->vertex(vec_to_tr(p1), { c.r, c.g, c.b, c.a });
    r_lines->vertex(vec_to_tr(p2), { c.r, c.g, c.b, c.a });

    r_lines->vertex(vec_to_tr(p2), { c.r, c.g, c.b, c.a });
    r_lines->vertex(vec_to_tr(p3), { c.r, c.g, c.b, c.a });

    r_lines->vertex(vec_to_tr(p3), { c.r, c.g, c.b, c.a });
    r_lines->vertex(vec_to_tr(p4), { c.r, c.g, c.b, c.a });

    r_lines->vertex(vec_to_tr(p4), { c.r, c.g, c.b, c.a });
    r_lines->vertex(vec_to_tr(p1), { c.r, c.g, c.b, c.a });
}
void b2GLDraw::Draw()
{
    r_lines->draw();
    r_points->draw();
    r_triangles->draw();
}
