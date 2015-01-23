#pragma once

#include <vector>
class Agent;

class Behaviour
{
public:

	Behaviour(){}
	virtual ~Behaviour() {}

	virtual bool	execute(Agent* a_agent) = 0;
};

class Composite : public Behaviour
{
public:

	Composite() {}
	virtual ~Composite() {}

	void	addChild(Behaviour* a_behaviour) { m_children.push_back(a_behaviour); }
protected:

	std::vector<Behaviour*>m_children;
};

class Selector : public Composite
{
public:

	Selector() {}
	virtual ~Selector() {}

	virtual bool	execute(Agent* a_agent)
	{
		for (auto behaviour : m_children)
		{
			if (behaviour->execute(a_agent) == true)
				return true;
		}
		return false;
	}
};

class Sequence : public Composite
{
public:

	Sequence() {}
	virtual ~Sequence() {}

	virtual bool	execute(Agent* a_agent)
	{
		for (auto behaviour : m_children)
		{
			if (behaviour->execute(a_agent) == false)
				return false;
		}
		return true;
	}
};