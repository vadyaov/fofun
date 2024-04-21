#ifndef ENTITY_H_
#define ENTITY_H_

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <memory>


class MovableCircle : public sf::CircleShape {
  public:
    MovableCircle(float r, sf::Vector2f pos) :
      sf::CircleShape(r), m{2 * r} {
      std::srand(time(nullptr));
      setOrigin(sf::Vector2f(r, r));
      setPosition(pos);
      
      float angle = (std::rand() % 360) * 3.14f / 180.f;
      /* float speed = (std::rand() % 100) + 50.f; */
      float speed = 0.0f;

      velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
    }

    void update(sf::Time elapsed) {
      move(velocity.x * elapsed.asSeconds(), velocity.y * elapsed.asSeconds());
    }

    void checkWallCollision(sf::Vector2u winsize) {
      sf::Vector2f pos = getPosition();
      float radius = getRadius();

      if (pos.x - radius <= 0) {
        setPosition(radius, pos.y);
        velocity.x = -velocity.x;
      }

      if (pos.x + radius >= winsize.x) {
        setPosition(winsize.x - radius, pos.y);
        velocity.x = -velocity.x;
      }

      if (pos.y - radius <= 0) {
        setPosition(pos.x, radius);
        velocity.y = -velocity.y;
      }

      if (pos.y + radius >= winsize.y) {
        setPosition(pos.x, winsize.y - radius);
        velocity.y = -velocity.y;
      }
    }

    static bool isCollide(std::shared_ptr<MovableCircle>& lhs,
                          std::shared_ptr<MovableCircle>& rhs) {
      float r1 = lhs->getRadius();
      float r2 = rhs->getRadius();
      sf::Vector2f pos1 = lhs->getPosition();
      sf::Vector2f pos2 = rhs->getPosition();

      float fDistance = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) +
                              (pos2.y - pos1.y) * (pos2.y - pos1.y));

      return fDistance <= r1 + r2;
    }

    static void ballCollisionProcess(std::shared_ptr<MovableCircle>& lhs,
                                     std::shared_ptr<MovableCircle>& rhs) {
      float r1 = lhs->getRadius();
      float r2 = rhs->getRadius();
      sf::Vector2f pos1 = lhs->getPosition();
      sf::Vector2f pos2 = rhs->getPosition();

      float fDistance = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) +
                              (pos2.y - pos1.y) * (pos2.y - pos1.y));

      float fOverlap = 0.5f * (r1 + r2 - fDistance);
      sf::Vector2f direction((pos2.x - pos1.x) / fDistance, (pos2.y - pos1.y) / fDistance);

      lhs->move(fOverlap * -direction.x, fOverlap * -direction.y);
      rhs->move(direction.x * fOverlap, direction.y * fOverlap);
    }

    void setVelocity(const sf::Vector2f& v) {
      velocity = v;
    }

    float mass() {
      return m;
    }

    sf::Vector2f getVelocity() const { return velocity; }

  private:
    sf::Vector2f velocity;
    float m;
};

#endif // ENTITY_H_ 
