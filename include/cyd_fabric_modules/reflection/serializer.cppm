// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  pretty_print.cppm
 *! \brief
 *!
 */

export module reflect:serializer;

import std;

export import fabric.ts.packs;

export import :types;
export import :accessors;

export namespace refl {
  using namespace fabric::ts::packs;

  namespace formats{
    struct reflected {
      struct args_t {
        bool         pretty = true;
        unsigned int indent = 2;
      } args;

      explicit reflected(args_t args_): args(args_) {}

      template <typename O, refl::Reflected R, std::size_t I>
      void print_obj_field_impl(O& out, const R& obj, std::size_t indent) const {
        using f = field<R, I>;

        for (std::size_t i = 0; i < std::max((indent * args.indent) - 2UL, 0UL); ++i) {
          out << " ";
        }

        switch (f::access) {
          case field_access::NONE:
            out << "- ";
            break;
          case field_access::PRIVATE:
            out << "🔒";
            break;
            //🔓
          case field_access::PROTECTED:
            out << "\\ ";
            break;
          case field_access::PUBLIC:
            out << "  ";
            break;
        }

        out << f::name;
        if constexpr (not refl::Reflected<typename f::type> or f::access != field_access::PUBLIC or std::is_reference_v<typename f::type>) {
          out << " [" << type_name<typename f::type> << "]";
        }

        if constexpr (f::access == field_access::PUBLIC) {
          if constexpr (std::is_reference_v<typename f::type>) {
            const auto&       it   = f::from_instance(obj);
            out << " {";
            out << std::format("0x{:X}", (std::size_t)&it);
            out << "}";

            if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {
              out << ": ";

              print_any(out, it, indent);
            }
          } else {
            if constexpr (refl::Reflected<typename f::type>) {
              out << ": ";
            } else {
              out << " {";
            }
            const auto& it = f::from_instance(obj);
            // const typename f::type& it =
            //   *(const typename f::type*)(((unsigned char*)&obj) + f::offset);
            print_any(out, it, indent);
            if constexpr (not refl::Reflected<typename f::type>) {
              out << "}";
            }
          }
        }

        out << std::endl;
      }

      template <typename O, refl::Reflected R, std::size_t... I>
      void
      print_obj_impl(O& out, const R& obj, std::size_t indent, std::index_sequence<I...>) const {
        out << type_name<R> << " {" << "\n";
        ((print_obj_field_impl<O, R, I>(out, obj, indent + 1)), ...);
        for (std::size_t i = 0; i < indent; ++i) {
          for (std::size_t j = 0; j < args.indent; ++j) {
            out << " ";
          }
        }
        out << "}";
      }

      template <typename O, refl::Reflected R>
      void print_obj(O& out, const R& obj, std::size_t indent = 0) const {
        print_obj_impl(out, obj, indent, std::make_index_sequence<field_count<R>>());
      }

      template <typename O, typename T>
      void print_any(O& out, const T& it, std::size_t indent) const {
        if constexpr (refl::Reflected<T>) {
          print_obj(out, it, indent);
        } else if constexpr (std::is_convertible_v<T, std::string>) {
          out << std::format("{:?}", it);
        } else if constexpr (std::same_as<T, char>) {
          out << std::format("0x{:X} '{}'", (int)it, it);
        } else if constexpr (std::formattable<T, char>) {
          out << std::format("{}", it);
        }
      }

      template <typename O, refl::Reflected R>
      void operator()(O& out, const R& obj) const {
        print_obj(out, obj);
        out << "\n";
      }
    };
  }

  template <refl::Reflected R, typename Format>
  struct serializer {
    using args_t = typename Format::args_t;

    static std::string to_string(const R& obj, const args_t& args = {}) {
      std::stringstream str{};
      auto              format = Format{args};
      format(str, obj);
      return str.str();
    }

    template <typename O>
    static void to_stream(O& out, const R& obj, const args_t& args = {}) {
      auto format = Format{args};
      format(out, obj);
    }
  };
} // namespace refl
