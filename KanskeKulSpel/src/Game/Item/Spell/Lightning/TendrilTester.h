#include "Tendril.h"

class TendrilTester : public sf::Drawable
{
public:
    TendrilTester();
    virtual ~TendrilTester() {};

    void update();

    bool open;

private:
    Tendril tendril;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};