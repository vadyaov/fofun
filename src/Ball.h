#ifndef BALL_H_
#define BALL_H_

#include <SFML/Graphics.hpp>
#include <cmath>

class Ball : public sf::CircleShape {
  public:
    Ball(const sf::Vector2f& pos) {
      // set random radius
      float radius = std::rand() % 10 + 10;
      setRadius(radius);

      // set origin to the center of ball
      setOrigin(sf::Vector2f(radius, radius));

      // set position
      setPosition(pos);

      // set random velocity and direction
      float angle = (std::rand() % 360) * 3.14f / 180.f;
      float speed = (std::rand() % 100) + 50.f;
      // vector(vx, vy)
      velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);

      // set random color
      std::vector<sf::Color> colors = {sf::Color::Blue, sf::Color::Cyan, sf::Color::Magenta,
                                       sf::Color::Yellow, sf::Color::Green, sf::Color::Red,
                                        sf::Color::White};
      setFillColor(colors[std::rand() % colors.size()]);

      // set mass
      mass = radius * 10.0f;
    }

    void update(sf::Time elapsed) {
      move(velocity.x * elapsed.asSeconds(), velocity.y * elapsed.asSeconds());
    }

    void setVelocity(const sf::Vector2f& v) {
      velocity = v;
    }

    float getMass() const {
      return mass;
    }

    void setMass(float m) {
      mass = m;
    }

    sf::Vector2f getVelocity() const { return velocity; }

  private:
    sf::Vector2f velocity;
    float mass;
};

#endif // BALL_H_ 
