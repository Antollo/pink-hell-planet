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
    void consumeButton(int glfwButtonCode)
    {
        if (abs(glfwButtonCode) == GLFW_MOUSE_BUTTON_2)
            isZooming = glfwButtonCode > 0;
    }
    void consumeCursorDiff(float xCursorDiff, float yCursorDiff)
    {
        yaw += yCursorDiff * mouseSensitivity;
        pitch -= xCursorDiff * mouseSensitivity;
    }

    float getYaw() { return yaw; }
    float getPitch() { return pitch; }
    float getZoom() { return zoom; }

protected:
    friend class Crosshair;
    static constexpr float friction = 0.1f;
    float getAlpha() const override { return std::pow(1 - zoom, 2); }

    bool forward = false, backward = false, left = false, right = false, up = false, isZooming = false;
    float yaw = 0, pitch = 0;
    float positionPitch = 0;
    float zoom = 0;
    float jumpCooldownRemaining = 0.f;

    void goForward(bool x) { forward = x; }
    void goBackward(bool x) { backward = x; }
    void goLeft(bool x) { left = x; }
    void goRight(bool x) { right = x; }
    void goUp(bool x) { up = x; }

private:
    static inline constexpr float speed = 3.f, sideSpeed = 1.f, jumpSpeed = 15.f;
    static inline constexpr float maxSpeed = 10.f;
    static constexpr float mouseSensitivity = 0.004f;
    static inline constexpr float zoomTime = 0.3f;
    static inline constexpr float jumpCooldown = 1.86f;
};
#endif /* !PLAYABLEOBJECT_H_ */
