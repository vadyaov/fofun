add_executable(fofun
  main.cpp
  Arena.cc
  ArenaManager.cc
  Collision.cc
  Gui.cc
)

# Yep, that's it!
target_link_libraries(fofun
  PUBLIC
    ImGui-SFML::ImGui-SFML
    sfml-audio
)

include(Install.cmake)
