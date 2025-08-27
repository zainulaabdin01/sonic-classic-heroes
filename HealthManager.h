#ifndef HEALTH_MANAGER_H
#define HEALTH_MANAGER_H

class HealthManager {
private:
    int health;
    int maxHealth;
public:
    HealthManager(int initial = 3, int maxH = 9) : health(initial), maxHealth(maxH) {}

    void incrementHealth() { if (health < maxHealth) health++; }
    void decrementHealth() { if (health > 0) health--; }
    int getHealth() const { return health; }
    void resetHealth(int value = 3) { health = value; }
    int getMaxHealth() const { return maxHealth; }
};

#endif // HEALTH_MANAGER_H 