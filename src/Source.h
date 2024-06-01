#pragma once

#include "Ball.h"

#include <memory>
#include "imgui.h"

using ballPtr = std::shared_ptr<Ball>;
struct Creation;

class Source {
  public:
    Source(sf::Vector2f pos, float spawn) : srcPos{pos}, period{spawn} {
      clock.restart();
    }

    bool spawnBall() {
      if (clock.getElapsedTime().asSeconds() >= period) {
        clock.restart();
        return true;
      }

      return false;
    }

    sf::Vector2f getPos() const {
      return srcPos;
    }

  private:
    sf::Vector2f srcPos;
    sf::Clock clock;
    float period;
};
