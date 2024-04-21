#ifndef FABRIC_HPP
#define FABRIC_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <memory>

int randInRange(int l, int r) {
  static std::random_device rd; // obtain a random number from hardware
  static std::mt19937 gen(rd()); // seed the generator
  static std::uniform_int_distribution<> distr(l, r); // define the range

  return distr(gen);
}

class ShapeFactory {
  public:
    virtual std::shared_ptr<sf::Drawable> createShape(int x, int y) = 0;
    virtual ~ShapeFactory() {}
};

class CircleFactory : public ShapeFactory {
  public:
    std::shared_ptr<sf::Drawable> createShape(int x, int y) override {
      std::shared_ptr<sf::CircleShape> circle(new sf::CircleShape);
      circle->setRadius(randInRange(15, 30));
      sf::Color circleColor;
      circleColor.r = randInRange(0, 255);
      circleColor.g = randInRange(0, 255);
      circleColor.b = randInRange(0, 255);

      circle->setFillColor(circleColor);
      circle->setOrigin(circle->getRadius(), circle->getRadius());
      circle->setPosition(x, y);

      return circle;
    }
};

class TriangleFactory : public ShapeFactory {
  public:
    std::shared_ptr<sf::Drawable> createShape(int x, int y) override {
      std::shared_ptr<sf::VertexArray> triangle(new sf::VertexArray(sf::Triangles, 3));
      (*triangle)[0].position = sf::Vector2f(x, y);
      (*triangle)[1].position = sf::Vector2f(x + randInRange(10, 40), y);
      (*triangle)[2].position = sf::Vector2f(x, y + randInRange(10, 40));

      (*triangle)[0].color = sf::Color::Red;
      (*triangle)[1].color = sf::Color::Blue;
      (*triangle)[2].color = sf::Color::Green;

      return triangle;
    }
};

class RectangleFactory : public ShapeFactory {
};

#endif // FABRIC_HPP
