#ifndef ARENA_H_
#define ARENA_H_

#include <vector>
#include <memory>
#include <iostream>

#include "imgui-SFML.h"
#include "imgui.h"

#include "Ball.h"
#include "Wall.h"

using std::vector;

class Arena : public sf::RenderWindow {
  public:
    using circlePtr = std::shared_ptr<Ball>;
    using wallPtr = std::shared_ptr<Wall>;
    using circlePair = std::pair<circlePtr, circlePtr>;

    enum ShapeType {NoShape, Circle, Rectangle, Triangle};
    enum GravityType {NoGravity, Earth};

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
      auto& lastCircle = circles.back();
      if (creationParameters.randomShape == false) {

        float radians = (float)creationParameters.angle * 3.14f / 180.0f;
        lastCircle->setRadius(creationParameters.radius);
        lastCircle->setOrigin(sf::Vector2f(creationParameters.radius, creationParameters.radius));
        lastCircle->setVelocity(sf::Vector2f(std::cos(radians) * creationParameters.speed,
                                            std::sin(radians) * creationParameters.speed));
        lastCircle->setFillColor(creationParameters.shapeColor);
        lastCircle->setMass(creationParameters.radius * 10.0f);
      }
      lastCircle->setAcceleration(gravityType == NoGravity ? sf::Vector2f(0.0f, 0.0f) : sf::Vector2f(0.0f, 100.0f));
      totalCircles++;
    }

    void run() {

      while (isOpen()) {
        sf::Event event;
        while (pollEvent(event)) {
          /* ImGui::SFML::ProcessEvent(event); // imgui DEPRECATED */
          ImGui::SFML::ProcessEvent(*this, event); // imgui 
          ImGuiIO& io = ImGui::GetIO();
          sf::Event::EventType tp = event.type;

          if (tp == sf::Event::Closed || (tp == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) {
              close();
          }

          if (tp == sf::Event::MouseButtonPressed) {
            sf::Event::MouseButtonEvent mouseEvent = event.mouseButton;
            auto mousePos = sf::Mouse::getPosition(*this);

            if (mouseEvent.button == sf::Mouse::Button::Left && !io.WantCaptureMouse) {
              switch (shapeType) {
                case Circle:
                  if (!isSelected())
                    addCircle(static_cast<sf::Vector2f>(mousePos));
                  break;
                case Rectangle:
                  buildingWall = true;
                  Line.startPoint = ImVec2(mousePos.x, mousePos.y);
                  break;
                case Triangle:
                  // add Triangle
                  break;
                default:
                  break;
              }
            } else if (mouseEvent.button == sf::Mouse::Button::Right && !isSelected()) {
              selectCircle(static_cast<sf::Vector2f>(mousePos));
            }
          }

          if (tp == sf::Event::MouseButtonReleased) {
            auto mousePos = sf::Mouse::getPosition(*this);
            if (event.mouseButton.button == sf::Mouse::Button::Left && buildingWall) {
              Line.endPoint = ImVec2(mousePos.x, mousePos.y);
              walls.push_back(std::make_shared<Wall>(sf::Vector2f(Line.startPoint.x, Line.startPoint.y), 
                                                     sf::Vector2f(Line.endPoint.x, Line.endPoint.y), 20.0f));
              Line.startPoint = {};
              Line.endPoint = {};

              buildingWall = false;
            } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
              unselect();
            }
          }

        }

        sf::Time elapsedTime = clock.restart();
        ImGui::SFML::Update(*this, elapsedTime);

        update(elapsedTime);

        if (isSelected() || buildingWall) {
          /* Line[1] = sf::Vertex(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*this)), sf::Color::Blue); */
          auto mousePosition = sf::Mouse::getPosition(*this);
          Line.endPoint = ImVec2(mousePosition.x, mousePosition.y);
          if (isSelected())
            selected->setPosition(Line.startPoint);

          // Draw a line between the button and the mouse cursor
          ImGui::GetForegroundDrawList()->AddLine(Line.startPoint, Line.endPoint,
                                                  ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
        }

        ImGui::Begin("FORFUN settings");
        {
          ImGui::ColorEdit4("Background color", (float*)&backColor);
          
          ImGui::AlignTextToFramePadding();
          ImGui::Text("Circles: %d", totalCircles); ImGui::SameLine();
          if (ImGui::Button("Clear")) {
            clearArena();
          }
         
          if (ImGui::CollapsingHeader("Create")) {
            if (ImGui::RadioButton("Circle", shapeType == Circle)) { shapeType = Circle; } ImGui::SameLine();
            if (ImGui::RadioButton("Rectangle", shapeType == Rectangle)) { shapeType = Rectangle; } ImGui::SameLine();
            if (ImGui::RadioButton("Triangle", shapeType == Triangle)) { shapeType = Triangle; } ImGui::SameLine();

            ImGui::ColorEdit4("Shape color", (float*)&creationParameters.shapeColor,
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            if (shapeType == Circle) {
              ImGui::Checkbox("Random", &creationParameters.randomShape);
              ImGui::SliderFloat("radius", &creationParameters.radius, 10.0f, 20.0f, "%.2f");
              ImGui::SliderFloat("speed", &creationParameters.speed, 0.0f, 200.0f, "%.2f");
              ImGui::SliderInt("angle", &creationParameters.angle, 0, 360);
            }

          }

          if (ImGui::CollapsingHeader("Gravity")) {
            if (ImGui::RadioButton("None", gravityType == NoGravity)) {
              gravityType = NoGravity;
              switchGravity(gravityType);
            } ImGui::SameLine();
            if (ImGui::RadioButton("Earth", gravityType == Earth)) {
              gravityType = Earth;
              switchGravity(gravityType);
            } // ImGui::SameLine();
          }
        }
        ImGui::End();

        ImGui::ShowDemoWindow();

        clear(backColor);

        for (auto& circle : circles) {
          draw(*circle);
        }

        for (auto& line : collisionLines) {
          draw(line);
        }

        for (auto& wall : walls) {
          draw(*wall);
        }

        ImGui::SFML::Render(*this);
        display();
      }
      
      ImGui::SFML::Shutdown();

    }

    void switchGravity(int t) {
      sf::Vector2f acc{0.0f, 0.0f};
      if (t == Earth) acc.y = 100.0f;

      for (auto& ptr : circles) {
        ptr->setAcceleration(acc);
      }
    }

    void clearArena() {
      circles.clear();
      totalCircles = 0;
    }

    void update(sf::Time elapsed) {
      collisionLines.clear();
      collided.clear();

      // for each frame update we're going to run physics simulation 4 times
      int nSimulationUpdates = 4;
      int maxSimulationSteps = 15;

      // put all physics code here
      for (int i = 0; i != nSimulationUpdates; ++i) {

        for (auto& circle : circles) {
          circle->fSimTimeRemaining = elapsed / (float)nSimulationUpdates;
        }

        for (int j = 0; j != maxSimulationSteps; ++j) {
          for (auto& p : circles) {
            if (p->fSimTimeRemaining > sf::Time::Zero) {
              p->prevPos = p->getPosition();
              p->update();

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

            // Time
            /* sf::Vector2f previos_pos = p->prevPos; */
            /* sf::Vector2f current_pos = p->getPosition(); */
            /* sf::Vector2f velo = p->getVelocity(); */

            /* float fIntendedSpeed = sqrtf(powf(velo.x, 2) + powf(velo.y, 2)); */
            /* float fIntendedDistance = fIntendedSpeed * p->fSimTimeRemaining.asSeconds(); */
            /* float fActualDistance = sqrtf(powf(current_pos.x - previos_pos.x, 2) + powf(current_pos.y - previos_pos.y, 2)); */

            /* float fActualTime = fActualDistance / fIntendedSpeed; */

            /* std::cout << "IntendedDistance = " << fIntendedDistance << " "; */
            /* std::cout << "ActualDistance = " << fActualDistance << "\n"; */

            /* std::cout << "IntendedTime = " << p->fSimTimeRemaining.asSeconds() << " "; */
            /* std::cout << "ActualTime = " << (sf::seconds(fActualTime)).asSeconds() << "\n"; */

            /* p->fSimTimeRemaining -= sf::seconds(fActualTime); */

          }

          staticCollisionProcess();
          dynamicCollisionProcess();
        }

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
          /* Line[0] = sf::Vertex(ptr->getPosition(), sf::Color::Blue); */
          Line.startPoint = ImVec2(ptr->getPosition().x, ptr->getPosition().y);
          break;
        }
      }
    }

    void unselect() {
      if (isSelected()) {
        /* sf::Vector2f point1 = Line[0].position; */
        /* sf::Vector2f point2 = Line[1].position; */

        /* sf::Vector2f velocityVector(point2.x - point1.x, point2.y - point1.y); */

        sf::Vector2f point1{Line.startPoint[0], Line.startPoint[1]};
        sf::Vector2f point2{Line.endPoint[0], Line.endPoint[1]};

        sf::Vector2f velocityVector(point2.x - point1.x, point2.y - point1.y);
        selected->setVelocity(-1.0f * velocityVector);
        selected->setMass(selected->getRadius() * 10.0f);

        /* Line[0] = {}; */
        /* Line[1] = {}; */

        Line.startPoint = {};
        Line.endPoint = {};

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

          // case when user is trying to create ball in the center of other ball
          // creation lock
          if (fabs(fDistance) <= 1) {
            circles.erase(circles.begin() + j);
            totalCircles--;
            return;
          }

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

            // time displacement
            // 1.
            /* float fIntendedSpeed1 = sqrtf(powf(circles[i]->getVelocity().x, 2) + powf(circles[i]->getVelocity().y, 2)); */
            /* float fIntendedDistance1 = fIntendedSpeed1 * circles[i]->fSimTimeRemaining.asSeconds(); */
            /* float fActualDistance1 = sqrtf(powf(circles[i]->getPosition().x - circles[i]->prevPos.x, 2) + */
            /*                                powf(circles[i]->getPosition().y - circles[i]->prevPos.y, 2)); */
            /* float fActualTime1 = fActualDistance1 / fIntendedSpeed1; */

            /* circles[i]->fSimTimeRemaining -= sf::seconds(fActualTime1); */

            /* // 2. */
            /* float fIntendedSpeed2 = sqrtf(powf(circles[j]->getVelocity().x, 2) + powf(circles[j]->getVelocity().y, 2)); */
            /* float fIntendedDistance2 = fIntendedSpeed1 * circles[j]->fSimTimeRemaining.asSeconds(); */
            /* float fActualDistance2 = sqrtf(powf(circles[j]->getPosition().x - circles[j]->prevPos.x, 2) + */
            /*                                powf(circles[j]->getPosition().y - circles[j]->prevPos.y, 2)); */
            /* float fActualTime2 = fActualDistance2 / fIntendedSpeed2; */

            /* circles[j]->fSimTimeRemaining -= sf::seconds(fActualTime2); */
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
    vector<circlePtr> circles; // want it to be some ShapePtr, where shape is circle or rect or triangle and so on
    vector<circlePair> collided;
    circlePtr selected;

    vector<sf::VertexArray> collisionLines;
    /* sf::VertexArray Line{sf::Lines, 2}; // ? */

    vector<wallPtr> walls;
    bool buildingWall = false;

    struct {
      ImVec2 startPoint;
      ImVec2 endPoint;
    } Line;

    sf::Vector2u size;

    int totalCircles;

    sf::Clock clock;

    ImVec4 backColor {0.15f, 0.15f, 0.15f, 0.0f}; // background color

    struct Params {
      ImVec4 shapeColor {1.0f, 0.0f, 0.0f, 1.0f};   // shape color
      float radius = 10.0f;
      float speed = 0.0f;
      int angle = 0;

      bool randomShape = false;
    } creationParameters;

    int shapeType = NoShape;
    int gravityType = NoGravity;
};

#endif // ARENA_H_
