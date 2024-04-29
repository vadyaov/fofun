#ifndef WALL_H_
#define WALL_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Wall : public sf::Drawable, public sf::Transformable {
  public:
    Wall(sf::Vector2f l, sf::Vector2f r, float width) {
      sf::CircleShape c1;
      c1.setRadius(0.5f * width);
      c1.setPosition(l);
      c1.setOrigin(0.5f * width, 0.5f * width);

      sf::CircleShape c2;
      c2.setRadius(0.5f * width);
      c2.setPosition(r);
      c2.setOrigin(0.5f * width, 0.5f * width);

      circles.emplace_back(c1); // will it work ?
      circles.emplace_back(c2); // will it work ?

      float length = sqrtf(powf(r.x - l.x, 2) + powf(r.y - l.y, 2));
      sf::RectangleShape rect(sf::Vector2f(length, width));
      rect.setFillColor(sf::Color::Green);

      rect.setPosition(c1.getPosition());

      float alpha = atanf((r.y - l.y) / (r.x - l.x)) * 180.0f / M_PI; // degrees
      rect.rotate(alpha);

      rectangle = std::move(rect);
    }

  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
      states.transform *= getTransform();

      target.draw(circles[0], states);
      target.draw(circles[1], states);
      target.draw(rectangle, states);
    }

    std::vector<sf::CircleShape> circles;
    sf::RectangleShape rectangle;
};

#endif // WALL_H_
