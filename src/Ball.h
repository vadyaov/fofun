#ifndef BALL_H_
#define BALL_H_

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

sf::Color getRandomColor();

class Ball : public sf::CircleShape {
  public:
    sf::Time fSimTimeRemaining; // if we occur with tunnel effect --> deltaT will be > 0
    sf::Vector2f prevVelo;
    sf::Vector2f prevPos;

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
      setFillColor(getRandomColor());

      // set mass
      mass = radius * 10.0f;
    }

    void update() {
      move(velocity.x * fSimTimeRemaining.asSeconds(), velocity.y * fSimTimeRemaining.asSeconds());
      velocity.y += acceleration.y * fSimTimeRemaining.asSeconds();
    }

    void setVelocity(const sf::Vector2f& v) {
      velocity = v;
    }

    void setAcceleration(const sf::Vector2f a) {
      acceleration = a;
    }

    sf::Vector2f getAcceleration() const { return acceleration; }

    float getMass() const {
      return mass;
    }

    void setMass(float m) {
      mass = m;
    }

    sf::Vector2f getVelocity() const { return velocity; }

  private:
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;
};

#endif // BALL_H_ 
