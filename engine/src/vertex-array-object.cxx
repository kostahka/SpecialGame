#include "vertex-array-object.hxx"

#include "glad/glad.h"
#include "transform3d.hxx"
#include "vertex.hxx"
#include <iostream>
#include <ostream>

namespace Kengine
{

class static_vertex_array_object_impl : public static_vertex_array_object
{
public:
    static_vertex_array_object_impl(const vertex_array& vertices)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertex),
                     vertices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex),
                              reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    };

    static_vertex_array_object_impl(const vertex_color_array& vertices)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertex_color),
                     vertices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_color),
                              reinterpret_cast<GLvoid*>(0));
        glVertexAttribPointer(1,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_color),
                              reinterpret_cast<GLvoid*>(sizeof(transform3d)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    };

    static_vertex_array_object_impl(const vertex_text2d_array& vertices)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertex_text2d),
                     vertices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_text2d),
                              reinterpret_cast<GLvoid*>(0));
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_text2d),
                              reinterpret_cast<GLvoid*>(sizeof(transform3d)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    };

    void draw_triangles(int count) override
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, count * 3);
        glBindVertexArray(0);
    };

private:
    GLuint VBO;
    GLuint VAO;
};

static_vertex_array_object* create_static_vao(
    const vertex_color_array& vertices)
{
    return new static_vertex_array_object_impl(vertices);
};

static_vertex_array_object* create_static_vao(
    const vertex_text2d_array& vertices)
{
    return new static_vertex_array_object_impl(vertices);
};

static_vertex_array_object* create_static_vao(const vertex_array& vertices)
{
    return new static_vertex_array_object_impl(vertices);
};

} // namespace Kengine
