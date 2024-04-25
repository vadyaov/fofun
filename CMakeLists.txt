# cmake_minimum_required(VERSION 3.16)
# project(forfun LANGUAGES CXX)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# include(FetchContent)
# FetchContent_Declare(SFML
#     GIT_REPOSITORY https://github.com/SFML/SFML.git
#     GIT_TAG 2.6.x)
# FetchContent_MakeAvailable(SFML)

# add_executable(forfun src/main.cpp
#                       imgui/imgui.cpp
#                       imgui/imgui_draw.cpp
#                       imgui/imgui_tables.cpp
#                       imgui/imgui_widgets.cpp
# )

# target_link_libraries(forfun PRIVATE sfml-graphics)
# target_compile_features(forfun PRIVATE cxx_std_17)

# if(WIN32)
#     add_custom_command(
#         TARGET forfun
#         COMMENT "Copy OpenAL DLL"
#         PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:CMakeSFMLProject>
#         VERBATIM)
# endif()

# install(TARGETS forfun)


# This is an example of how to use FetchContent to easily
# manage 3rd-party dependencies
# First, check out src/CMakeLists.txt - it's really simple!
# Then, check out dependencies directory, it get a bit more complex
# there, but you'll see how it goes
#
# Options:
# * USE_SYSTEM_DEPS - try to find dependencies using find_package (OFF by default)
# * LINK_DEPS_STATIC - link to dependencies statically (ON by default)
#
cmake_minimum_required(VERSION 3.12)

project(imgui-sfml-example
  VERSION 1.0
  LANGUAGES CXX
)

add_subdirectory(dependencies)
add_subdirectory(src)
