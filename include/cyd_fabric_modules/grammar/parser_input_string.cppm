/*! \file  parser_input_string.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:parser_input_string;

import std;
export import :parser_input;

export namespace fabric {
  template <>
  struct parser_input_t<std::string> {
    static constexpr bool implemented = true;

    struct string_restore_point_t {
      std::size_t offset {0};
    };

    parser_input_t(const std::string &str)
      : str_(str),
        data_(str_.c_str()),
        size_(str_.size()),
        offset_(0) {
    }

    [[nodiscard]]
    inline string_restore_point_t create_restore_point() const {
      return {offset_};
    }

    inline void restore(const string_restore_point_t& restore_point) {
      offset_ = restore_point.offset;
    }

    [[nodiscard]]
    inline char get_current_char() const {
      return data_[offset_];
    }

    [[nodiscard]]
    inline std::size_t get_current_offset() const {
      return offset_;
    }

    void append_to(std::string& str, std::size_t count) {
      count                       = std::min(size_ - offset_, count);
      const std::size_t prev_size = str.size();
      str.resize(prev_size + count);
      std::memcpy(&str[prev_size], &data_[offset_], count);
      offset_ += count;
    }

    [[nodiscard]]
    inline bool is_at_end() const {
      return offset_ >= size_;
    }

    inline void seek(const long relative_offset) {
      if (relative_offset > 0) {
        offset_ += relative_offset;
        // if (offset_ > size_) {
          // offset_ = size_;
        // }
      } else {
        if (-relative_offset > offset_) {
          offset_ = 0;
        } else {
          offset_ -= relative_offset;
        }
      }
    }

    inline std::size_t get_size() const {
      return size_;
    }
  private:
    const std::string& str_;
    const char* data_;
    std::size_t size_;
    std::size_t offset_;
  };
}