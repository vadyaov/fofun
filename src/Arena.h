#ifndef ARENA_H_
#define ARENA_H_

#include <vector>
#include <memory>

#include "imgui-SFML.h"
#include "imgui.h"

#include "Ball.h"

using std::vector;

class Arena : public sf::RenderWindow {
  public:
    using circlePtr = std::shared_ptr<Ball>;
    using circlePair = std::pair<circlePtr, circlePtr>;

    Arena(sf::VideoMode mode) : sf::RenderWindow(mode, "fofun", sf::Style::Fullscreen),
                                selected{nullptr}, size{getSize()}, totalCircles{0} {
      setFramerateLimit(144);
      if (!ImGui::SFML::Init(*this)) // imgui
      {
        throw std::runtime_error("ImGui initialization failed");
      }
    }

    Arena(sf::VideoMode mode, int n) : Arena(mode) {
      totalCircles = n;

      circles.reserve(totalCircles);
      while (n--) {
        sf::Vector2f randomPos(std::rand() % size.x, std::rand() % size.y);
        circles.push_back(std::make_shared<Ball>(randomPos));
      }
    }

    Arena(int width, int height) : sf::RenderWindow(sf::VideoMode(width, height), "", sf::Style::Default),
                                   selected{nullptr}, size{getSize()}, totalCircles{0} {
      setFramerateLimit(144);
      if (!ImGui::SFML::Init(*this)) // imgui
      {
        throw std::runtime_error("ImGui initialization failed");
      }
    }

    Arena(int width, int height, int n) : Arena(width, height) {
      totalCircles = n;

      circles.reserve(totalCircles);
      while (n--) {
        sf::Vector2f randomPos(std::rand() % size.x, std::rand() % size.y);
        circles.push_back(std::make_shared<Ball>(randomPos));
      }
    }

    void addCircle(sf::Vector2f pos) {
      circles.push_back(std::make_shared<Ball>(pos));
      totalCircles++;
    }

    void run() {

      while (isOpen()) {
        sf::Event event;
        while (pollEvent(event)) {
          /* ImGui::SFML::ProcessEvent(event); // imgui DEPRECATED */
          ImGui::SFML::ProcessEvent(*this, event); // imgui 
          sf::Event::EventType tp = event.type;

          if (tp == sf::Event::Closed || (tp == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) {
              close();
          }

          if (tp == sf::Event::MouseButtonPressed) {
            sf::Event::MouseButtonEvent mouseEvent = event.mouseButton;

            if (mouseEvent.button == sf::Mouse::Button::Left && !isSelected()) {
              addCircle(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*this)));
            } else if (mouseEvent.button == sf::Mouse::Button::Right && !isSelected()) {
              selectCircle(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*this)));
            }
          }

          if (tp == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
            unselect();
          }

        }

        sf::Time elapsedTime = clock.restart();
        ImGui::SFML::Update(*this, elapsedTime);

        update(elapsedTime);

        ImGui::Begin("FORFUN settings");
        ImGui::Button("Look at this pretty button");
        ImGui::ColorEdit3("Background color", fclear);
        ImGui::End();

        ImGui::ShowDemoWindow();

        col.r = fclear[0];
        col.g = fclear[1];
        col.b = fclear[2];

        clear(col);

        for (auto& circle : circles) {
          draw(*circle);
        }

        for (auto& line : collisionLines) {
          draw(line);
        }

        draw(speedLine);

        ImGui::SFML::Render(*this);
        display();
      }
      
      ImGui::SFML::Shutdown();

    }

    void update(sf::Time elapsed) {
      collisionLines.clear();
      collided.clear();

      for (auto& p : circles) {
        p->update(elapsed);

        // wall collision
        sf::Vector2f currentPos = p->getPosition();
        sf::Vector2f currentVel = p->getVelocity();
        float radius = p->getRadius();

        if (currentPos.x - radius <= 0) {
          p->setPosition(radius, currentPos.y);
          p->setVelocity({-currentVel.x, currentVel.y});
        } else if (currentPos.x + radius >= size.x) {
          p->setPosition(size.x - radius, currentPos.y);
          p->setVelocity({-currentVel.x, currentVel.y});
        }

        if (currentPos.y - radius <= 0) {
          p->setPosition(currentPos.x, radius);
          p->setVelocity({currentVel.x, -currentVel.y});
        } else if (currentPos.y + radius >= size.y) {
          p->setPosition(currentPos.x, size.y - radius);
          p->setVelocity({currentVel.x, -currentVel.y});
        }

      }

      staticCollisionProcess();
      dynamicCollisionProcess();

      if (isSelected()) {
        speedLine[1] = sf::Vertex(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*this)), sf::Color::Blue);
        selected->setPosition(speedLine[0].position);
      }

    }

    bool isSelected() const {
      return !(selected == nullptr);
    }

    void selectCircle(sf::Vector2f pos) {
      for (auto& ptr : circles) {
        if (ptr->getGlobalBounds().contains(pos)) {
          selected = ptr;
          selected->setVelocity(sf::Vector2f(0.0f, 0.0f));
          selected->setMass(10000.0f);
          speedLine[0] = sf::Vertex(ptr->getPosition(), sf::Color::Blue);
          break;
        }
      }
    }

    void unselect() {
      if (isSelected()) {
        sf::Vector2f point1 = speedLine[0].position;
        sf::Vector2f point2 = speedLine[1].position;

        sf::Vector2f velocityVector(point2.x - point1.x, point2.y - point1.y);
        selected->setVelocity(-1.0f * velocityVector);
        selected->setMass(selected->getRadius() * 10.0f);

        speedLine[0] = {};
        speedLine[1] = {};

        selected = nullptr;
      }
    }

  private:
    void staticCollisionProcess() {
      for (int i = 0; i < totalCircles - 1; ++i) {
        sf::Vector2f pos1 = circles[i]->getPosition();
        float r1 = circles[i]->getRadius();

        for (int j = i + 1; j < totalCircles; ++j) {
          sf::Vector2f pos2 = circles[j]->getPosition();
          float r2 = circles[j]->getRadius();

          float fDistance = sqrtf(powf(pos2.x - pos1.x, 2) + powf(pos2.y - pos1.y, 2));
          if (r1 + r2 >= fDistance) {
            // collision
            float fOverlap = 0.5f * (r1 + r2 - fDistance);

            // normal unit vector directed from pos2 to pos1
            sf::Vector2f normal((pos2.x - pos1.x) / fDistance, (pos2.y - pos1.y) / fDistance);
            
            // avoid the collision
            circles[i]->move(-fOverlap * normal.x, -fOverlap * normal.y);
            circles[j]->move(fOverlap * normal.x, fOverlap * normal.y);

            // create line of collision
            sf::VertexArray collisionLine(sf::Lines, 2);
            collisionLine[0] = pos1;
            collisionLine[1] = pos2;
            collisionLines.push_back(std::move(collisionLine));

            // remember collided pair
            collided.push_back(std::make_pair(circles[i], circles[j]));
          }
        }
      }
    }

    void dynamicCollisionProcess() {
      for (auto& [ptr1, ptr2] : collided) {
        sf::Vector2f pos1 = ptr1->getPosition();
        sf::Vector2f pos2 = ptr2->getPosition();
        float fDistance = sqrtf(powf(pos2.x - pos1.x, 2) + powf(pos2.y - pos1.y, 2));

        sf::Vector2f velo1 = ptr1->getVelocity();
        sf::Vector2f velo2 = ptr2->getVelocity();

        float m1 = ptr1->getMass();
        float m2 = ptr2->getMass();

        // normal vector
        sf::Vector2f normal((pos2.x - pos1.x) / fDistance, (pos2.y - pos1.y) / fDistance);
        // tangental vector
        sf::Vector2f tan(-normal.y, normal.x);

        // normal component of velocity
        float normalVelo1 = velo1.x * normal.x + velo1.y * normal.y;
        float normalVelo2 = velo2.x * normal.x + velo2.y * normal.y;

        // tangental component of velocity
        float tanVelo1 = velo1.x * tan.x + velo1.y * tan.y;
        float tanVelo2 = velo2.x * tan.x + velo2.y * tan.y;

        // normal velocities after collision
        float u1 = ((m1 - m2) * normalVelo1 + 2.0f * m2 * normalVelo2) / (m1 + m2);
        float u2 = ((m2 - m1) * normalVelo2 + 2.0f * m1 * normalVelo1) / (m1 + m2);

        ptr1->setVelocity(sf::Vector2f(u1 * normal.x, u1 * normal.y) +
                          sf::Vector2f(tanVelo1 * tan.x, tanVelo1 * tan.y));
        ptr2->setVelocity(sf::Vector2f(u2 * normal.x, u2 * normal.y) +
                          sf::Vector2f(tanVelo2 * tan.x, tanVelo2 * tan.y));
      }
    }

  private:
    vector<circlePtr> circles;
    vector<circlePair> collided;
    circlePtr selected;

    vector<sf::VertexArray> collisionLines;
    sf::VertexArray speedLine{sf::Lines, 2}; // ?

    sf::Vector2u size;

    int totalCircles;

    sf::Clock clock;

    float fclear[3];
    sf::Color col = sf::Color::Black;
};

#endif // ARENA_H_
