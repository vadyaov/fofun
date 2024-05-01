add_executable(fofun
  main.cpp
  Arena.cc
)

# Yep, that's it!
target_link_libraries(fofun
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
