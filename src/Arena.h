#ifndef ARENA_H_
#define ARENA_H_

#include "imgui-SFML.h"
#include "imgui.h"

#include "ArenaManager.h"

class Arena : public sf::RenderWindow, public ArenaManager {
  public:
    // creating scene with existing videmode (for full screen)
    Arena(sf::VideoMode mode);
    Arena(sf::VideoMode mode, int n);

    // creating scene with screen input params
    Arena(int width, int height);
    Arena(int width, int height, int n);

    // main loop
    void run();

  private:
    void pollEvents(sf::Event& event);   // event processing
    void ImguiWindow();

    sf::Clock clock;
    bool pause = false;
};

#endif // ARENA_H_
