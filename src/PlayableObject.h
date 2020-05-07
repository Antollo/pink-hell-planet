#ifndef PLAYABLEOBJECT_H_
#define PLAYABLEOBJECT_H_

#include "PhysicsObject.h"

class PlayableObject : public PhysicsObject
{
public:
    PlayableObject(World &w, btCollisionShape *shape) : PhysicsObject(w, shape, /*mass*/4, /*position*/{3, 20, 3}), 
        forward(false), backward(false), left(false), right(false), up(false) {}

    void update(float delta) override
    {
        PhysicsObject::update(delta);
        glm::vec3 v;
        body->activate(true);
        if (forward)
        {
            v = (M * glm::vec4(0.f, 0.f, 1.f, 0.f)) * 20.f * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }
        if (backward)
        {
            v = (M * glm::vec4(0.f, 0.f, -1.f, 0.f)) * 20.f * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }
        if (left)
        {
            v = (M * glm::vec4(0.f, 1.f, 0.f, 0.f)) * 5.f * delta;
            body->applyTorqueImpulse(btVector3(v.x, v.y, v.z));
        }
        if (right)
        {
            v = (M * glm::vec4(0.f, -1.f, 0.f, 0.f)) * 5.f * delta;
            body->applyTorqueImpulse(btVector3(v.x, v.y, v.z));
        }
        if (up)
        {
            // std::cout << "up\n";
            v = (M * glm::vec4(0.f, 1.f, 0.f, 0.f)) * 50.f * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }
    }
    void goForward(bool x) { forward = x; }
    void goBackward(bool x) { backward = x; }
    void goLeft(bool x) { left = x; }
    void goRight(bool x) { right = x; }
    void goUp(bool x) { up = x; }

protected:
    bool forward, backward, left, right, up;
};
#endif /* !PLAYABLEOBJECT_H_ */
