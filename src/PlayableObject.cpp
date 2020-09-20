#include "PlayableObject.h"

#include "Game.h"

glm::vec3 PlayableObject::getAimAngles() const
{
    const glm::vec3 b = getAimCoords();
    if (std::isnan(b.x) || std::isnan(b.y) || std::isnan(b.z))
        return {-yaw + glm::pi<float>() / 2.f, pitch, yaw};
    else
    {
        const glm::vec3 a = getInitialBulletPosition();

        const float d = glm::distance(glm::vec2(a.x, a.z), glm::vec2(b.x, b.z));
        const float aimPitch = glm::orientedAngle(glm::normalize(glm::vec2(a.x - b.x, a.z - b.z)), glm::vec2(0.f, -1.f));
        const float aimYaw = glm::orientedAngle(glm::vec2(0.f, -1.f), glm::normalize(glm::vec2(d, a.y - b.y)));

        const float v = getBulletImpulse() / Bullet::mass;
        const float v2 = v * v;
        const float g = -World::g;
        const float y = b.y - a.y;

        // https://www.forrestthewoods.com/blog/solving_ballistic_trajectories/

        const float theta = std::atan((v2 - std::sqrt(v2 * v2 - g * (g * d * d + 2.f * v2 * y))) / (g * d));

        return {theta, aimPitch, aimYaw};
    }
}

void PlayableObject::shoot(const glm::vec3 &angles)
{
    if (!isReloaded())
        return;
    reloadClock.reset();

    btTransform transform;

    transform.setOrigin(toBtVec3(getInitialBulletPosition()));
    transform.setRotation(btQuaternion(angles.y, angles.z, 0.f));

    Game::addDrawable(std::make_unique<Bullet>(*world, *this, transform, getAimDirection(angles) * getBulletImpulse()));
    MusicManager::get("launch.wav")
        .setVolumeMultiplier(Music::volumeMultiplier(5.f, Game::getCameraPosition() - getPosition()))
        .play();
    MusicManager::get("reload.wav")
        .setVolumeMultiplier(4.f * Music::volumeMultiplier(5.f, Game::getCameraPosition() - getPosition()))
        .playDelayed(reloadTime - 1.f);
}
