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

    // creating scene with existing videmode (for full screen)
    Arena(sf::VideoMode mode);
    Arena(sf::VideoMode mode, int n);

    // creating scene with screen input params
    Arena(int width, int height);
    Arena(int width, int height, int n);

    // main loop
    void run();

  private:
    void pollEvents(sf::Event& event);   // event processing
    void addCircle(sf::Vector2f pos);    // add new circle
    void staticCollisionProcess();       // static collision
    void dynamicCollisionProcess();      // dynamic collision
    void switchGravity(int t);           // changing gravity
    void updateArena(sf::Time elapsed);  // update arena with delta time
    void clearArena() noexcept;          // remove all objects
    bool isSelected() const noexcept;    // check if any circle is selected
    void select(sf::Vector2f pos);       // try to select circle
    void unselect();                     // unselect circle and give it velocity in direction
    void ImGuiWindow();                  // gui


  private:
    vector<circlePtr> circles; // want it to be some ShapePtr, where shape is circle or rect or triangle and so on
    vector<circlePair> collided;
    circlePtr selected;

    vector<sf::VertexArray> collisionLines;

    vector<wallPtr> walls;
    bool buildingWall = false;

    struct {
      ImVec2 startPoint;
      ImVec2 endPoint;
    } Line;

    sf::Vector2u size;

    sf::Clock clock;

    ImVec4 backColor {0.15f, 0.15f, 0.15f, 0.0f}; // background color

    struct {
      ImVec4 shapeColor {1.0f, 0.0f, 0.0f, 1.0f}; bool randomColor = false;
      float radius = 10.0f;                       bool randomRadius = false;
      float speed = 0.0f;                         bool randomSpeed = false;
      int angle = 0;                              bool randomAngle = false;

    } creationParameters;

    int shapeType = NoShape;
    int gravityType = NoGravity;
};

#endif // ARENA_H_
