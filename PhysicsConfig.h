#ifndef PHYSICSCONFIG_H
#define PHYSICSCONFIG_H

class PhysicsConfig {
private:
    float acceleration;
    float max_speed;
    float friction;
    float deceleration;
    float gravity;
    float terminalVelocity;
    float jumpStrength;
public:
    PhysicsConfig(float acc = 0.2f, float maxSpd = 15.0f, float fric = 0.1f, float decel = 0.2f, float grav = 0.98f, float termVel = 20.0f, float jumpStr = -20.0f)
        : acceleration(acc), max_speed(maxSpd), friction(fric), deceleration(decel), gravity(grav), terminalVelocity(termVel), jumpStrength(jumpStr) {}

    // Getters
    float getAcceleration() const { return acceleration; }
    float getMaxSpeed() const { return max_speed; }
    float getFriction() const { return friction; }
    float getDeceleration() const { return deceleration; }
    float getGravity() const { return gravity; }
    float getTerminalVelocity() const { return terminalVelocity; }
    float getJumpStrength() const { return jumpStrength; }

    // Setters
    void setAcceleration(float v) { acceleration = v; }
    void setMaxSpeed(float v) { max_speed = v; }
    void setFriction(float v) { friction = v; }
    void setDeceleration(float v) { deceleration = v; }
    void setGravity(float v) { gravity = v; }
    void setTerminalVelocity(float v) { terminalVelocity = v; }
    void setJumpStrength(float v) { jumpStrength = v; }
};

#endif // PHYSICSCONFIG_H 