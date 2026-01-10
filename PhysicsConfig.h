#ifndef PHYSICSCONFIG_H
#define PHYSICSCONFIG_H

class PhysicsConfig {
private:
    float acceleration;      // Ground acceleration
    float max_speed;         // Maximum horizontal speed
    float friction;          // Ground friction multiplier (0.0-1.0, higher = more slippery)
    float deceleration;      // Not used with new system, kept for compatibility
    float gravity;           // Gravity strength
    float terminalVelocity;  // Max fall speed
    float jumpStrength;      // Initial jump velocity (negative = up)
    float airControl;        // Air control multiplier (0.0-1.0)
    
public:
    // Updated defaults for smooth, responsive movement
    PhysicsConfig(float acc = 0.45f, float maxSpd = 15.0f, float fric = 0.92f, float decel = 0.2f, 
                  float grav = 0.65f, float termVel = 18.0f, float jumpStr = -18.0f, float airCtrl = 0.6f)
        : acceleration(acc), max_speed(maxSpd), friction(fric), deceleration(decel), 
          gravity(grav), terminalVelocity(termVel), jumpStrength(jumpStr), airControl(airCtrl) {}

    // Getters
    float getAcceleration() const { return acceleration; }
    float getMaxSpeed() const { return max_speed; }
    float getFriction() const { return friction; }
    float getDeceleration() const { return deceleration; }
    float getGravity() const { return gravity; }
    float getTerminalVelocity() const { return terminalVelocity; }
    float getJumpStrength() const { return jumpStrength; }
    float getAirControl() const { return airControl; }

    // Setters
    void setAcceleration(float v) { acceleration = v; }
    void setMaxSpeed(float v) { max_speed = v; }
    void setFriction(float v) { friction = v; }
    void setDeceleration(float v) { deceleration = v; }
    void setGravity(float v) { gravity = v; }
    void setTerminalVelocity(float v) { terminalVelocity = v; }
    void setJumpStrength(float v) { jumpStrength = v; }
    void setAirControl(float v) { airControl = v; }
};

#endif // PHYSICSCONFIG_H 