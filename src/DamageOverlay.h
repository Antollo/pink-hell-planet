#ifndef C791185A_EAA7_47AF_BFFF_A6F529D95970
#define C791185A_EAA7_47AF_BFFF_A6F529D95970

#include "ShapeArray.h"
#include "PlayableObject.h"

class DamageOverlay : public ShapeArray
{
public:
    DamageOverlay(PlayableObject *&p)
        : ShapeArray(ShapeArray::Type::triangles), player(p),
          lastHp(0.f), active(false)
    {
        insert(end(), {

                          {-1.f, 1.f},
                          {-1.f, -1.f},
                          {1.f, 1.f},

                          {-1.f, -1.f},
                          {1.f, -1.f},
                          {1.f, 1.f}

                      });
        setColor({1.f, 0.561f, 0.933f, 0.f});
    }

    void update(float delta) override
    {
        setScale(1.f / getGlobalScale());
        ShapeArray::update(delta);
        alpha = 0.f;
        if (player != nullptr)
        {
            active = true;
            if (player->getHP() < lastHp)
                damageClock.reset();
            lastHp = player->getHP();
        }
        alpha = 0.3f * std::max(t - std::pow(damageClock.getTime(), 2.f) / t, 0.f);
        if (active)
            setAlpha(alpha);
    }

private:
    static constexpr float t = 2.f;
    PlayableObject *&player;
    Clock damageClock;
    float lastHp, alpha;
    bool active;
};

#endif /* C791185A_EAA7_47AF_BFFF_A6F529D95970 */
