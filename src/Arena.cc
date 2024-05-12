#include "Arena.h"

// ctors
Arena::Arena(sf::VideoMode mode) : sf::RenderWindow(mode, "fofun", sf::Style::Fullscreen),
                            selected{nullptr}, size{getSize()} {
  setFramerateLimit(144);
  if (!ImGui::SFML::Init(*this)) // imgui
  {
    throw std::runtime_error("ImGui initialization failed");
  }
}

Arena::Arena(sf::VideoMode mode, int n) : Arena(mode) {
  circles.reserve(n);
  while (n--) {
    sf::Vector2f randomPos(std::rand() % size.x, std::rand() % size.y);
    circles.push_back(std::make_shared<Ball>(randomPos));
  }
}

Arena::Arena(int width, int height) : sf::RenderWindow(sf::VideoMode(width, height), "", sf::Style::Default),
                               selected{nullptr}, size{getSize()} {
  setFramerateLimit(144);
  if (!ImGui::SFML::Init(*this)) // imgui
  {
    throw std::runtime_error("ImGui initialization failed");
  }
}

Arena::Arena(int width, int height, int n) : Arena(width, height) {
  circles.reserve(n);
  while (n--) {
    sf::Vector2f randomPos(std::rand() % size.x, std::rand() % size.y);
    circles.push_back(std::make_shared<Ball>(randomPos));
  }
}

// main loop
void Arena::run() {
  while (isOpen()) {
    sf::Event event;
    pollEvents(event);

    sf::Time elapsedTime = clock.restart();
    ImGui::SFML::Update(*this, elapsedTime);

    updateArena(elapsedTime);

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

    if (wallSelection.selectedWall) {
      auto mousePosition = sf::Mouse::getPosition(*this);
      if (wallSelection.first) {
        wallSelection.selectedWall->setStartingPoint(static_cast<sf::Vector2f>(mousePosition));
      } else if (wallSelection.second) {
        wallSelection.selectedWall->setEndingPoint(static_cast<sf::Vector2f>(mousePosition));
      }
    }

    ImGuiWindow();

    /* ImGui::ShowDemoWindow(); */

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

    for (auto& fake : fake_circles) {
      draw(*fake);
    }

    ImGui::SFML::Render(*this);
    display();
  }
  
  ImGui::SFML::Shutdown();
}

// private members

void Arena::pollEvents(sf::Event& event) {
  while (pollEvent(event)) {
    ImGui::SFML::ProcessEvent(*this, event); // imgui poll events
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
        select(static_cast<sf::Vector2f>(mousePos));
      }
    }

    if (tp == sf::Event::MouseButtonReleased) {
      auto mousePos = sf::Mouse::getPosition(*this);
      if (event.mouseButton.button == sf::Mouse::Button::Left && buildingWall) {
        Line.endPoint = ImVec2(mousePos.x, mousePos.y);
        walls.push_back(std::make_shared<Wall>(sf::Vector2f(Line.startPoint.x, Line.startPoint.y), 
                                               sf::Vector2f(Line.endPoint.x, Line.endPoint.y), creationParameters.radius));
        Line.startPoint = {};
        Line.endPoint = {};

        buildingWall = false;
      } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
        unselect();
      }
    }

  }
}

void Arena::addCircle(sf::Vector2f pos) {
  circles.push_back(std::make_shared<Ball>(pos));

  auto& lastCircle = circles.back();
  if (creationParameters.randomColor == false) {
    lastCircle->setFillColor(creationParameters.shapeColor);
  }

  if (creationParameters.randomRadius == false) {
    lastCircle->setRadius(creationParameters.radius);
    lastCircle->setOrigin(sf::Vector2f(creationParameters.radius, creationParameters.radius));
    lastCircle->setMass(creationParameters.radius * 10.0f);
  }

  if (creationParameters.randomSpeed == false) {
    sf::Vector2f currentVelo = lastCircle->getVelocity();
    float spd = sqrtf(powf(currentVelo.x, 2) + powf(currentVelo.y, 2));
    sf::Vector2f direct(currentVelo.x / spd, currentVelo.y / spd);

    lastCircle->setVelocity(sf::Vector2f(creationParameters.speed * direct.x, creationParameters.speed * direct.y));
  }

  if (creationParameters.randomAngle == false) {
    // set Angle
    float radians = creationParameters.angle * 180.0f / M_PI;
    sf::Vector2f currentVelo = lastCircle->getVelocity();
    float spd = sqrtf(powf(currentVelo.x, 2) + powf(currentVelo.y, 2));

    lastCircle->setVelocity(sf::Vector2f(spd * cos(radians), spd * sin(radians)));
  }

  lastCircle->setAcceleration(gravityType == NoGravity ? sf::Vector2f(0.0f, 0.0f) : sf::Vector2f(0.0f, 100.0f));
}

void Arena::switchGravity(int t) {
  sf::Vector2f acc{0.0f, 0.0f};
  if (t == Earth) acc.y = 100.0f;

  for (auto& ptr : circles) {
    ptr->setAcceleration(acc);
  }
}

void Arena::clearArena() noexcept {
  circles.clear();
  walls.clear();
}

void Arena::updateArena(sf::Time elapsed) {
  const int iElapsedTime = 4;
  const int nMaxSimulationSteps = 15;

  for (int i = 0; i != iElapsedTime; ++i) {

    for (auto& p : circles) {
      p->fSimTimeRemaining = elapsed / (float)iElapsedTime;
    }

    for (int j = 0; j != nMaxSimulationSteps; ++j) {

      for (auto& p : circles) {
        if (p->fSimTimeRemaining > sf::Time::Zero) {
          p->prevPos = p->getPosition();
          p->prevVelo = p->getVelocity();

          p->update();

          // wall collision
          float radius = p->getRadius();
          sf::Vector2f currentPos = p->getPosition();

          if (currentPos.x < 0) {
            p->setPosition(size.x + currentPos.x, currentPos.y);
          } else if (currentPos.x > size.x) {
            p->setPosition(currentPos.x - size.x, currentPos.y);
          }

          if (currentPos.y < 0) {
            p->setPosition(currentPos.x, size.y + currentPos.y);
          } else if (currentPos.y > size.y) {
            p->setPosition(currentPos.x, currentPos.y - size.y);
          }

        }
      }

      staticCollisionProcess();

      // Time
      for (auto& p : circles) {
        sf::Vector2f intendedVelocity = p->prevVelo;
        sf::Vector2f currentPos = p->getPosition();

        float fIntendedSpeed = sqrtf(powf(intendedVelocity.x, 2) + powf(intendedVelocity.y, 2));
        float fActualDistance = sqrtf(powf(currentPos.x - p->prevPos.x, 2) + 
                                      powf(currentPos.y - p->prevPos.y, 2));
        float fActualTime = fActualDistance / fIntendedSpeed;
        p->fSimTimeRemaining -= sf::seconds(fActualTime);
      }

      dynamicCollisionProcess();

      // system total kinetic energy
      for (auto& ptr : circles) {
        sf::Vector2f velocity = ptr->getVelocity();
        float speed = sqrtf(powf(velocity.x, 2) + powf(velocity.y, 2));
        totalEnergy += ptr->getMass() * speed;
      }

      collided.clear();
      fake_circles.clear();
      collisionLines.clear();

    }
  }

}

bool Arena::isSelected() const noexcept {
  return !(selected == nullptr);
}

void Arena::select(sf::Vector2f pos) {
  for (auto& ball : circles) {
    if (ball->getGlobalBounds().contains(pos)) {
      selected = ball;

      // stop moving + extra heavy
      selected->setVelocity(sf::Vector2f(0.0f, 0.0f));
      selected->setMass(10000.0f);

      Line.startPoint = ImVec2(ball->getPosition().x, ball->getPosition().y);
      return;
    }
  }

  for (auto& wall : walls) {
    if (wall->firstCornerContains(pos)) {
      wallSelection.selectedWall = wall;
      wallSelection.first = true;

      return;

    } else if (wall->secondCornerContains(pos)) {
      wallSelection.selectedWall = wall;
      wallSelection.second = true;

      return;

    }
  }
}

void Arena::unselect() {
  if (isSelected()) {
    sf::Vector2f point1{Line.startPoint[0], Line.startPoint[1]};
    sf::Vector2f point2{Line.endPoint[0], Line.endPoint[1]};

    sf::Vector2f velocityVector(point2.x - point1.x, point2.y - point1.y);
    selected->setVelocity(-1.0f * velocityVector);
    selected->setMass(selected->getRadius() * 10.0f);

    Line.startPoint = {};
    Line.endPoint = {};

    selected = nullptr;
  } else if (wallSelection.selectedWall) {
    wallSelection.selectedWall.reset();
    wallSelection.first = false;
    wallSelection.second = false;
  }
}
