#include "Particle.h"

CParticle::CParticle(glm::vec3 m_position)
{
	this->m_position = m_position;
	m_oldPosition = m_position;
}

CParticle::~CParticle()
{

}

void CParticle::AddForce(glm::vec3 force)
{
	/*Newtons second law*/
	m_acceleration += force / m_mass;
}

void CParticle::Update(float deltaTime)
{
	float DAMPING = 0.97f;

	if (m_moveable)
	{
		/*Verlet Integration*/
		glm::vec3 Temp = m_position;
		m_position = m_position + (m_position - m_oldPosition) * DAMPING + m_acceleration * (deltaTime);
		m_oldPosition = Temp;
		m_acceleration = glm::vec3(0, 0, 0);
	}
}

void CParticle::SetPosition(glm::vec3 _Val)
{
	m_position = _Val;
}

glm::vec3 CParticle::GetPosition()
{
	return m_position;
}
