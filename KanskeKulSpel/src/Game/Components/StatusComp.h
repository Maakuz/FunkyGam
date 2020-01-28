#pragma once
#include "Comp.h"
#include <unordered_map>
#include "Game/Components/Statuses/Status.h"

class StatusComp : public Comp
{
public:
	static const int STATUS_COUNT = 1;
	
	//statusID and status
	typedef std::unordered_map<Statuses, Status*> StatusList; 

	StatusComp() {};
	virtual ~StatusComp();


	void resolveStatusEffects(Entity* owner, float dt);
	void addStatusEffect(Statuses status, int duration);
	void removeStatusEffect(Statuses status);
	void addFromStatusComp(const StatusComp& other);

	friend std::istream& operator>>(std::istream& in, StatusComp& status);
	friend std::ostream& operator<<(std::ostream& out, const StatusComp& status);

	Comp* clone() const { return new StatusComp(*this); };

	StatusList* getStatusPtr() { return &m_statuses; };
	const StatusList* getStatusPtr() const { return &m_statuses; };

	static ComponentKey getStaticKey() { return ComponentKey::status; };
	static StatusList getTemplateList();
private:
	StatusList m_statuses;

	void clear();


};