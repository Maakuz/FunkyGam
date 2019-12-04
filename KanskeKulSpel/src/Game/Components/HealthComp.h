#pragma once
#include "Game/Components/Comp.h"

class HealthComp : public Comp
{
public:
    HealthComp();
    virtual ~HealthComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::health; };
    virtual Comp* clone() const { return new HealthComp(*this); };

    void takeDamage(int damage);

    void setCurrentHealth(int health);
    int getCurrentHealth() const { return health; };

    void setMaxHealth(unsigned int health);
    int getMaxHealth() const { return maxHealth; };

    float getHealthPercentage() const;

    void fillHealth();

    bool isAlive()const { return alive; };
private:
    int health;
    int maxHealth;
    bool alive;
};
