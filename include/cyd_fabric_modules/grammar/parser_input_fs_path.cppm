/*! \file  parser_input_fs_path.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:parser_input_fs_path;

import std;
export import :parser_input;

export namespace fabric {
  template <>
  struct parser_input_t<std::filesystem::path> {
    static constexpr bool implemented = true;

    struct string_restore_point_t {
      std::size_t offset {0};
    };

    parser_input_t(const std::filesystem::path &file)
      : fd_(file) {
      fd_.seekg(0, std::ios::end);
      file_size_ = fd_.tellg();
      fd_.seekg(0, std::ios::beg);
    }

    [[nodiscard]]
    string_restore_point_t create_restore_point() {
      return {get_current_offset()};
    }

    void restore(const string_restore_point_t& restore_point) {
      fd_.seekg(restore_point.offset, std::ios::beg);
    }

    [[nodiscard]]
    char get_current_char() {
      return fd_.peek();
    }

    [[nodiscard]]
    std::size_t get_current_offset() {
      return fd_.tellg();
    }

    void append_to(std::string& str, std::size_t count) {
      std::size_t prev_size = str.size();
      str.resize(prev_size + count);
      fd_.read(&str[prev_size], count);
    }

    [[nodiscard]]
    bool is_at_end() {
      return fd_.tellg() == file_size_;
    }

    void seek(const long relative_offset) {
      if (relative_offset > 0) {
        fd_.seekg(relative_offset, std::ios::cur);
        if (fd_.tellg() > file_size_) {
          fd_.seekg(0, std::ios::end);
        }
      } else {
        if (-relative_offset > fd_.tellg()) {
          fd_.seekg(0, std::ios::beg);
        } else {
          fd_.seekg(relative_offset, std::ios::cur);
        }
      }
    }

    std::size_t get_size() const {
      return file_size_;
    }
  private:
    std::ifstream fd_;
    std::size_t file_size_;
  };
}