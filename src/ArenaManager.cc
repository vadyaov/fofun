#include "ArenaManager.h"

sf::Color getRandomColor() {
  static std::vector<sf::Color> colors = {sf::Color::Blue, sf::Color::Cyan, sf::Color::Magenta,
                                          sf::Color::Yellow, sf::Color::Green, sf::Color::Red,
                                          sf::Color::White};
  return colors[std::rand() % colors.size()];
}


void ArenaManager::addBall(sf::Vector2f pos) {
  balls.push_back(std::make_shared<Ball>(pos));

  auto& lastBall = balls.back();
  if (creationParameters.randomColor == false) {
    lastBall->setFillColor(creationParameters.shapeColor);
  }

  if (creationParameters.randomRadius == false) {
    lastBall->setRadius(creationParameters.radius);
    lastBall->setOrigin(sf::Vector2f(creationParameters.radius, creationParameters.radius));
    lastBall->setMass(creationParameters.radius * 10.0f);
  }

  if (creationParameters.randomSpeed == false) {
    sf::Vector2f currentVelo = lastBall->getVelocity();
    float spd = sqrtf(powf(currentVelo.x, 2) + powf(currentVelo.y, 2));
    sf::Vector2f direct(currentVelo.x / spd, currentVelo.y / spd);

    lastBall->setVelocity(sf::Vector2f(creationParameters.speed * direct.x, creationParameters.speed * direct.y));
  }

  if (creationParameters.randomAngle == false) {
    // set Angle
    float radians = creationParameters.angle * 180.0f / M_PI;
    sf::Vector2f currentVelo = lastBall->getVelocity();
    float spd = sqrtf(powf(currentVelo.x, 2) + powf(currentVelo.y, 2));

    lastBall->setVelocity(sf::Vector2f(spd * cos(radians), spd * sin(radians)));
  }

  lastBall->setAcceleration(gravityType == NoGravity ? sf::Vector2f(0.0f, 0.0f) : sf::Vector2f(0.0f, 100.0f));
}

void ArenaManager::addWall() {
  wallPtr newWall(new Wall(sf::Vector2f(Line.startPoint.x, Line.startPoint.y), 
                           sf::Vector2f(Line.endPoint.x, Line.endPoint.y),
                           creationParameters.radius, creationParameters.shapeColor));
    if (creationParameters.randomColor) {
      newWall->changeColor(getRandomColor());
    }
    walls.push_back(newWall);

    Line.startPoint = {};
    Line.endPoint = {};

    buildingWall = false;
}

  // changing gravity
void ArenaManager::switchGravity(int t) {
  sf::Vector2f acc{0.0f, 0.0f};
  if (t == Earth) acc.y = 100.0f;

  for (auto& ptr : balls) {
    ptr->setAcceleration(acc);
  }
}
//
// update arena with delta time
void ArenaManager::update(sf::Time elapsed) {
  const int iElapsedTime = 4;
  const int nMaxSimulationSteps = 15;

  collisionLines.clear();

  for (int i = 0; i != iElapsedTime; ++i) {

    for (auto& p : balls) {
      p->fSimTimeRemaining = elapsed / (float)iElapsedTime;
    }

    for (int j = 0; j != nMaxSimulationSteps; ++j) {

      for (auto& p : balls) {
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
      for (auto& p : balls) {
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
      for (auto& ptr : balls) {
        sf::Vector2f velocity = ptr->getVelocity();
        float speed = sqrtf(powf(velocity.x, 2) + powf(velocity.y, 2));
        totalEnergy += ptr->getMass() * speed;
      }

      collided.clear();
      fake_balls.clear();

    }
  }

}

// try to select ball
void ArenaManager::select(sf::Vector2f pos) {
  for (auto& ball : balls) {
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

    } else if (wall->bodyContains(pos)) {
      wallSelection.selectedWall = wall;
      wallSelection.all = true;

      wallSelection.oldMouse = pos;
      return;
    }
  }
}

// unselect ball and give it velocity in direction
void ArenaManager::unselect() {
  if (selected) {
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
    wallSelection.all = false;
  }
}

void ArenaManager::drawSelectionLine(const sf::Vector2f& mousePos) {
  if (selected || buildingWall) {
    Line.endPoint = ImVec2(mousePos.x, mousePos.y);

    if (selected) {
      selected->setPosition(Line.startPoint);

    // Draw a line between the button and the mouse cursor
      ImGui::GetForegroundDrawList()->AddLine(Line.startPoint, Line.endPoint,
                                              ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
    } else {
      ImGui::GetForegroundDrawList()->AddLine(Line.startPoint, Line.endPoint,
                                              ImGui::GetColorU32(ImGuiCol_Button), creationParameters.radius);
    }
  }
}

void ArenaManager::moveSelectedWall(const sf::Vector2f& mousePos) {
  if (wallSelection.first) {
    wallSelection.selectedWall->setStartingPoint(mousePos);
  } else if (wallSelection.second) {
    wallSelection.selectedWall->setEndingPoint(mousePos);
  } else if (wallSelection.all) {
    if (mousePos != wallSelection.oldMouse) {
      sf::Vector2f direct(mousePos.x - wallSelection.oldMouse.x,
                          mousePos.y - wallSelection.oldMouse.y);
      float fDistance = sqrtf(powf(direct.x, 2) + powf(direct.y, 2));
      sf::Vector2f unitDir(direct.x / fDistance, direct.y / fDistance);

      wallSelection.selectedWall->move(unitDir, fDistance);
    }
    wallSelection.oldMouse = mousePos;
  }
}

void ArenaManager::eraseObject(const sf::Vector2f& mousePos) {
  auto ball_it = std::find_if(balls.begin(), balls.end(), [&](const ballPtr& ball) {
      return ball->getGlobalBounds().contains(mousePos);
      });

  if (ball_it != balls.end()) {
    balls.erase(ball_it);
    return;
  }

  auto wall_it = std::find_if(walls.begin(), walls.end(), [&](const wallPtr& wall) {
      return wall->firstCornerContains(mousePos) || wall->secondCornerContains(mousePos) ||
             wall->bodyContains(mousePos);
             });

  if (wall_it != walls.end()) {
    walls.erase(wall_it);
  }
}
