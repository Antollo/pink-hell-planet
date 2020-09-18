#include "PlayableObject.h"

#include "Game.h"

void PlayableObject::shoot()
{
    if (reloadClock.getTime() < reloadTime)
        return;
    reloadClock.reset();

    glm::vec3 dir;

    glm::vec3 a = getPosition() + getFrontDirection() * bulletSpawnDistance;
    glm::vec3 b = getRaycastAim();

    if (std::isnan(b.x) || std::isnan(b.y) || std::isnan(b.z))
        dir = getFrontDirection();
    else
    {
        float v = bulletImpulse / Bullet::mass;
        float v2 = v * v;
        float d = glm::distance(glm::vec2(a.x, a.z), glm::vec2(b.x, b.z));
        float g = -World::g;
        float y = b.y - a.y;

        // https://www.forrestthewoods.com/blog/solving_ballistic_trajectories/

        float theta = std::atan((v2 - std::sqrt(v2 * v2 - g * (g * d * d + 2.f * v2 * y))) / (g * d));

        dir.z = cos(theta) * cos(pitch);
        dir.x = cos(theta) * sin(pitch);
        dir.y = sin(theta);

        dir = glm::normalize(dir);
    }

    btTransform transform;

    transform.setOrigin(toBtVec3(a));
    transform.setRotation(btQuaternion(pitch, yaw, 0.f));

    Game::addDrawable(std::make_unique<Bullet>(*world, *this, transform, dir * bulletImpulse));
    MusicManager::get("launch.wav")
        .setVolumeMultiplier(Music::volumeMultiplier(5.f, Game::getCameraPosition() - getPosition()))
        .play();
}
