#pragma once

class HealthComp
{
public:
    HealthComp();
    ~HealthComp() {};

    void takeDamage(int damage);

    void setHealth(int health);
    int getHealth() const { return health; };

    void setMaxHealth(unsigned int health);
    int getMaxHealth() const { return maxHealth; };

    void fillHealth();

    bool isAlive()const { return alive; };
private:
    int health;
    int maxHealth;
    bool alive;
};
