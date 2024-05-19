#include "Arena.h"

#include <iostream>

// ctors
Arena::Arena(sf::VideoMode mode) : sf::RenderWindow(mode, "fofun", sf::Style::Fullscreen),
                                   ArenaManager(getSize()) {
  setFramerateLimit(144);
  if (!ImGui::SFML::Init(*this)) // imgui
  {
    throw std::runtime_error("ImGui initialization failed");
  }
}

/* Arena::Arena(sf::VideoMode mode, int n) : Arena(mode) { */
/*   balls.reserve(n); */
/*   while (n--) { */
/*     sf::Vector2f randomPos(std::rand() % size.x, std::rand() % size.y); */
/*     balls.push_back(std::make_shared<Ball>(randomPos)); */
/*   } */
/* } */

Arena::Arena(int width, int height) : sf::RenderWindow(sf::VideoMode(width, height), "", sf::Style::Default), ArenaManager(getSize()) {
  std::cout << "CONSTRUCTED!\n";
  setFramerateLimit(144);
  if (!ImGui::SFML::Init(*this)) // imgui
  {
    throw std::runtime_error("ImGui initialization failed");
  }
}

/* Arena::Arena(int width, int height, int n) : Arena(width, height) { */
/*   balls.reserve(n); */
/*   while (n--) { */
/*     sf::Vector2f randomPos(std::rand() % size.x, std::rand() % size.y); */
/*     balls.push_back(std::make_shared<Ball>(randomPos)); */
/*   } */
/* } */

// main loop
void Arena::run() {
  while (isOpen()) {
    sf::Event event;
    pollEvents(event);

    sf::Time elapsedTime = clock.restart();
    ImGui::SFML::Update(*this, elapsedTime);

    if (!pause)
      ArenaManager::update(elapsedTime); // ArenaManager

    sf::Vector2f mousePos{sf::Mouse::getPosition(*this)};
    if (selected || buildingWall) {
      Line.endPoint = ImVec2(mousePos.x, mousePos.y);

      if (selected)
        selected->setPosition(Line.startPoint);

      // Draw a line between the button and the mouse cursor
      ImGui::GetForegroundDrawList()->AddLine(Line.startPoint, Line.endPoint,
                                              ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
    }

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

    ImguiWindow();

    /* ImGui::ShowDemoWindow(); */

    sf::RenderWindow::clear(backColor);

    for (auto& ball : balls) {
      draw(*ball);
    }

    if (showCollisions) {
      for (auto& line : collisionLines) {
        draw(line);
      }
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
          case SPHERE:
            if (!selected)
              addBall(static_cast<sf::Vector2f>(mousePos));
            break;
          case WALL:
            buildingWall = true;
            Line.startPoint = ImVec2(mousePos.x, mousePos.y);
            break;
        }
      } else if (mouseEvent.button == sf::Mouse::Button::Right && !selected) {
        select(static_cast<sf::Vector2f>(mousePos));
      }
    }

    if (tp == sf::Event::MouseButtonReleased) {
      auto mousePos = sf::Mouse::getPosition(*this);
      if (event.mouseButton.button == sf::Mouse::Button::Left && buildingWall) {
        Line.endPoint = ImVec2(mousePos.x, mousePos.y);
        addWall();
      } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
        unselect();
      }
    }

  }
}
