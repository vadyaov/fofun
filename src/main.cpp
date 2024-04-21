#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

/* #include "Fabric.hpp" */
#include "Entity.h"

void checkCollisions(std::vector<std::shared_ptr<MovableCircle>>& circles) {
  for (auto ptr1 : circles) {
    float r1 = ptr1->getRadius();
    sf::Vector2f pos1 = ptr1->getPosition();
    for (auto ptr2 : circles) {
      if (ptr1 != ptr2) {
        float r2 = ptr2->getRadius();
        sf::Vector2f pos2 = ptr2->getPosition();

        // distance between balls
        float fDistance = sqrtf((pos2.x - pos1.x) * (pos2.x - pos1.x) +
                               (pos2.y - pos1.y) * (pos2.y - pos1.y));
        if (fDistance <= r1 + r2) {
          std::cout << "Collision\n";
          sf::Vector2f velo1 = ptr1->getVelocity();
          sf::Vector2f velo2 = ptr2->getVelocity();

          // normal vector
          float nx = (pos2.x - pos1.x) / fDistance;
          float ny = (pos2.y - pos1.y) / fDistance;

          float tx = -ny;
          float ty = nx;

          float dpTan1 = velo1.x * tx + velo1.y * ty;
          float dpTan2 = velo2.x * tx + velo2.y * ty;

          float dpNorm1 = velo1.x * nx + velo1.y * ny;
          float dpNorm2 = velo2.x * nx + velo2.y * ny;

          float m1 = (dpNorm1 * (ptr1->mass() - ptr2->mass()) + 2.0f * ptr2->mass() * dpNorm2) /
            (ptr1->mass() + ptr2->mass());
          float m2 = (dpNorm2 * (ptr2->mass() - ptr1->mass()) + 2.0f * ptr1->mass() * dpNorm1) /
            (ptr1->mass() + ptr2->mass());

          ptr1->setVelocity(sf::Vector2f(tx * dpTan1 + nx * m1,
                                         ty * dpTan1 + ny * m1));
          ptr2->setVelocity(sf::Vector2f(tx * dpTan2 + nx * m2,
                                         ty * dpTan2 + ny * m2));
        }
      }
    }
  }
}

using collided_pair = std::pair<std::shared_ptr<MovableCircle>, std::shared_ptr<MovableCircle>>;

std::vector<collided_pair> findCollidedCircles(const std::vector<std::shared_ptr<MovableCircle>>& circles,
                                               std::vector<sf::VertexArray>& lines) {
  lines.clear();
  std::vector<collided_pair> collided;
  
  for (auto ptr1 : circles) {
    for (auto ptr2 : circles) {
      if (ptr1 != ptr2 && MovableCircle::isCollide(ptr1, ptr2)) {
        collided.push_back(std::make_pair(ptr1, ptr2));
        sf::VertexArray line(sf::Lines, 2);
        line[0] = sf::Vertex(ptr1->getPosition(), sf::Color::Red);
        line[1] = sf::Vertex(ptr2->getPosition(), sf::Color::Red);
        lines.push_back(line);
      }
    }
  }

  return collided;
}


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "forfun project", sf::Style::Default);
    window.setFramerateLimit(144);

    std::vector<std::shared_ptr<MovableCircle>> circles;
    sf::Clock clock;

    bool binded = false;
    std::shared_ptr<MovableCircle> bindedCircle = nullptr;

    std::vector<collided_pair> collidedCircles;
    std::vector<sf::VertexArray> collisionLines;

    sf::VertexArray speedLine(sf::Lines, 2);

    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        sf::Event::EventType tp = event.type;
        if (tp == sf::Event::Closed ||
            (tp == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) {
            window.close();
        }

        if (tp == sf::Event::MouseButtonPressed) {

          sf::Event::MouseButtonEvent ms_button_event = event.mouseButton;
          int x = ms_button_event.x;
          int y = ms_button_event.y;
          if (ms_button_event.button == sf::Mouse::Button::Left && !binded) {
            std::shared_ptr<MovableCircle> c(new MovableCircle(30.0f, sf::Vector2f(x, y)));
            circles.push_back(c);
          } else if (ms_button_event.button == sf::Mouse::Button::Right) {
            if (binded == false) {
              sf::Vector2i mousePos = sf::Mouse::getPosition(window);
              for (auto cptr : circles) {
                sf::FloatRect bounds = cptr->getGlobalBounds();
                if (bounds.contains((float)mousePos.x, (float)mousePos.y)) {
                  binded = true;
                  bindedCircle = cptr;
                  speedLine[0] = sf::Vertex(bindedCircle->getPosition(), sf::Color::Blue);
                  bindedCircle->setVelocity({});
                  break;
                }
              }
            }
          }

        }

        if (tp == sf::Event::MouseButtonReleased) {
          sf::Event::MouseButtonEvent ms_button_event = event.mouseButton;
          if (ms_button_event.button == sf::Mouse::Button::Right && binded) {
            binded = false;
            bindedCircle->setVelocity(sf::Vector2f(speedLine[0].position.x - speedLine[1].position.x,
                                                   speedLine[0].position.y - speedLine[1].position.y));
            speedLine[0] = {};
            speedLine[1] = {};
          }
        }

      }

      if (binded) {
          speedLine[1] = sf::Vertex(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)), sf::Color::Blue);
      }

      sf::Time elapsed = clock.restart();
      for (auto ptr : circles) {
        ptr->update(elapsed);
        ptr->checkWallCollision(window.getSize());
      }

      collidedCircles = findCollidedCircles(circles, collisionLines);
      for (auto& [ptr1, ptr2] : collidedCircles) {
        MovableCircle::ballCollisionProcess(ptr1, ptr2);
      }

      window.clear(sf::Color::Black);
      for (auto ptr : circles) {
        window.draw(*ptr);
      }

      for (auto arr : collisionLines) {
        window.draw(arr);
      }

      window.draw(speedLine);
      window.display();
    }

    return 0;
}
