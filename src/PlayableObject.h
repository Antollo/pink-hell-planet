#ifndef PLAYABLEOBJECT_H_
#define PLAYABLEOBJECT_H_

#include "PhysicsObject.h"
#include "debug.h"

class PlayableObject : public PhysicsObject
{
public:
    PlayableObject(World &w, btCollisionShape *shape) : PhysicsObject(w, shape, /*mass*/ 4, /*position*/ {3, 30, 3}),
                                                        forward(false), backward(false), left(false), right(false), up(false)
    {
        body->setAngularFactor(btVector3(0, 0, 0));
        body->setFriction(friction); // TODO: find better solution?
        body->setMassProps(0.2f, body->getLocalInertia());
    }

    void update(float delta) override
    {
        PhysicsObject::update(delta);
        glm::vec4 v;
        body->activate(true);

        btQuaternion q = body->getWorldTransform().getRotation();
        q.setRotation(btVector3(0, 1.f, 0), pitch - positionPitch);
        body->getWorldTransform().setRotation(q);

        if (forward)
        {
            v = (M * glm::vec4(0.f, 0.f, 1.f, 0.f)) * speed * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }
        if (backward)
        {
            v = (M * glm::vec4(0.f, 0.f, -1.f, 0.f)) * speed * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }
        if (left)
        {
            v = (M * glm::vec4(1.f, 0.f, 0.f, 0.f)) * speed * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }
        if (right)
        {
            v = (M * glm::vec4(-1.f, 0.f, 0.f, 0.f)) * speed * delta;
            body->applyCentralImpulse(btVector3(v.x, v.y, v.z));
        }

        btVector3 velocity = body->getLinearVelocity();
        float ySpeed = velocity.getY();
        btVector3 xzSpeed = velocity - ySpeed * btVector3(0.f, 1.f, 0.f);
        float speedLen = xzSpeed.length();
        if (speedLen > maxSpeed)
            xzSpeed *= maxSpeed / speedLen;

        jumpCooldownRemaining -= delta;
        if (up && jumpCooldownRemaining <= 0)
        {
            ySpeed = jumpSpeed;
            jumpCooldownRemaining = jumpCooldown;
        }

        body->setLinearVelocity(xzSpeed + ySpeed * btVector3(0.f, 1.f, 0.f));

        zoom += (isZooming ? 1 : -1) * delta / zoomTime;
        if (zoom < 0)
            zoom = 0;
        if (zoom > 1)
            zoom = 1;

        auto it = bullets.begin();
        while (it != bullets.end())
        {
            (*it)->update(delta);
            if (!(*it)->isAlive())
                it = bullets.erase(it);
            else
                it++;
        }
    }

    void draw(Window *window) const override
    {
        for (auto &bullet : bullets)
            bullet->draw(window);

        PhysicsObject::draw(window);
    }

    void consumeKey(int glfwKeyCode)
    {
        bool pressed = glfwKeyCode > 0;
        switch (abs(glfwKeyCode))
        {
        case GLFW_KEY_W:
            goForward(pressed);
            break;
        case GLFW_KEY_S:
            goBackward(pressed);
            break;

        case GLFW_KEY_A:
            goLeft(pressed);
            break;
        case GLFW_KEY_D:
            goRight(pressed);
            break;

        case GLFW_KEY_SPACE:
            goUp(pressed);
            break;
        }
    }
    void consumeButton(Window::MouseButtonEvent ev)
    {
        if (ev.button == Window::MouseButton::right)
            isZooming = ev.down;
        else if (ev.button == Window::MouseButton::left && ev.down)
            shoot();
    }
    void consumeCursorDiff(float xCursorDiff, float yCursorDiff)
    {
        yaw += yCursorDiff * mouseSensitivity;
        pitch -= xCursorDiff * mouseSensitivity;

        frontDirection.z = sin(yaw) * cos(pitch);
        frontDirection.x = sin(yaw) * sin(pitch);
        frontDirection.y = cos(yaw);

        frontDirection = glm::normalize(frontDirection);
    }

    glm::vec3 getFrontDirection() const { return frontDirection; }

    float getYaw() { return yaw; }
    float getPitch() { return pitch; }
    float getZoom() { return zoom; }

    glm::vec3 getRaycastAim() const
    {
        auto result = world->getRaycastResult(getPosition() + getFrontDirection(), getFrontDirection() * 1000.f);
        return result->hasHit() ? toGlmVec3(result->m_hitPointWorld) : glm::vec3(NAN, NAN, NAN);
    }

protected:
    friend class Crosshair;
    static constexpr float friction = 0.1f;
    float getAlpha() const override { return std::pow(1 - zoom, 2); }

    bool forward = false, backward = false, left = false, right = false, up = false, isZooming = false;
    float yaw = 0, pitch = 0;
    float positionPitch = 0;
    float zoom = 0;
    float jumpCooldownRemaining = 0.f;
    glm::vec3 frontDirection;
    std::vector<std::unique_ptr<Bullet>> bullets;

    void goForward(bool x) { forward = x; }
    void goBackward(bool x) { backward = x; }
    void goLeft(bool x) { left = x; }
    void goRight(bool x) { right = x; }
    void goUp(bool x) { up = x; }

    void shoot()
    {
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
            float g = World::g;
            float y = b.y - a.y;

            // https://www.forrestthewoods.com/blog/solving_ballistic_trajectories/
            // TODO: fix it

            float theta = std::atan((v2 - std::sqrt(v2 * v2 - g * (g * d * d + 2.f * v2 * y))) / (g * d));

            //std::cout << "t " << theta << std::endl;
            //std::cout << "y " << pi / 2 - yaw << std::endl;

            dir.z = cos(theta) * cos(pitch);
            dir.x = cos(theta) * sin(pitch);
            dir.y = sin(theta);

            dir = glm::normalize(dir);
        }

        btTransform transform;

        transform.setOrigin(toBtVec3(a));
        transform.setRotation(btQuaternion(pitch, yaw, 0.f));

        bullets.emplace_back(new Bullet(*world));
        bullets.back()->setTransform(transform);
        bullets.back()->applyCentralImpulse(dir * bulletImpulse);
    }

private:
    static constexpr float speed = 3.f, sideSpeed = 1.f, jumpSpeed = 15.f;
    static constexpr float maxSpeed = 10.f;
    static constexpr float mouseSensitivity = 0.004f;
    static constexpr float zoomTime = 0.3f;
    static constexpr float jumpCooldown = 1.86f;
    static constexpr float bulletImpulse = 60.f;
    static constexpr float bulletSpawnDistance = 3.f;
};
#endif /* !PLAYABLEOBJECT_H_ */
