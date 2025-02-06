/*! \file  parser_input.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:parser_input;

import std;

export namespace fabric {
  template <typename Input>
  struct parser_input_t {
    static constexpr bool implemented = false;
    static_assert(false, "parser_input_t not implemented.");

    parser_input_t() = delete;
    parser_input_t(const parser_input_t&) = delete;
    parser_input_t(parser_input_t&&) = delete;

    void create_restore_point() {}
    void restore() {}

    [[nodiscard]]
    char get_current_char() { return '\0'; }
    [[nodiscard]]
    std::size_t get_current_offset() { return 0UL; }

    void append_to(std::string& str, std::size_t count) {}

    [[nodiscard]]
    bool is_at_end() { return true; }

    void seek(long relative_offset) {}

    std::size_t get_size() const { return 0UL; }
  };

  template <typename Input>
  requires parser_input_t<Input>::implemented
  parser_input_t(Input input) -> parser_input_t<Input>;
}