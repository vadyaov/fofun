#ifndef BORDER_H_
#define BORDER_H_

#include "Wall.h"

#include <SFML/Graphics.hpp>

class LeftBorder : public Wall {
  public:
    LeftBorder(sf::Vector2u size) : Wall(sf::Vector2f(-1.0f, 0.0f), sf::Vector2f(-1.0f, size.y), 2.0f) {}
};

class RightBorder : public Wall {
  public:
    RightBorder(sf::Vector2u size) : Wall(sf::Vector2f(size.x + 1.0f, 0.0f), sf::Vector2f(size.x + 1.0f, size.y), 2.0f) {}
};

class TopBorder : public Wall {
  public:
    TopBorder(sf::Vector2u size) : Wall(sf::Vector2f(0.0f, -1.0f), sf::Vector2f(size.x, -1.0f), 2.0f) {}
};

class BotBorder : public Wall {
  public:
    BotBorder(sf::Vector2u size) : Wall(sf::Vector2f(0.0f, size.y + 1.0f), sf::Vector2f(size.x, size.y + 1.0f), 2.0f) {}
};

#endif // BORDER_H_
