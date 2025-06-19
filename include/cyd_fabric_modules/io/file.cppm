// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  io_tasks.cppm
 *! \brief
 *!
 */

export module fabric.io.file;

import std;
import reflect;
import reflect.serialize;

import fabric.logging;
export import fabric.io.tasks;

export namespace fabric::io {

  struct data_t {
    std::filesystem::path path;
    file_decriptor        fd;
    std::size_t           current_offset;
  };

  class file: fabric::tasks::async_lifetime<file, data_t> {
    friend fabric::tasks::async_lifetime<file, data_t>;

    file() = default;

    io_task<> constructor(const std::filesystem::path& path) {
      auto fd_res = co_await io::open(path);
      if (fd_res.has_value()) {
        co_await init_data(path, fd_res.value(), 0UL);
        co_return {};
      } else {
        co_return fd_res.map_value<void>();
      }
    }

    static io_task<> destructor(const data_t& data) {
      co_return co_await io::close(data.fd);
    }

  public:
    static io_task<file> open(const std::filesystem::path& path) {
      co_return co_await async_lifetime::make(path);
    }

    io_task<> close() {
      co_return co_await self_destruct();
    }

    io_task<> seek(std::size_t offset) {
      data->current_offset = offset;
      co_return {};
    }

    io_task<> reset() {
      data->current_offset = 0UL;
      co_return {};
    }

    io_task<> seek_relative(int offset) {
      data->current_offset += offset;
      co_return {};
    }

    io_task<int> read(void* buffer, unsigned int size, std::size_t offset) {
      co_return co_await io::read(data->fd, buffer, size, offset);
    }

    io_task<int> read(std::string& str, unsigned int size, std::size_t offset) {
      str.resize(size);
      auto read_res = (co_await read(str.data(), size, offset));
      if (read_res.ok()) {
        int bytes_read = read_res.value();
        if (bytes_read != size) {
          str.resize(bytes_read);
        }
        co_return bytes_read;
      }
      co_return read_res.error();
    }

    io_task<int> read(void* buffer, unsigned int size) {
      auto read_res = co_await io::read(data->fd, buffer, size, data->current_offset);
      if (read_res.ok()) {
        int bytes_read        = read_res.value();
        data->current_offset += bytes_read;
        co_return bytes_read;
      }
      co_return read_res.error();
    }

    io_task<int> read(std::string& str, unsigned int size) {
      auto res =  co_await read(str, size, data->current_offset);
      if (res.ok()) {
        data->current_offset += res.value();
      }
      co_return res;
    }

    io_task<bool> get_line(std::string& str) {
      static constexpr std::size_t buf_size = 128;

      str.clear();
      std::string buf{};

      std::size_t offset = data->current_offset;

      int bytes_read;
      do {
        // read some bytes
        auto read_res = co_await read(buf, buf_size, offset);
        if (not read_res.ok()) {
          co_return read_res.error();
        }
        bytes_read = read_res.value();


        std::size_t newline_index = buf.find_first_of('\n');
        if (newline_index != std::string::npos) {
          str.append(buf.c_str(), newline_index);
          offset += newline_index + 1;
          break;
        } else {
          str.append(buf.c_str(), bytes_read);
          offset += bytes_read;
        }
      } while (bytes_read == buf_size);
      data->current_offset = offset;

      co_return bytes_read != 0;
    }
  };
} // namespace fabric::io
