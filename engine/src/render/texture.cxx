#include "Kengine/render/texture.hxx"
#include "Kengine/io/file-manager.hxx"
#include "picopng.hxx"

#include "Kengine/transform3d.hxx"
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif
#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

namespace Kengine
{

class texture_impl : public texture_object
{
public:
    texture_impl(std::string texture_path)
    {
        /*
                std::vector<unsigned char> png_memory;

                std::ifstream png_file;

                png_file.exceptions(std::ifstream::failbit);

                try
                {
                    png_file.open(texture_path, std::ios_base::binary);

                    png_file.seekg(0, std::ios_base::end);
                    auto png_file_end = png_file.tellg();
                    png_file.seekg(0, std::ios_base::beg);

                    png_memory.resize(static_cast<size_t>(png_file_end));

                    png_file.read(reinterpret_cast<char*>(png_memory.data()),
                                  png_file_end);

                    png_file.close();
                }
                catch (std::ifstream::failure e)
                {
                    std::cerr << "Failed to load texture from png file ["
                              << texture_path << "]. Error: " << e.what() <<
           std::endl;
                }

                // loadFile(png_memory, texture_path);
        */
        file_manager::membuf texture_file =
            file_manager::load_file(texture_path);

        std::vector<unsigned char> image_memory;
        unsigned long              image_width  = 0;
        unsigned long              image_height = 0;

        if (decodePNG(image_memory,
                      image_width,
                      image_height,
                      reinterpret_cast<unsigned char*>(texture_file.begin()),
                      texture_file.size(),
                      true))
        {
            std::cerr << "Failed to decode png texture" << std::endl;
        }
        size = { static_cast<int>(image_width),
                 static_cast<int>(image_height) };

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     static_cast<GLsizei>(image_width),
                     static_cast<GLsizei>(image_height),
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     &image_memory[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    };

    void bind() override { glBindTexture(GL_TEXTURE_2D, texture); };

    [[nodiscard]] itransform2d get_size() const override { return size; };
    [[nodiscard]] intptr_t     get_id() const override { return texture; };

    ~texture_impl() override { glDeleteTextures(1, &texture); };

private:
    itransform2d size;
    GLuint       texture;
};

texture_object* create_texture(std::string texture_path)
{
    return new texture_impl(texture_path);
}
} // namespace Kengine
