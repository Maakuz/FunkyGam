#pragma once
#include "SpriteComp.h"
#include <vector>

struct Animation
{
    sf::Vector2u idleFrame;
    sf::Vector2u startFrame;
    sf::Vector2u stopFrame;
    float animationSpeed;
    bool startIdle;

    Animation(sf::Vector2u startFrame, sf::Vector2u stopFrame, float animationSpeed, sf::Vector2u idleFrame = sf::Vector2u(0, 0), bool startIdle = 0)
    {
        this->animationSpeed = animationSpeed;
        this->idleFrame = idleFrame;
        this->startFrame = startFrame;
        this->stopFrame = stopFrame;
        this->startIdle = startIdle;
    }
};
struct AnimationData
{
    sf::Texture* spriteSheet;
    sf::Vector2u frameCount;
    std::vector<Animation> animations;
    int currentAnimation;

    AnimationData(sf::Texture* spriteSheet, sf::Vector2u frameCount, std::vector<Animation> animations)
    {
        this->spriteSheet = spriteSheet;
        this->frameCount = frameCount;
        this->animations = animations;
        this->currentAnimation = 0;
    }
    AnimationData()
    {
        this->spriteSheet = nullptr;
        this->currentAnimation = 0;
    };
};


class AnimatedSpriteComp : public SpriteComp
{
public:
    AnimatedSpriteComp(AnimationData data, sf::Vector2f pos = sf::Vector2f(0, 0));

    //only for getting the key
    virtual ~AnimatedSpriteComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::animatedSprite; };
    virtual Comp* clone() const { return new AnimatedSpriteComp(*this); };

    void flipHorizontally();
    void setAnimationData(AnimationData data);

    void setAnimation(int animation);
    void pauseAnimation();
    void resumeAnimation() { this->idle = false; };
    void updateAnimation(float dt);

    void setAnimationFrame(sf::Vector2u frame) { this->currentFrame = frame; };

    bool isIdle() const { return this->idle; }
private:
    sf::Vector2u currentFrame;
    bool idle;
    float timer;

    sf::IntRect texRect;

    AnimationData data;
};