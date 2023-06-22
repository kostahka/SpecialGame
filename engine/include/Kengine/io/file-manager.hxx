#pragma once

#include <iostream>
#include <memory>

namespace file_manager
{

struct membuf : public std::streambuf
{
    membuf();
    membuf(std::unique_ptr<char[]> buffer, size_t size);
    membuf(membuf&& other);

    pos_type seekoff(off_type               pos,
                     std::ios_base::seekdir seek_dir,
                     std::ios_base::openmode) override;

    char*  begin() const;
    size_t size() const;

private:
    std::unique_ptr<char[]> buf;
    size_t                  buf_size;
};

membuf load_file(std::string_view path);

} // namespace file_manager
