#ifndef PLAYABLEOBJECT_H_
#define PLAYABLEOBJECT_H_

#include "PhysicsObject.h"
#include "Clock.h"
#include "debug.h"

class Game;

class PlayableObject : public PhysicsObject
{
public:
    PlayableObject(World &w, btCollisionShape *shape, std::unique_ptr<ManagedDrawableObject> newWeapon)
        : PhysicsObject(w, shape, /*mass*/ 4, /*position*/ {3, 30, 3}), weapon(std::move(newWeapon)),
          forward(false), backward(false), left(false), right(false), up(false)
    {
        body->setAngularFactor(btVector3(0, 0, 0));
        body->setFriction(friction);
        body->setMassProps(0.2f, body->getLocalInertia());
    }

    virtual ~PlayableObject() {}

    void draw(Window *window) const override
    {
        PhysicsObject::draw(window);
        weapon->draw(window);
    }

    void update(float delta) override
    {
        PhysicsObject::update(delta);
        glm::vec4 v;
        body->activate(true);

        btQuaternion q = body->getWorldTransform().getRotation();
        q.setRotation(btVector3(0, 1.f, 0), pitch);
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

        setAlpha(std::pow(1 - zoom, 2));

        aimAngles = getAimAngles();

        weapon->setAlpha(std::pow(1 - zoom, 2));
        weapon->setM(glm::scale(glm::translate(glm::rotate(M, -aimAngles.x - 0.1f, {1.f, 0.f, 0.f}), {0.1f, 0.3f, 1.5f}), {1.f, 1.f, 0.5f}));
        weapon->update(delta);
    }

    void consumeKeyEvent(int glfwKeyCode)
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
    void consumeMouseButtonEvent(Window::MouseButtonEvent ev)
    {
        if (ev.button == Window::MouseButton::right)
            isZooming = ev.down;
        else if (ev.button == Window::MouseButton::left)
        {
            if (ev.down)
                fullPowerClock.reset();
            else if (isReloaded())
                shoot(aimAngles);
            mouseButtonDown = ev.down;
        }
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

    glm::vec3 getRightDirection() const { return glm::vec3(std::cos(pitch), 0.f, -std::sin(pitch)); }

    float getYaw() { return yaw; }
    float getPitch() { return pitch; }
    float getZoom() { return zoom; }

    glm::vec3 getRaycastFront() const { return getRaycast(getFrontDirection()); }

    float getReloadState() const { return reloadClock.getTime() / reloadTime; }
    float getFullPowerState() const { return mouseButtonDown ? fullPowerClock.getTime() / fullPowerTime : 0.f; }

    float getHP() const { return hp; }
    void setHP(float HP) { hp = HP; }
    static constexpr inline float maxHP = 100;

    void damage(float damage)
    {
        hp -= damage;
        std::cerr << this << " hit for " << damage << ", " << hp << "HP left" << std::endl;
        if (hp <= 0)
            kill();
    }

protected:
    friend class Crosshair;
    static constexpr float friction = 0.1f;
    std::unique_ptr<ManagedDrawableObject> weapon;
    virtual glm::vec3 getAimCoords() const { return getRaycastFront(); }

    bool forward = false, backward = false, left = false, right = false, up = false, isZooming = false;
    float yaw = glm::pi<float>() / 2.f, pitch = 0.f, theta = 0.f;
    float zoom = 0;
    float jumpCooldownRemaining = 0.f;
    glm::vec3 frontDirection;

    void goForward(bool x) { forward = x; }
    void goBackward(bool x) { backward = x; }
    void goLeft(bool x) { left = x; }
    void goRight(bool x) { right = x; }
    void goUp(bool x) { up = x; }

    bool isReloaded() const { return reloadClock.getTime() >= reloadTime; }
    glm::vec3 getInitialBulletPosition() const { return getPosition() + getFrontDirection() * bulletSpawnDistance; }
    glm::vec3 getAimAngles() const;
    glm::vec3 aimAngles = {0.f, 0.f, 0.f};
    glm::vec3 getAimDirection(const glm::vec3 &angles) const
    {
        return glm::normalize(glm::vec3(
            cos(angles.x) * sin(angles.y),
            sin(angles.x),
            cos(angles.x) * cos(angles.y)));
    }
    void shoot(const glm::vec3 &angles);

private:
    float getBulletImpulse() const { return (1.f + getFullPowerState()) * initialBulletImpulse; }

    Clock reloadClock;
    Clock fullPowerClock;
    bool mouseButtonDown = false;
    float hp = maxHP;

    static constexpr float speed = 3.f, sideSpeed = 1.f, jumpSpeed = 15.f;
    static constexpr float maxSpeed = 10.f;
    static constexpr float mouseSensitivity = 0.004f;
    static constexpr float zoomTime = 0.3f;
    static constexpr float jumpCooldown = 1.86f;
    static constexpr float initialBulletImpulse = 30.f;
    static constexpr float bulletSpawnDistance = 0.5f;
    static constexpr float reloadTime = 2.f;
    static constexpr float fullPowerTime = 1.f;
};
#endif /* !PLAYABLEOBJECT_H_ */
