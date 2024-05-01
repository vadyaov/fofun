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

    ImGuiWindow();

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

void Arena::staticCollisionProcess() {
  int totalCircles = (int)circles.size();
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
      }
    }
  }
}

void Arena::dynamicCollisionProcess() {
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
  collisionLines.clear();
  collided.clear();

  // for each frame update we're going to run physics simulation 4 times
  /* int nSimulationUpdates = 4; */
  /* int maxSimulationSteps = 15; */

  // put all physics code here
  /* for (int i = 0; i != nSimulationUpdates; ++i) { */

    /* for (auto& circle : circles) { */
    /*   circle->fSimTimeRemaining = elapsed / (float)nSimulationUpdates; */
    /* } */

    /* for (int j = 0; j != maxSimulationSteps; ++j) { */
      for (auto& p : circles) {
        /* if (p->fSimTimeRemaining > sf::Time::Zero) { */
          p->setPreviousPosition(p->getPosition());
          p->update(elapsed);

          // wall collision
          float radius = p->getRadius();
          sf::Vector2f currentPos = p->getPosition();
          sf::Vector2f currentVel = p->getVelocity();

          float speed = sqrtf(powf(currentVel.x, 2) + powf(currentVel.y, 2));
          sf::Vector2f direct(currentVel.x / speed, currentVel.y / speed);

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

        /* } */

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
    /* } */

  /* } */

}

bool Arena::isSelected() const noexcept {
  return !(selected == nullptr);
}

void Arena::select(sf::Vector2f pos) {
  for (auto& ptr : circles) {
    if (ptr->getGlobalBounds().contains(pos)) {
      selected = ptr;

      // stop moving + extra heavy
      selected->setVelocity(sf::Vector2f(0.0f, 0.0f));
      selected->setMass(10000.0f);

      Line.startPoint = ImVec2(ptr->getPosition().x, ptr->getPosition().y);
      break;
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
  }
}

void Arena::ImGuiWindow() {
  ImGui::Begin("FORFUN settings");
  {
    ImGui::ColorEdit4("Background color", (float*)&backColor);
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Circles: %ld", circles.size()); ImGui::SameLine();
    ImGui::Text("Walls: %ld", walls.size()); ImGui::SameLine();
    if (ImGui::Button("Clear")) {
      clearArena();
    } ImGui::Separator();
   
    ImGui::Text("CREATION");
    {
      if (ImGui::RadioButton("Ball", shapeType == Circle)) { shapeType = Circle; } ImGui::SameLine();
      if (ImGui::RadioButton("Wall", shapeType == Rectangle)) { shapeType = Rectangle; } ImGui::SameLine();
      if (ImGui::RadioButton("Triangle", shapeType == Triangle)) { shapeType = Triangle; }

      ImGui::ColorEdit4("color ", (float*)&creationParameters.shapeColor); ImGui::SameLine();
      ImGui::Checkbox("random##1", &creationParameters.randomColor);
      if (shapeType == Circle) {
        ImGui::SliderFloat("radius", &creationParameters.radius, 10.0f, 20.0f, "%.2f"); ImGui::SameLine();
        ImGui::Checkbox("random##2", &creationParameters.randomRadius);
        ImGui::SliderFloat("speed ", &creationParameters.speed, 0.0f, 200.0f, "%.2f"); ImGui::SameLine();
        ImGui::Checkbox("random##3", &creationParameters.randomSpeed);
        ImGui::SliderInt("angle ", &creationParameters.angle, 0, 360); ImGui::SameLine();
        ImGui::Checkbox("random##4", &creationParameters.randomAngle);
      } else if (shapeType == Rectangle) {
        ImGui::SliderFloat("width", &creationParameters.radius, 5.0f, 20.0f, "%.1f");
      }

    } ImGui::Separator();

    ImGui::Text("GRAVITY");
    {
      if (ImGui::RadioButton("None", gravityType == NoGravity)) {
        gravityType = NoGravity;
        switchGravity(gravityType);
      } ImGui::SameLine();
      if (ImGui::RadioButton("Earth", gravityType == Earth)) {
        gravityType = Earth;
        switchGravity(gravityType);
      }
    }
  }
  ImGui::End();
}
