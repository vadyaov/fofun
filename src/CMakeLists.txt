add_executable(fofun
  main.cpp
  Arena.cc
  Collision.cc
  Gui.cc
)

# Yep, that's it!
target_link_libraries(fofun
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
