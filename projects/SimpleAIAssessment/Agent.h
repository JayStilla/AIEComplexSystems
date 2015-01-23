#pragma once 
using namespace std; 

#include "Utilities.h"
#include "Behaviour.h"
#include <glm/glm.hpp>
#include <vector>

class Agent
{
public:

	float hp;
	Agent* enemy; 
	bool dead = false;

	Agent() : m_behaviour(nullptr), m_position(0), m_target(0) {}
	virtual ~Agent() {}

	const glm::vec3& getPosition() const { return m_position; }
	const glm::vec3& getTarget() const { return m_target;  }

	void setPosition(const glm::vec3& a_pos) { m_position = a_pos; }
	void setTarget(const glm::vec3& a_pos) { m_target = a_pos; }

	void setBehaviour(Behaviour* a_behaviour) { m_behaviour = a_behaviour; }

	virtual void update(float a_deltaTime)
	{
		if (m_behaviour != nullptr)
			m_behaviour->execute(this);
	}

private:

	Behaviour* m_behaviour; 
	Behaviour* m_behaviour2; 

	glm::vec3	m_position; 
	glm::vec3	m_target; 
};

// WithinRange
// RandomiseTarget
// SeekTarget

// if WithinRange then RandomiseTarget else SeekTarget

class WithinRange : public Behaviour
{
public:

	WithinRange(float a_range) : m_range2(a_range*a_range) {}
	virtual ~WithinRange() {}

	virtual bool	execute(Agent* a_agent)
	{
		float dist2 = glm::distance2(a_agent->getPosition(), a_agent->getTarget());

		if (dist2 < m_range2)
			return true;
		return false;
	}
	float m_range2;
};

class RandomiseTarget : public Behaviour
{
public:

	RandomiseTarget(float a_radius) : m_radius(a_radius) {}
	virtual ~RandomiseTarget() {}

	virtual bool execute(Agent* a_agent)
	{
		glm::vec3 target(0);

		target.xz = glm::circularRand(m_radius);

		a_agent->setTarget(target);
		return true;
	}

	float m_radius;
};

class SeekTarget : public Behaviour
{
public:

	SeekTarget(float a_speed) : m_speed(a_speed) {}
	virtual ~SeekTarget() {}

	virtual bool execute(Agent* a_agent)
	{
		glm::vec3 pos = a_agent->getPosition();
		glm::vec3 dir = glm::normalize(a_agent->getTarget() - pos);

		a_agent->setPosition(pos + dir * m_speed * Utility::getDeltaTime());
		return true;
	}

	float m_speed;
};

class WithinEnemyRange : public Behaviour
{
public:

	WithinEnemyRange(float a_range) : m_range2(a_range*a_range) {}
	virtual ~WithinEnemyRange() {}

	virtual bool execute(Agent* a_agent)
	{
		float dist2 = glm::distance2(a_agent->getPosition(), a_agent->enemy->getPosition());

		if (dist2 < m_range2)
			return true;
		return false;
	}
	float m_range2;

};

class SeekEnemy : public Behaviour
{
public:

	SeekEnemy(float a_speed, float a_attack) : m_speed(a_speed), m_attack(a_attack) {}
	virtual ~SeekEnemy() {}

	virtual bool execute(Agent* a_agent)
	{
		if (a_agent->enemy->hp > 0)
		{
			//a_agent->setTarget(m_target->getPosition());
			glm::vec3 pos = a_agent->getPosition();
			glm::vec3 dir = glm::normalize(a_agent->enemy->getPosition() - pos);
			float dist2 = glm::distance2(a_agent->getPosition(), a_agent->enemy->getPosition());
			if (dist2 > 1.5f)
				a_agent->setPosition(pos + dir * m_speed * Utility::getDeltaTime());
			else {
				if (dist2 <= 3)
					a_agent->enemy->hp -= m_attack;
			}
			return true;
		}
		else
			return false;
	}
	float m_attack;
	float m_speed;
};
class FleeEnemy : public Behaviour
{
public:

	FleeEnemy(float a_speed) : m_speed(a_speed) {}
	virtual ~FleeEnemy() {}

	virtual bool execute(Agent* a_agent)
	{
		float dist2 = glm::distance2(a_agent->getPosition(), a_agent->enemy->getPosition());
		if (dist2 > 80)
		{
			return false;
		}
		if (a_agent->hp <= 20)
		{
			//float dist2 = glm::distance2(a_agent->getPosition(), a_agent->enemy->getPosition());


			glm::vec3 pos = a_agent->getPosition();
			glm::vec3 dir = glm::normalize(a_agent->enemy->getPosition() - pos);
			dir = glm::vec3(dir.x*-1, dir.y*-1, dir.z);

			a_agent->setPosition(pos + dir * m_speed * Utility::getDeltaTime());

			return true;
		}
		else
			return false;
	}
	float m_speed;
};