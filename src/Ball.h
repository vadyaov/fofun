#ifndef BALL_H_
#define BALL_H_

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

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
      setRandomColor();

      // set mass
      mass = radius * 10.0f;
    }

    void update(sf::Time elapsed) {
      move(velocity.x * elapsed.asSeconds(), velocity.y * elapsed.asSeconds());
      velocity.y += acceleration.y * elapsed.asSeconds();
    }

    void setVelocity(const sf::Vector2f& v) {
      velocity = v;
    }

    void setAcceleration(const sf::Vector2f a) {
      acceleration = a;
    }

    sf::Vector2f getacceleration() const { return acceleration; }

    float getMass() const {
      return mass;
    }

    void setMass(float m) {
      mass = m;
    }

    sf::Vector2f getVelocity() const { return velocity; }

    void setPreviousPosition(const sf::Vector2f& p) {
      previousPos = p;
    }

    sf::Vector2f getPreviousPosition() const {
      return previousPos;
    }

  private:
    void setRandomColor() {
      std::vector<sf::Color> colors = {sf::Color::Blue, sf::Color::Cyan, sf::Color::Magenta,
                                       sf::Color::Yellow, sf::Color::Green, sf::Color::Red,
                                        sf::Color::White};
      setFillColor(colors[std::rand() % colors.size()]);

    }

  private:
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;
    
    sf::Vector2f previousPos;
};

#endif // BALL_H_ 
