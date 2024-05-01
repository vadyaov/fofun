#ifndef WALL_H_
#define WALL_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Wall : public sf::Drawable, public sf::Transformable {
  public:
    Wall(sf::Vector2f l, sf::Vector2f r, float width) {
      sf::CircleShape c1(0.5f * width);
      c1.setPosition(l);
      c1.setOrigin(0.5f * width, 0.5f * width);

      sf::CircleShape c2(0.5f * width);
      c2.setPosition(r);
      c2.setOrigin(0.5f * width, 0.5f * width);

      circles.emplace_back(c1); // will it work ?
      circles.emplace_back(c2); // will it work ?

      sf::Vector2f direction(r.x - l.x, r.y - l.y);

      float length = sqrtf(powf(direction.x, 2) + powf(direction.y, 2));
      sf::Vector2f normal(direction.y / length, -direction.x / length);

      sf::RectangleShape rect(sf::Vector2f(length, width));
      rect.setFillColor(sf::Color::Green);
      rect.setPosition(c1.getPosition());

      if (r.x > l.x) {
        rect.rotate(atanf((r.y - l.y) / (r.x - l.x)) * 180.0f / M_PI);
      } else {
        rect.rotate((M_PI + atanf((r.y - l.y) / (r.x - l.x))) * 180.0f / M_PI);
      }
      rect.move(0.5f * width * normal.x, 0.5f * width * normal.y);

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
