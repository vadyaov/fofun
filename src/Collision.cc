#include "Arena.h"

static std::pair<bool, std::shared_ptr<Ball>> BallWallCollision(Arena::circlePtr& c, const Arena::wallPtr& w) {
  sf::Vector2f p = c->getPosition();

  sf::Vector2f s = w->getStartingPoint();
  sf::Vector2f e = w->getEndingPoint();

  sf::Vector2f se(e.x - s.x, e.y - s.y);
  sf::Vector2f sp(p.x - s.x, p.y - s.y);

  float fEdgeLength = w->getSize().x;
  float fRadius = 0.5f * w->getSize().y;

  float t = std::max(0.0f, std::min(fEdgeLength, (se.x * sp.x + se.y * sp.y) / fEdgeLength)) / fEdgeLength;

  sf::Vector2f closestPoint(s.x + t * se.x, s.y + t * se.y);

  float fDistance = sqrtf(powf(p.x - closestPoint.x, 2) + powf(p.y - closestPoint.y, 2));
  
  if (fDistance <= c->getRadius() + fRadius) {
    std::shared_ptr<Ball> fake(new Ball(closestPoint));
    fake->setRadius(fRadius);
    fake->setOrigin(sf::Vector2f(fRadius, fRadius));
    fake->setMass(c->getMass());
    sf::Vector2f v = c->getVelocity();
    fake->setVelocity(sf::Vector2f(-v.x, -v.y));

    float fOverlap = 1.0f * (c->getRadius() + fRadius - fDistance);
    sf::Vector2f direction((p.x - closestPoint.x) / fDistance, (p.y - closestPoint.y) / fDistance);
    c->move(direction.x * fOverlap, direction.y * fOverlap);

    return std::make_pair(true, fake);
  }

  return std::make_pair(false, nullptr);
}

void Arena::staticCollisionProcess() {
  int totalCircles = (int)circles.size();

  // walls collision
  for (auto& c : circles) {
    for (auto& w : walls) {
      auto checkResult = BallWallCollision(c, w);
      if (checkResult.first == true) {
        fake_circles.push_back(checkResult.second);
        collided.push_back(std::make_pair(c, checkResult.second));
      }
    }

    for (auto& [exist, border] : borders) {
      if (exist) {
        auto checkResult = BallWallCollision(c, border);
        if (checkResult.first == true) {
          fake_circles.push_back(checkResult.second);
          collided.push_back(std::make_pair(c, checkResult.second));
        }
      }
    }
  }

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
