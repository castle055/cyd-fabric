list(APPEND CMAKE_PREFIX_PATH /usr/lib/cmake/clang/)
list(APPEND CMAKE_PREFIX_PATH /usr/lib/cmake/llvm/FindLibEdit.cmake)

list(APPEND LibEdit_INCLUDE_DIRS /usr/include/editline)
list(APPEND LibEdit_LIBRARIES /usr/lib/libedit.so)

find_package(Clang REQUIRED CONFIG)
if("${LLVM_VERSION_MAJOR}" VERSION_LESS 18)
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 18 or above")
endif()


add_library(ReflectorPlugin SHARED test_plugin.cpp ASTConsumer.h)
target_link_libraries(ReflectorPlugin PRIVATE
        clang
        LLVMSupport
)

target_include_directories(ReflectorPlugin PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_compile_options(ReflectorPlugin PRIVATE "-stdlib=libstdc++")

set(REFLECTOR_PLUGIN_BINARY "${CMAKE_CURRENT_BINARY_DIR}/libReflectorPlugin.so")

macro(reflect_target TARGET)
  add_dependencies(${TARGET} ReflectorPlugin)
  set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-fplugin=${REFLECTOR_PLUGIN_BINARY}")
endmacro(reflect_target)
