#ifndef ENTITY_H_
#define ENTITY_H_

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <memory>

class MovableCircle : public sf::CircleShape {
  public:

  using collided_pair = std::pair<std::shared_ptr<MovableCircle>, std::shared_ptr<MovableCircle>>;

    MovableCircle(float r, sf::Vector2f pos) :
      sf::CircleShape(r), mass{2 * r} {
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

    static std::vector<collided_pair> getCollidedCircles(const std::vector<std::shared_ptr<MovableCircle>>& circles,
                                                  std::vector<sf::VertexArray>& lines) {
      std::vector<collided_pair> collided;

      int n = (int)circles.size();
      for (int i = 0; i < n - 1; ++i) {
        auto ptr1 = circles[i];
        sf::Vector2f pos1 = ptr1->getPosition();
        float r1 = ptr1->getRadius();
        for (int j = i + 1; j < n; ++j) {
          auto ptr2 = circles[j];
          sf::Vector2f pos2 = ptr2->getPosition();
          float r2 = ptr2->getRadius();

          // collision
          float fDistance = sqrtf(powf(pos2.x - pos1.x, 2) + powf(pos2.y - pos1.y, 2));
          if (fDistance <= r1 + r2) {

            sf::VertexArray line(sf::Lines, 2);
            line[0] = sf::Vertex(pos1, sf::Color::Red);
            line[1] = sf::Vertex(pos2, sf::Color::Red);
            lines.push_back(line);

            sf::Vector2f normal((pos2.x - pos1.x) / fDistance, (pos2.y - pos1.y) / fDistance);
            float fOverlap = 0.5f * (r1 + r2 - fDistance);

            ptr1->move(-normal.x * fOverlap, -normal.y * fOverlap);
            ptr2->move(normal.x * fOverlap, normal.y * fOverlap);

            collided.push_back({ptr1, ptr2});
          }
        }
      }

      return collided;
    }

    static void DynamicCollisionProcess(std::vector<collided_pair>& collided) {
      for (auto& [ptr1, ptr2] : collided) {
        sf::Vector2f pos1 = ptr1->getPosition();
        sf::Vector2f pos2 = ptr2->getPosition();
        float fDistance = sqrtf(powf(pos2.x - pos1.x, 2) + powf(pos2.y - pos1.y, 2));
        sf::Vector2f normal((pos2.x - pos1.x) / fDistance, (pos2.y - pos1.y) / fDistance);

        sf::Vector2f tan(-normal.y, normal.x);
        
        sf::Vector2f tangentVelo1(ptr1->velocity.x * fabs(tan.x), ptr1->velocity.y * fabs(tan.y));
        sf::Vector2f tangentVelo2(ptr2->velocity.x * fabs(tan.x), ptr2->velocity.y * fabs(tan.y));

        sf::Vector2f normVelo1(ptr1->velocity.x * fabs(normal.x), ptr1->velocity.y * fabs(normal.y));
        sf::Vector2f normVelo2(ptr2->velocity.x * fabs(normal.x), ptr2->velocity.y * fabs(normal.y));

        /* std::swap(normVelo1, normVelo2); */

        std::cout << "1. " << tangentVelo1.x << " " << tangentVelo1.y << "\n";
        std::cout << "   " << normVelo1.x << " " << normVelo1.y << "\n";

        std::cout << "2. " << tangentVelo2.x << " " << tangentVelo2.y << "\n";
        std::cout << "   " << normVelo2.x << " " << normVelo2.y << "\n";

        ptr1->velocity = tangentVelo1 + normVelo2;
        ptr2->velocity = tangentVelo2 + normVelo1;

      }
    }

    void setVelocity(const sf::Vector2f& v) {
      velocity = v;
    }

    float getMass() {
      return mass;
    }

    sf::Vector2f getVelocity() const { return velocity; }

  private:
    sf::Vector2f velocity;
    float mass;
};

#endif // ENTITY_H_ 
