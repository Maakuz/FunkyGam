#include "HealthComp.h"
#include <algorithm>
HealthComp::HealthComp()
{
    this->health = 0;
    this->maxHealth = 0;
    this->alive = true;
}

void HealthComp::takeDamage(int damage)
{
    this->health -= damage;
    if (this->health <= 0)
        this->alive = false;
}

void HealthComp::setCurrentHealth(int health)
{
    this->health = std::min(health, this->maxHealth);

    if (this->health > 0)
        this->alive = true;

    else
        this->alive = false;
}

void HealthComp::setMaxHealth(unsigned int health)
{
    this->maxHealth = health;

    this->health = std::min(this->health, this->maxHealth);

    if (this->maxHealth == 0)
        this->alive = false;
}

float HealthComp::getHealthPercentage() const
{
    return this->health / (float)this->maxHealth;
}

void HealthComp::fillHealth()
{ 
    this->health = this->maxHealth; 
    this->alive = true;
}
