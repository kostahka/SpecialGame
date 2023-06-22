#include "Kengine/io/file-manager.hxx"

#include "SDL3/SDL_filesystem.h"
#include <SDL_error.h>
#include <SDL_rwops.h>
#include <iostream>

namespace file_manager
{
membuf::membuf()
    : std::streambuf()
    , buf()
    , buf_size(0)
{
}
membuf::membuf(std::unique_ptr<char[]> buffer, size_t size)
    : std::streambuf()
    , buf(std::move(buffer))
    , buf_size(size)
{
    char* beg_ptr = buf.get();
    char* end_ptr = beg_ptr + buf_size;
    setg(beg_ptr, beg_ptr, end_ptr);
    setp(beg_ptr, end_ptr);
}
membuf::membuf(membuf&& other)
{
    setp(nullptr, nullptr);
    setg(nullptr, nullptr, nullptr);

    other.swap(*this);

    buf      = std::move(other.buf);
    buf_size = other.buf_size;

    other.buf_size = 0;
}

membuf::pos_type membuf::seekoff(off_type               pos,
                                 std::ios_base::seekdir seek_dir,
                                 std::ios_base::openmode)
{
    if (seek_dir == std::ios_base::beg)
    {
        return 0 + pos;
    }
    else if (seek_dir == std::ios_base::end)
    {
        return buf_size + pos;
    }
    else
    {
        return egptr() - gptr();
    }
}

char* membuf::begin() const
{
    return eback();
}
size_t membuf::size() const
{
    return buf_size;
}

membuf load_file(std::string_view path)
{
    SDL_RWops* io = SDL_RWFromFile(path.data(), "rb");
    if (nullptr == io)
    {
        std::cerr << "Failed to load file [" << path
                  << "]. Error: " << SDL_GetError() << std::endl;
        return {};
    }

    Sint64 file_size = io->size(io);
    if (-1 == file_size)
    {
        std::cerr << "Can't determine size of file [" << path << "]"
                  << std::endl;
        return {};
    }
    const size_t            size = static_cast<size_t>(file_size);
    std::unique_ptr<char[]> mem  = std::make_unique<char[]>(size);

    const size_t num_readed_objects = io->read(io, mem.get(), size);
    if (num_readed_objects != size)
    {
        std::cerr << "Can't read all content from file [" << path << "]"
                  << std::endl;
        return {};
    }

    if (0 != io->close(io))
    {
        std::cerr << "Failed to close file [" << path << "]" << std::endl;
        return {};
    }
    return membuf(std::move(mem), size);
};
} // namespace file_manager
