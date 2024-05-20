#ifndef ARENA_H_
#define ARENA_H_

#include "imgui-SFML.h"

#include "ArenaManager.h"

class Arena : public sf::RenderWindow, public ArenaManager {
  public:
    Arena(sf::VideoMode mode);
    Arena(sf::VideoMode mode, int n);

    Arena(int width, int height);
    Arena(int width, int height, int n);

    void run();

  private:
    void pollEvents(sf::Event& event);
    void ImguiWindow();

    sf::Clock clock;
    bool pause = false;
};

#endif // ARENA_H_
