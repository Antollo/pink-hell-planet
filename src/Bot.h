#ifndef BOT_H_
#define BOT_H_

#include <limits>
#include <set>
#include <functional>
#include <memory>
#include <algorithm>
#include "DummyModel.h"
#include "Bullet.h"

class Bot : public DummyModel
{
public:
    Bot(World &world)
        : DummyModel(world) { yaw = 0.f; }
    void update(float delta) override
    {
        glm::vec3 aimCoords(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

        float distance = std::numeric_limits<float>::max(), newDistance;

        targets.erase(std::remove_if(targets.begin(), targets.end(), [](const auto &x) {
                          return x.expired();
                      }),
                      targets.end());

        for (const auto &targetPtr : targets)
        {
            auto target = targetPtr.lock();
            if ((newDistance = glm::distance(getPosition(), target->getPosition())) <= distance)
            {
                aimCoords = target->getPosition();
                distance = newDistance;
            }
        }

        if (aimCoords != glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()))
        {
            float aimAngle = glm::orientedAngle(glm::vec2(1.f, 0.f), glm::normalize(glm::vec2(aimCoords.z - getPosition().z, -aimCoords.x + getPosition().x))) - getOrientationY();
            while (aimAngle > pi)
                aimAngle -= 2.f * pi;
            while (aimAngle < -pi)
                aimAngle += 2.f * pi;

            goUp(aimCoords.y > getPosition().y + 1.f);
            body->setFriction(friction);

            if (aimAngle < -0.2f)
            {
                goLeft(distance > fireDistance);
                goRight(false);
                goForward(false);
                consumeCursorDiff(500.0f * delta * aimAngle, 0.f);
            }
            else if (aimAngle > 0.2f)
            {
                goLeft(false);
                goRight(distance > fireDistance);
                goForward(false);
                consumeCursorDiff(500.0f * delta * aimAngle, 0.f);
            }
            else
            {
                goLeft(false);
                goRight(false);
                if (distance > fireDistance)
                    goForward(true);
                else
                {
                    goForward(false);
                    body->setFriction(1.5f);

                    static int i = 0;

                    if (i == 0 && isReloaded())
                    {
                        glm::vec3 angles = getAimAngles(aimCoords), aimDirection = getAimDirection(angles);
                        glm::vec3 v1 = getRaycast(getInitialBulletPosition() + glm::vec3(0.f, 1.f, 0.f), aimDirection);
                        glm::vec3 v2 = getRaycast(getInitialBulletPosition() - glm::vec3(0.f, 1.f, 0.f), aimDirection);

                        //std::cout << glm::distance(v1, getPosition()) << " " << glm::distance(v2, getPosition()) << std::endl;
                        if ((std::isnan(v1.x) || glm::distance(v1, getPosition()) > Bullet::explosionRadius * 2.f) &&
                            (std::isnan(v2.x) || glm::distance(v2, getPosition()) > Bullet::explosionRadius * 2.f))
                            shoot(angles);
                        else
                            goUp(true);
                    }
                    i++;
                    if (i > 10)
                        i = 0;
                }
            }
        }
        else
        {
            goLeft(false);
            goRight(false);
            goForward(false);
        }
        DummyModel::update(delta);
    }
    void target(const std::shared_ptr<PlayableObject> &target)
    {
        targets.push_back(target);
    }

private:
    static constexpr float fireDistance = 80.f;
    std::vector<std::weak_ptr<PlayableObject>> targets;
};

#endif /* !BOT_H_ */
