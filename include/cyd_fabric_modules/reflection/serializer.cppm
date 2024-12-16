// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  pretty_print.cppm
 *! \brief
 *!
 */

export module reflect:serializer;

import std;

export import packtl;

export import :types;
export import :accessors;

export namespace refl {
  using namespace packtl;

  namespace formats {
    struct reflected {
      struct args_t {
        bool         pretty = true;
        unsigned int indent = 2;
      } args;

      explicit reflected(args_t args_): args(args_) {}

      template <typename O, refl::Reflected R, std::size_t I>
      void print_obj_field_impl(O& out, const R& obj, std::size_t indent) {
        using f = field<R, I>;

        for (std::size_t i = 0; i < std::max((indent * args.indent), 0UL); ++i) {
          out << " ";
        }

        switch (f::access) {
          case field_access::NONE:
            out << " -";
            break;
          case field_access::PRIVATE:
            // out << "🔒";
            out << "🔒";
            break;
            //🔓
          case field_access::PROTECTED:
            out << " \\";
            break;
          case field_access::PUBLIC:
            out << "  ";
            break;
        }

        out << f::name;
        // using raw_field_type = std::remove_pointer_t<std::remove_reference_t<typename f::type>>;
        // if constexpr (not refl::Reflected<typename f::type>) {
          // out << " [" << type_name<typename f::type> << "]";
          out << ": " << type_name<typename f::type> << " = ";
        // }

        // if constexpr (f::access == field_access::PUBLIC) {
          if constexpr (std::is_reference_v<typename f::type>) {
            const auto&       it   = f::from_instance(obj);
            out << " {";
            out << std::format("0x{:X}", (std::size_t)&it);
            out << "} ";

            // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {

              print_any(out, it, indent);
            // }
          } else if constexpr (std::is_pointer_v<typename f::type>) {
            const auto* it = f::from_instance(obj);
            out << " {";
            out << std::format("0x{:X}", (std::size_t)it);
            out << "} ";

            // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {
            if constexpr (not std::is_void_v<std::remove_pointer_t<typename f::type>>) {
              if (it != nullptr) {
                print_any(out, *it, indent);
              }
            }
          } else {
            if constexpr (not refl::Reflected<typename f::type>) {
              out << " { ";
            }
            const auto& it = f::from_instance(obj);
            // const typename f::type& it =
            //   *(const typename f::type*)(((unsigned char*)&obj) + f::offset);
            print_any(out, it, indent);
            if constexpr (not refl::Reflected<typename f::type>) {
              out << " }";
            }
          }
        // }

        out << std::endl;
      }

      template <typename O, refl::Reflected R, std::size_t... I>
      void print_obj_impl(O& out, const R& obj, std::size_t indent, std::index_sequence<I...>) {
        out << type_name<R> << " {" << "\n";
        ((print_obj_field_impl<O, R, I>(out, obj, indent + 1)), ...);
        for (std::size_t i = 0; i < indent; ++i) {
          for (std::size_t j = 0; j < args.indent; ++j) {
            out << " ";
          }
        }
        out << "}";
      }

      template <typename O, refl::Reflected R, std::size_t I>
      void print_obj_method(O& out, const R& obj) {
        using m = method<R, I>;

        switch (m::access) {
          case field_access::NONE:
            out << " -";
            break;
          case field_access::PRIVATE:
            // out << "🔒";
            out << "🔒";
            break;
            //🔓
          case field_access::PROTECTED:
            out << " \\";
            break;
          case field_access::PUBLIC:
            out << "  ";
            break;
        }

        out << m::name;
        out << ": " << type_name<typename m::type>;
      }

      template <typename O, refl::Reflected R>
      void print_obj(O& out, const R& obj, std::size_t indent = 0) {
        print_obj_impl(out, obj, indent, std::make_index_sequence<field_count<R>>());

        [&]<std::size_t ...I>(std::index_sequence<I...>) {
          (print_obj_method<O, R, I>(out, obj), ...);
        }(std::make_index_sequence<method_count<R>>());
      }

      template <typename O, typename T>
      void print_std_iterable(O& out, const T& it, std::size_t indent) {
        using item_type = typename T::value_type;
        out << "[" << std::endl;
        for (auto item = it.begin(); item != it.end(); ++item) {
          for (std::size_t i = 0; i < std::max((indent * args.indent), 0UL); ++i) {
            out << " ";
          }

          if constexpr (std::is_reference_v<item_type>) {
            const auto& value = *item;
            out << " {";
            out << std::format("0x{:X}", (std::size_t)&value);
            out << "} ";

            // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {

            print_any(out, value, indent + 1);
            // }
          } else if constexpr (std::is_pointer_v<item_type>) {
            const auto* value = *item;
            out << " {";
            out << std::format("0x{:X}", (std::size_t)value);
            out << "} ";

            // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {
            if constexpr (not std::is_void_v<std::remove_pointer_t<item_type>>) {
              if (value != nullptr) {
                print_any(out, *value, indent + 1);
              }
            }
          } else {
            if constexpr (refl::Reflected<item_type>) {
              out << "  ";
            } else {
              out << " {";
            }
            const auto& value = *item;
            // const typename f::type& it =
            //   *(const typename f::type*)(((unsigned char*)&obj) + f::offset);
            print_any(out, value, indent + 1);
            if constexpr (not refl::Reflected<item_type>) {
              out << " }";
            }
          }
          out << "," << std::endl;
        }
        for (std::size_t i = 0; i < std::max((indent * args.indent), 0UL); ++i) {
          out << " ";
        }
        out << "]";
      }

      template <typename O, typename T>
      void print_std_pair(O& out, const T& it, std::size_t indent) {
        using item_type_1 = typename T::first_type;
        using item_type_2 = typename T::second_type;
        out << "[";

        if constexpr (std::is_reference_v<item_type_1>) {
          const auto& value = it.first;
          out << " {";
          out << std::format("0x{:X}", (std::size_t)&value);
          out << "} ";

          // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {

          print_any(out, value, indent + 1);
          // }
        } else if constexpr (std::is_pointer_v<item_type_1>) {
          const auto* value = it.first;
          out << " {";
          out << std::format("0x{:X}", (std::size_t)value);
          out << "} ";

          // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {
          if constexpr (not std::is_void_v<std::remove_pointer_t<item_type_1>>) {
            if (value != nullptr) {
              out << ": ";

              print_any(out, *value, indent + 1);
            }
          }
        } else {
          if constexpr (not refl::Reflected<item_type_1>) {
            out << "{";
          }
          const auto& value = it.first;
          // const typename f::type& it =
          //   *(const typename f::type*)(((unsigned char*)&obj) + f::offset);
          print_any(out, value, indent + 1);
          if constexpr (not refl::Reflected<item_type_1>) {
            out << "}";
          }
        }
        out << ", ";
        if constexpr (std::is_reference_v<item_type_2>) {
          const auto& value = it.second;
          out << " {";
          out << std::format("0x{:X}", (std::size_t)&value);
          out << "} ";

          // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {

          print_any(out, value, indent);
          // }
        } else if constexpr (std::is_pointer_v<item_type_2>) {
          const auto* value = it.second;
          out << " {";
          out << std::format("0x{:X}", (std::size_t)value);
          out << "} ";

          // if constexpr (refl::Reflected<std::remove_reference_t<typename f::type>>) {
          if constexpr (not std::is_void_v<std::remove_pointer_t<item_type_2>>) {
            if (value != nullptr) {
              print_any(out, *value, indent);
            }
          }
        } else {
          if constexpr (not refl::Reflected<item_type_2>) {
            out << "{";
          }
          const auto& value = it.second;
          // const typename f::type& it =
          //   *(const typename f::type*)(((unsigned char*)&obj) + f::offset);
          print_any(out, value, indent);
          if constexpr (not refl::Reflected<item_type_2>) {
            out << "}";
          }
        }
        out << "]";
      }

      template <typename O, typename T>
      void print_any(O& out, const T& it, std::size_t indent) {
        if constexpr (is_type<std::vector, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::list, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::deque, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::queue, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::stack, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::map, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::unordered_map, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::set, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::unordered_set, T>::value) {
          print_std_iterable(out, it, indent);
        } else if constexpr (is_type<std::pair, T>::value) {
          print_std_pair(out, it, indent);
        } else if constexpr (std::is_same_v<T, std::atomic_flag>) {
          out << (it.test() ? "SET" : "CLEAR");
        } else if constexpr (is_type<std::unique_ptr, T>::value) {
          const auto* value = it.get();
          out << " {";
          out << std::format("0x{:X}", (std::size_t)value);
          out << "} ";
          print_any(out, *value, indent);
        } else if constexpr (is_type<std::shared_ptr, T>::value) {
          const auto* value = it.get();
          out << "{";
          out << std::format("0x{:X}", (std::size_t)value);
          out << "} ";
          print_any(out, *value, indent);
        } else if constexpr (is_type<std::weak_ptr, T>::value) {
          const auto* value = it.get();
          out << " {";
          out << std::format("0x{:X}", (std::size_t)value);
          out << "}: ";
          print_any(out, *value, indent);
        } else if constexpr (refl::Reflected<T>) {
          if (visited_.contains((std::size_t)&it)) {
            out << "<circular reference>";
            return;
          }
          visited_.emplace((std::size_t)&it);
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
      void operator()(O& out, const R& obj) {
        print_obj(out, obj);
        out << "\n";
      }

    private:
      std::unordered_set<std::size_t> visited_{};
    };
  } // namespace formats

  template <refl::Reflected R, typename Format = formats::reflected>
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

  template <refl::Reflected R, typename Format = formats::reflected>
  std::string to_string(const R& obj, const typename Format::args_t& args = {}) {

  }
} // namespace refl
