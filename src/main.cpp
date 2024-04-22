#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#include "Entity.h"

using collided_pair = std::pair<std::shared_ptr<MovableCircle>, std::shared_ptr<MovableCircle>>;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "forfun project", sf::Style::Default);
    window.setFramerateLimit(144);

    std::vector<std::shared_ptr<MovableCircle>> circles;
    sf::Clock clock;

    bool binded = false;
    std::shared_ptr<MovableCircle> bindedCircle = nullptr;

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


      if (binded)
          speedLine[1] = sf::Vertex(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)), sf::Color::Blue);


      sf::Time elapsed = clock.restart();
      for (auto ptr : circles) {
        ptr->update(elapsed);
        ptr->checkWallCollision(window.getSize());
      }

      std::vector<sf::VertexArray> collisionLines;
      std::vector<collided_pair> collidedCircles = MovableCircle::getCollidedCircles(circles, collisionLines);

      MovableCircle::DynamicCollisionProcess(collidedCircles);

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
