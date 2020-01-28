#pragma once
#include "Status.h"

class Regeneration : public Status
{
public:
	Regeneration(int duration);
	virtual ~Regeneration() {};

	void resolve(Entity* owner, float dt);
private:

};