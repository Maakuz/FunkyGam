#include "Player.h"
#include "Game/Misc/KeyboardState.h"
#include "Game/Misc/MouseState.h"
#include "Game/Misc/VectorFunctions.h"
#include "Renderer/Lighting/LightQueue.h"
#include "Imgui/imgui.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Misc/ConsoleWindow.h"
#include "Enemies/Grunt.h"

#define TESTING false

Player::Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos, sf::Vector2f size)
    :Collidable(pos, size)
{
    getColliderComp()->addComponent(ColliderKeys::player);

    addComponent<MovementComp>(new MovementComp);

    AnimatedSpriteComp* sprite = new AnimatedSpriteComp(data, pos);
    addComponent<AnimatedSpriteComp>(sprite);

    TomeComp* tome = new TomeComp(getComponent<ColliderComp>()->getCenterPos());
    addComponent<TomeComp>(tome);

    addComponent<HealthComp>(new HealthComp);

    m_ui = uiHandler;
    m_illumination = 0;
    m_throwingPower = 0;
    m_exitReached = -1;
    m_debugMode = false;
    m_noClip = false;
    m_canReturn = false;
    m_returning = false;
    m_gatherableInRange = nullptr;

    m_platformPassingCounter.stopValue = 1000;
    m_platformPassingCounter.counter = m_platformPassingCounter.stopValue;

    m_invincibilityCounter.stopValue = 200;
    m_invincibilityCounter.counter = 200;

    sprite->spriteOffset.y = -(abs(sprite->getTextureRect().height) - size.y);
    sprite->spriteOffset.x = -size.x / 2.f;

    
   ConsoleWindow::get().addCommand("setPos", [&](Arguments args)->std::string 
        {
            if (args.size() >= 2)
            {
                try
                {
                    int x = std::stoi(args[0]);
                    int y = std::stoi(args[1]);

                     this->getMovementComp()->transform.pos = (sf::Vector2f(x, y));
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid coordinates. ";
                    ret.append(e.what());
                    return ret;
                }
            }

            else
                return "missing argument int x, int y";


            return "Position set";
        });
    
    ConsoleWindow::get().addCommand("debugMode", [&](Arguments args)->std::string
        {
            if (args.size() >= 1)
            {
                try
                {
                    int x = std::stoi(args[0]);

                    m_debugMode = x;
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid argument 0 or 1.";
                    return ret;
                }
            }

            else
                return "missing argument 0 or 1";


            return "The wondow is somewhere";
        });

    ConsoleWindow::get().addCommand("setHealth", [&](Arguments args)->std::string
        {
            if (args.size() >= 1)
            {
                try
                {
                    int x = std::stoi(args[0]);

                    this->getHealthComp()->setCurrentHealth(x);
                }
                catch (const std::exception & e)
                {
                    std::string ret = "Not valid argument int.";
                    return ret;
                }
            }

            else
                return "missing argument int health";


            return "The life is liven";
        });

}

void Player::update(float dt, sf::Vector2f mousePos)
{
    MovementComp* movement = getMovementComp();
    AnimatedSpriteComp* sprite = getComponent<AnimatedSpriteComp>();

    m_invincibilityCounter.update(dt);

    if (m_debugMode)
    {
        ImGui::Begin("Player tweaker", &m_debugMode);

        ImGui::DragFloat("Walk speed", &movement->walkSpeed, 0.01, 0, 10000);
        ImGui::DragFloat("Jump height", &movement->jumpHeight, 0.01, 0, 10000);
        ImGui::DragFloat("Mass", &movement->mass, 0.01, 0, 10000);

        ImGui::Checkbox("Noclip", &m_noClip);

        ImGui::End();
    }

    if (!m_noClip)
        updateMove(dt);

    else
        debugMove(dt);

    if (movement->grounded)
        sprite->setAnimation(0);

    else
        sprite->setAnimation(1);

    movement->update(dt);

    updateItems(dt, mousePos);
    getTomeComp()->update(dt, getComponent<ColliderComp>()->getCenterPos());
    getComponent<ColliderComp>()->setPosition(movement->transform.pos);
    sprite->updateAnimation(dt);
    sprite->setPosition(movement->transform.pos);

    if (m_canReturn && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        m_returning = true;

    if (m_gatherableInRange != nullptr && KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::E))
    {
        m_gatherableInRange->item.getComponent<LogisticsComp>()->pluck();
        m_ui->getInventory()->addItem(m_gatherableInRange->item.getLogisticsComp()->id, m_gatherableInRange->count);
    }

    m_ui->setHealthPercentage(getHealthComp()->getHealthPercentage());
    m_canReturn = false;

    if (abs(movement->momentum.x) < movement->walkSpeed * 0.75f && !sprite->isIdle())
        sprite->pauseAnimation();

    else if (abs(movement->momentum.x) > movement->walkSpeed * 0.75f)
        sprite->resumeAnimation();


}

void Player::reset(sf::Vector2f spawnPoint, bool fillHealth)
{
    MovementComp* movement = getMovementComp();
    movement->transform.pos = spawnPoint;
    m_returning = false;
    m_exitReached = -1;
    movement->reset();

    if (fillHealth)
        getHealthComp()->fillHealth();
}

void Player::updateMove(float dt)
{
    MovementComp* movement = getMovementComp();
    AnimatedSpriteComp* sprite = getAnimatedSpriteComp();

    movement->acceleration = sf::Vector2f(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        movement->acceleration.x = 1;

        if (sprite->isFlippedHorizontally())
            sprite->flipHorizontally();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        movement->acceleration.x = -1;

        if (!sprite->isFlippedHorizontally())
            sprite->flipHorizontally();
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        movement->jump();
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        m_platformPassingCounter.reset();

    if (movement->isJumping() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        movement->stopJump(0.3);


    m_platformPassingCounter.update(dt);

}

void Player::debugMove(float dt)
{
    MovementComp* movement = getMovementComp();

    movement->acceleration = sf::Vector2f(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement->acceleration.x = 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement->acceleration.x = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        movement->acceleration.y = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        movement->acceleration.y = 1;
}

void Player::updateItems(float dt, sf::Vector2f mousePos)
{
    if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Button::Right))
    {
        int itemID = m_ui->getInventory()->useSelectedItem();
        if (itemID != -1)
        {
            if (dynamic_cast<const Throwable*>(ItemHandler::getTemplate(itemID)))
            {
                MovementComp* movement = getMovementComp();

                sf::Vector2f direction = mousePos - movement->transform.pos - sf::Vector2f(16, 10);
                normalize(direction);
                direction *= (m_throwingPower);

                direction += movement->momentum * 0.5f;

                ProjectileHandler::addThrowable(itemID, movement->transform.pos, direction, DamageComp::DamageOrigin::player);
            }

            else if (dynamic_cast<const Tome*>(ItemHandler::getTemplate(itemID)))
            {
                getTomeComp()->startChannelling(itemID);
            }
        }
    }

    if (getTomeComp()->isChannelling()  && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        getTomeComp()->castSpell(mousePos);
    }

    for (int i = 0; i < 5; i++)
    {
        if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key(27 + i))) //keys 1 to 5
        {
            m_ui->getInventory()->setSelectedItem(i);
            getTomeComp()->stopChannelling();
        }
    }

    if (KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::I))
    {
        if (m_ui->getInventory()->isInventoryOpen())
            m_ui->getInventory()->closeInventory();

        else
            m_ui->getInventory()->openInventory();
    }
}

void Player::takeDamage(int damage)
{
    if (m_invincibilityCounter.isTimeUp())
    {
        getHealthComp()->takeDamage(damage);
        m_invincibilityCounter.reset();
    }
}

void Player::handleGround(const ColliderComp* otherCollider)
{
    MovementComp* movement = getMovementComp();
    ColliderComp* collider = getColliderComp();

    //walking on ground
    if (movement->momentum.y > 0 && ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
    {
        movement->momentum.y = 0;
        movement->transform.pos.y = (otherCollider->up() - collider->height());
        movement->grounded = true;
    }

    //smackin into roof
    if (movement->momentum.y < 0 && ColliderComp::intersects(otherCollider->getDownBox(), collider->getUpBox()))
    {
        movement->momentum.y = 0;
        movement->transform.pos.y = (otherCollider->down());
    }

    if (ColliderComp::intersects(otherCollider->getLeftBox(), collider->getRightBox()))
    {
        movement->momentum.x *= -0.5f;
        movement->transform.pos.x = (otherCollider->left() - collider->width());
    }

    if (ColliderComp::intersects(otherCollider->getRightBox(), collider->getLeftBox()))
    {
        movement->momentum.x *= -0.5f;
        movement->transform.pos.x = (otherCollider->right());
    }
}

void Player::handleCollision(const Collidable* colliderEntity)
{
    ColliderComp* collider = getColliderComp();
    const ColliderComp* otherCollider = colliderEntity->getColliderComp();
    MovementComp* movement = getMovementComp();

    if (!m_noClip)
    {

        if (movement->momentum.y > 0 && otherCollider->hasComponent(ColliderKeys::platform))
        {
            //walking on ground
            if (ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
            {
                if (m_platformPassingCounter.isTimeUp())
                {
                    movement->momentum.y = 0;
                    movement->transform.pos.y = (otherCollider->up() - collider->height());
                    movement->grounded = true;
                }
            }
        }

        else if (otherCollider->hasComponent(ColliderKeys::ground))
        {
            handleGround(otherCollider);
        }
    }

    if (otherCollider->hasComponent(ColliderKeys::levelReturn))
        m_returning = true;

    else if (otherCollider->hasComponent(ColliderKeys::levelWarp))
        m_canReturn = true;

    else if (otherCollider->hasComponent(ColliderKeys::customTerrain))
    {
        std::string flag = otherCollider->getFlag();
        if (flag.compare(0, 4, "exit") == 0)
            m_exitReached = flag[4] - '0';
    }

    else if (otherCollider->hasComponent(ColliderKeys::grunt))
    {
        const Grunt* ptr = dynamic_cast<const Grunt*>(colliderEntity);
        if (ptr->isAttacking())
        {
            movement->addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider->getCenterPos(), otherCollider->getCenterPos(), movement->momentum, ptr->getMovementComp()->momentum, movement->mass, ptr->getMovementComp()->mass));
            
            takeDamage(ptr->getDamageComp()->damage);
        }
    }

    else if (otherCollider->hasComponent(ColliderKeys::throwable) ||
        otherCollider->hasComponent(ColliderKeys::projectile) ||
        otherCollider->hasComponent(ColliderKeys::hazard))
    {
        const DamageComp* damage = colliderEntity->getComponent<DamageComp>();
        if (damage->origin != DamageComp::DamageOrigin::player)
        {
            movement->addCollisionMomentum(damage->knockback);

            takeDamage(damage->damage);
        }
    }
}

void Player::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(getComponent<ColliderComp>()->getCenterPos());
        takeDamage(damage);
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*getAnimatedSpriteComp(), states);
}

std::istream& operator>>(std::istream& in, Player& player)
{
    std::string trash;
    int health = 0;
    MovementComp* movement = player.getMovementComp();
    in >> trash;

    in >> trash >> movement->walkSpeed;
    in >> trash >> movement->jumpHeight;
    in >> trash >> movement->mass;
    in >> trash >> health;
    in >> trash >> player.m_throwingPower;

    player.getHealthComp()->setMaxHealth(health);
    player.getHealthComp()->setCurrentHealth(health);

    return in;
}
