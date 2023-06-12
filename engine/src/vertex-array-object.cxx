#include "vertex-array-object.hxx"

#include "glad/glad.h"
#include "opengl-error.hxx"
#include "transform3d.hxx"
#include "vertex.hxx"

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

class vertex_array_object_impl : public vertex_array_object
{
public:
    vertex_array_object_impl(const vertex_text2d_array& vertices)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertex_text2d),
                     vertices.data(),
                     GL_DYNAMIC_DRAW);

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

    vertex_array_object_impl(const vertex_color_size* vertices,
                             const size_t             v_count)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     v_count * sizeof(vertex_color_size),
                     vertices,
                     GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_color_size),
                              reinterpret_cast<GLvoid*>(0));
        glVertexAttribPointer(1,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_color_size),
                              reinterpret_cast<GLvoid*>(sizeof(transform3d)));
        glVertexAttribPointer(
            2,
            1,
            GL_FLOAT,
            GL_FALSE,
            sizeof(vertex_color_size),
            reinterpret_cast<GLvoid*>(sizeof(transform3d) + sizeof(color)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    };

    vertex_array_object_impl(const vertex_color* vertices, const size_t v_count)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     v_count * sizeof(vertex_color),
                     vertices,
                     GL_DYNAMIC_DRAW);

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

    vertex_array_object_impl(const transform2d* vertices,
                             const size_t       v_count,
                             const uint32_t*    indexes,
                             const size_t       i_count)
    {

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     v_count * sizeof(transform2d),
                     vertices,
                     GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(transform2d),
                              reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     i_count * sizeof(uint32_t),
                     indexes,
                     GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    };

    virtual ~vertex_array_object_impl() override
    {
        if (VAO)
        {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO)
        {
            glDeleteBuffers(1, &VBO);
        }
        if (EBO)
        {
            glDeleteBuffers(1, &EBO);
        }
    };

    void draw_triangles(int count) override
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, count * 3);
        glBindVertexArray(0);
    }

    void draw_triangles_elements(int count) override
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

    void draw_triangles_elements(int count, size_t offset) override
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,
                       count,
                       GL_UNSIGNED_INT,
                       reinterpret_cast<void*>(offset));
        glBindVertexArray(0);
    };

    void draw_points(int count) override
    {
        glBindVertexArray(VAO);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, count);
        glDisable(GL_PROGRAM_POINT_SIZE);
        glBindVertexArray(0);
    }

    void draw_lines(int count) override
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, count);
        glBindVertexArray(0);
    }

    void set_vertices(void* data, size_t offset, size_t size) override
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        glBindVertexArray(0);
    };

    void set_indexes(uint32_t* data, size_t offset, size_t size) override
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
        glBindVertexArray(0);
    };

private:
    GLuint VBO{ 0 };
    GLuint VAO{ 0 };
    GLuint EBO{ 0 };
};

vertex_array_object* create_vao(const vertex_text2d_array& vertices)
{
    return new vertex_array_object_impl(vertices);
};

vertex_array_object* create_vao(const transform2d* vertices,
                                const size_t       v_count,
                                const uint32_t*    indexes,
                                const size_t       i_count)
{
    return new vertex_array_object_impl(vertices, v_count, indexes, i_count);
}

vertex_array_object* create_vao(const vertex_color_size* vertices,
                                const size_t             v_count)
{
    return new vertex_array_object_impl(vertices, v_count);
};

vertex_array_object* create_vao(const vertex_color* vertices,
                                const size_t        v_count)
{
    return new vertex_array_object_impl(vertices, v_count);
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
