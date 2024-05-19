#pragma once

#include <memory>

#include "Ball.h"
#include "Border.h"

#include "imgui.h"

using ballPtr = std::shared_ptr<Ball>;
using wallPtr = std::shared_ptr<Wall>;
using ballPair = std::pair<ballPtr, ballPtr>;

using std::vector;

class ArenaManager {
  public:
    enum ShapeType {NoShape, SPHERE, WALL};
    enum GravityType {NoGravity, Earth};

    explicit ArenaManager(sf::Vector2u sz) : size{sz}, borders {
      std::make_pair(false, std::make_shared<Wall>(TopBorder(size))),
      std::make_pair(false, std::make_shared<Wall>(BotBorder(size))),
      std::make_pair(false, std::make_shared<Wall>(LeftBorder(size))),
      std::make_pair(false, std::make_shared<Wall>(RightBorder(size)))
    } {
      // init smth
    }

  protected:
    void addBall(sf::Vector2f pos);      // add new ball
    void addWall();
    void staticCollisionProcess();       // static collision
    void dynamicCollisionProcess();      // dynamic collision
    void switchGravity(int t);           // changing gravity
    void update(sf::Time elapsed);
    void select(sf::Vector2f pos);       // try to select ball
    void unselect();                     // unselect ball and give it velocity in direction

    virtual ~ArenaManager() = default;

  protected:
    sf::Vector2u size; // window size

    vector<ballPtr> balls;
    vector<wallPtr> walls;

    vector<ballPair> collided; // collision pairs
    ballPtr selected; // one ball selection

    vector<ballPtr> fake_balls; // fake balls to resolve collisions with edges

    vector<sf::VertexArray> collisionLines; // lines of ball-ball collision

    std::array<std::pair<bool, wallPtr>, 4> borders;

    struct {
      ImVec4 shapeColor {1.0f, 0.0f, 0.0f, 1.0f}; bool randomColor = false;
      float radius = 10.0f;                       bool randomRadius = false;
      float speed = 0.0f;                         bool randomSpeed = false;
      int angle = 0;                              bool randomAngle = false;

    } creationParameters;

    struct BuildLine {
      ImVec2 startPoint;
      ImVec2 endPoint;
    } Line;

    struct {
      wallPtr selectedWall;
      bool first = false;
      bool second = false;
      bool all = false;
      sf::Vector2f oldMouse;
    } wallSelection;

    bool buildingWall = false;

    ImVec4 backColor {0.15f, 0.15f, 0.15f, 0.0f}; // background color

    int shapeType = NoShape;
    int gravityType = NoGravity;

    bool showCollisions = false;

    float totalEnergy{};
};
