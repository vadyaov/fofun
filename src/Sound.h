#pragma once

#include <filesystem>
#include <vector>
#include <iostream>

namespace fs = std::filesystem;

class Sound {
  public:
    Sound(fs::path directory) {
      for (auto const& dir_entry : std::filesystem::directory_iterator{directory}) {
          std::cout << dir_entry.path() << '\n';
          sounds.push_back(dir_entry.path());
      }
    }

    std::vector<fs::path>& getSounds() {
      return sounds;
    }

  private:
    std::vector<fs::path> sounds;
};
