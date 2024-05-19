#ifndef WALL_H_
#define WALL_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Wall : public sf::Drawable, public sf::Transformable {
  public:
    Wall(sf::Vector2f l, sf::Vector2f r, float w, const sf::Color& color = sf::Color::Green) : width{w}, col{color} {
      sf::CircleShape c1(0.5f * width);
      c1.setPosition(l);
      c1.setOrigin(0.5f * width, 0.5f * width);
      c1.setFillColor(sf::Color(128u, 128u, 128u));

      sf::CircleShape c2(0.5f * width);
      c2.setPosition(r);
      c2.setOrigin(0.5f * width, 0.5f * width);
      c2.setFillColor(sf::Color(128u, 128u, 128u));

      circles.emplace_back(c1); // will it work ?
      circles.emplace_back(c2); // will it work ?

      buildRectangle();
    }

    sf::Vector2f getStartingPoint() const {
      return circles[0].getPosition();
    }

    void setStartingPoint(sf::Vector2f p) {
      circles[0].setPosition(p);
      buildRectangle();
    }

    sf::Vector2f getEndingPoint() const {
      return circles[1].getPosition();
    }

    void setEndingPoint(sf::Vector2f p) {
      circles[1].setPosition(p);
      buildRectangle();
    }

    sf::Vector2f getSize() const {
      return rectangle.getSize();
    }

    bool firstCornerContains(sf::Vector2f pos) {
      return circles[0].getGlobalBounds().contains(pos);
    }

    bool secondCornerContains(sf::Vector2f pos) {
      return circles[1].getGlobalBounds().contains(pos);
    }

    bool bodyContains(sf::Vector2f pos) {
      sf::Vector2f s = circles[0].getPosition();
      sf::Vector2f e = circles[1].getPosition();

      sf::Vector2f se(e.x - s.x, e.y - s.y);
      sf::Vector2f sp(pos.x - s.x, pos.y - s.y);

      float length = sqrtf(se.x * se.x + se.y * se.y);

      float projection = (se.x * sp.x + se.y * sp.y) / length;

      float t = std::max(0.0f, std::min(length, projection)) / length;

      sf::Vector2f tPos(s.x + se.x * t, s.y + se.y * t);

      float distance = sqrtf(powf(tPos.x - pos.x, 2) + powf(tPos.y - pos.y, 2));

      if (distance <= 0.5f * width) {
        return true;
      }
      
      return false;
    }

    void move(sf::Vector2f direction, float dist) {
      circles[0].move(direction.x * dist, direction.y * dist);
      circles[1].move(direction.x * dist, direction.y * dist);
      buildRectangle();
    }

    void changeColor(const sf::Color& color) {
      col = color;
      buildRectangle();
    }

    virtual ~Wall() = default;

  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
      states.transform *= getTransform();

      target.draw(circles[0], states);
      target.draw(circles[1], states);
      target.draw(rectangle, states);
    }

    void buildRectangle() {
      sf::Vector2f l = circles[0].getPosition();
      sf::Vector2f r = circles[1].getPosition();
      sf::Vector2f direction(r.x - l.x, r.y - l.y);

      float length = sqrtf(powf(direction.x, 2) + powf(direction.y, 2));
      sf::Vector2f normal(direction.y / length, -direction.x / length);

      sf::RectangleShape rect(sf::Vector2f(length, width));
      rect.setFillColor(col);
      rect.setPosition(circles[0].getPosition());

      if (r.x > l.x) {
        rect.rotate(atanf((r.y - l.y) / (r.x - l.x)) * 180.0f / M_PI);
      } else if (r.x < l.x) {
        rect.rotate((M_PI + atanf((r.y - l.y) / (r.x - l.x))) * 180.0f / M_PI);
      } else if (r.y > l.y) {
        rect.rotate(180.0f / 2.0f);
      } else {
        rect.rotate(-180.0f / 2.0f);
      }

      rect.move(0.5f * width * normal.x, 0.5f * width * normal.y);
      rectangle = std::move(rect);
    }

    std::vector<sf::CircleShape> circles;
    sf::RectangleShape rectangle;
    float width;
    sf::Color col;
};

#endif // WALL_H_
