#ifndef _ARENA_MANAGER_H_
#define _ARENA_MANAGER_H_

#include <memory>

#include "Ball.h"
#include "Border.h"
#include "Source.h"
#include "Sound.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Audio/Music.hpp>

using ballPtr = std::shared_ptr<Ball>;
using wallPtr = std::shared_ptr<Wall>;
using ballPair = std::pair<ballPtr, ballPtr>;
using sourcePtr = std::shared_ptr<Source>;
using std::vector;

class ArenaManager {
  public:
    enum ShapeType {NoShape, SPHERE, WALL, SOURCE};
    enum GravityType {NoGravity, Earth};

    explicit ArenaManager(sf::Vector2u sz) : size{sz}, borders {
      std::make_pair(false, std::make_shared<Wall>(TopBorder(size))),
      std::make_pair(false, std::make_shared<Wall>(BotBorder(size))),
      std::make_pair(false, std::make_shared<Wall>(LeftBorder(size))),
      std::make_pair(false, std::make_shared<Wall>(RightBorder(size)))
    } {
      fs::path curr = fs::current_path();
      curr += "/sound";
      sounds = std::make_shared<Sound>(curr);
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

    void createSource(sf::Vector2f pos);

    void drawSelectionLine(const sf::Vector2f& mousePos);
    void moveSelectedWall(const sf::Vector2f& mousePos);
    void eraseObject(const sf::Vector2f& mousePos);

    virtual ~ArenaManager() = default;

  protected:
    sf::Vector2u size; // window size

    vector<ballPtr> balls;
    vector<wallPtr> walls;

    vector<ballPair> collided; // collision pairs
    ballPtr selected; // one ball selection

    vector<ballPtr> fake_balls; // fake balls to resolve collisions with edges

    vector<sf::VertexArray> collisionLines; // lines of ball-ball collision

    vector<sourcePtr> sources;

    std::array<std::pair<bool, wallPtr>, 4> borders;

    struct Creation {
      ImVec4 shapeColor {1.0f, 0.0f, 0.0f, 1.0f}; bool randomColor = false;
      float radius = 10.0f;                       bool randomRadius = false;
      float speed = 0.0f;                         bool randomSpeed = false;
      int angle = 0;                              bool randomAngle = false;

      float spawnPeriod = 1.0f; // seconds
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

    std::shared_ptr<Sound> sounds;
    sf::Music music;
};

#endif // _ARENA_MANAGER_H_
