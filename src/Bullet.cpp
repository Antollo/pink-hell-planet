
#include "Bullet.h"

#include <algorithm>

#include "PlayableObject.h"
#include "Game.h"

void Bullet::explode()
{
    ghostObject.setPositionNoRotation(getPosition());

    auto colliding = world.getColliding(ghostObject);
    for (auto ptr : colliding)
    {
        PlayableObject* playable = dynamic_cast<PlayableObject*>(ptr);
        if (playable != nullptr)
        {
            float distance = glm::distance(getPosition(), playable->getPosition());
            static constexpr float falloff = explosionRadius - fullDamageRadius;
            float damage = damageBase * std::clamp(1 - (distance - fullDamageRadius) / falloff, 0.f, 1.f);
            playable->damage(damage);
        }
    }

    Game::getTerrain()->collideWith(ghostObject);
    Game::getParticleSystem()->generate(getPosition());
}
