#pragma once

#include "box2d/box2d.h"
#include "draw-primitives.hxx"

struct b2AABB;
struct GLRenderPoints;
struct GLRenderLines;
struct GLRenderTriangles;
struct GLFWwindow;

class b2GLDraw : public b2Draw
{
public:
    b2GLDraw();
    ~b2GLDraw();

    void Create();
    void Destroy();

    void DrawPolygon(const b2Vec2*  vertices,
                     int32          vertexCount,
                     const b2Color& color) override;

    void DrawSolidPolygon(const b2Vec2*  vertices,
                          int32          vertexCount,
                          const b2Color& color) override;

    void DrawCircle(const b2Vec2&  center,
                    float          radius,
                    const b2Color& color) override;

    void DrawSolidCircle(const b2Vec2&  center,
                         float          radius,
                         const b2Vec2&  axis,
                         const b2Color& color) override;

    void DrawSegment(const b2Vec2&  p1,
                     const b2Vec2&  p2,
                     const b2Color& color) override;

    void DrawTransform(const b2Transform& xf) override;

    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

    void DrawAABB(b2AABB* aabb, const b2Color& color);

    void Draw();

private:
    Kengine::gl_render_primitive* r_points;
    Kengine::gl_render_primitive* r_lines;
    Kengine::gl_render_primitive* r_triangles;
};
