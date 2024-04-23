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
    
    MovableCircle(const sf::Vector2f& pos) {
      // set random radius
      float radius = std::rand() % 10 + 10;
      std::cout << "Radius = " << radius << "\n";
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

    void checkWallCollision(sf::Vector2u winsize) {
      sf::Vector2f pos = getPosition();
      float radius = getRadius();

      if (pos.x - radius <= 0) {
        setPosition(radius, pos.y);
        velocity.x = -velocity.x;
      } else if (pos.x + radius >= winsize.x) {
        setPosition(winsize.x - radius, pos.y);
        velocity.x = -velocity.x;
      }

      if (pos.y - radius <= 0) {
        setPosition(pos.x, radius);
        velocity.y = -velocity.y;
      } else if (pos.y + radius >= winsize.y) {
        setPosition(pos.x, winsize.y - radius);
        velocity.y = -velocity.y;
      }

    }

    static std::vector<collided_pair> staticCollisionProcess(const std::vector<std::shared_ptr<MovableCircle>>& circles,
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

        // normal vector
        sf::Vector2f normal((pos2.x - pos1.x) / fDistance, (pos2.y - pos1.y) / fDistance);
        //tangental vector
        sf::Vector2f tan(-normal.y, normal.x);

        // normal component of velocity
        float normalVelo1 = ptr1->velocity.x * normal.x + ptr1->velocity.y * normal.y;
        float normalVelo2 = ptr2->velocity.x * normal.x + ptr2->velocity.y * normal.y;

        // tangental component of velocity
        float tanVelo1 = ptr1->velocity.x * tan.x + ptr1->velocity.y * tan.y;
        float tanVelo2 = ptr2->velocity.x * tan.x + ptr2->velocity.y * tan.y;

        float u1 = ((ptr1->mass - ptr2->mass) * normalVelo1 + 2.0f * ptr2->mass * normalVelo2) /
                    (ptr1->mass + ptr2->mass);
        float u2 = ((ptr2->mass - ptr1->mass) * normalVelo2 + 2.0f * ptr1->mass * normalVelo1) /
                    (ptr1->mass + ptr2->mass);

        ptr1->velocity = sf::Vector2f(u1 * normal.x, u1 * normal.y) +
                         sf::Vector2f(tanVelo1 * tan.x, tanVelo1 * tan.y);
        ptr2->velocity = sf::Vector2f(u2 * normal.x, u2 * normal.y) +
                         sf::Vector2f(tanVelo2 * tan.x, tanVelo2 * tan.y);
        
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
