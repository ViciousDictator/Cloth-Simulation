#include "Camera.h"

CCamera::CCamera(int _width, int _height)
{
	/*Setup pos and dir*/
	m_position = glm::vec3(0, 0 , 100);
	SpeedBoost = false;
	cameraSpd = 2.5f;

	for (unsigned int i = 0; i < 256; i++)
	{
		keyArray[i] = false;
	}

	UpDirection = glm::vec3(0, 1, 0);
	//CameraFront = glm::vec3(0, 0, 1);
	CameraFront = glm::normalize(glm::vec3(0, 0, 0) - m_position);

	//make our projection matrix
	ProjectMatrix = glm::perspective(glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 100000.0f);

	//make view matrix
	ViewMatrix = glm::lookAt
	(
		m_position,			//Camera is where now?
		glm::vec3(0,0,0),	//Camera looks at what now?
		UpDirection			//Camera head is what way now?
	);
}

CCamera::~CCamera()
{

}

void CCamera::resize(int _width, int _height)
{
	//make our projection matrix
	ProjectMatrix = glm::perspective(glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 1000.0f);

	//make view matrix
	ViewMatrix = glm::lookAt
	(
		m_position,	//Camera is where now?
		glm::vec3(0, 0, 0),	//Camera looks at what now?
		UpDirection	//Camera head is what way now?
	);
}

void CCamera::update(float deltaTime)
{
	//make view matrix
	ViewMatrix = glm::lookAt
	(
		m_position,	//Camera is where now?
		m_position + CameraFront,	//Camera looks at what now?
		UpDirection	//Camera head is what way now?
	);

	float deltaCamSpd = cameraSpd * deltaTime;

	//Lets handle movement
	if (keyArray['w'] == true)
	{
		m_position += deltaCamSpd * CameraFront;
	}
	else if (keyArray['s'] == true)
	{
		m_position += -deltaCamSpd * CameraFront;
	}

	if (keyArray['a'] == true)
	{
		m_position += -deltaCamSpd * glm::normalize(glm::cross(CameraFront, UpDirection));
	}
	else if (keyArray['d'] == true)
	{
		m_position += deltaCamSpd * glm::normalize(glm::cross(CameraFront, UpDirection));
	}

	if (keyArray['e'] == true)
	{
		m_position += deltaCamSpd * UpDirection;
	}
	else if (keyArray['q'] == true)
	{
		m_position += -deltaCamSpd * UpDirection;
	}

	if (SpeedBoost)
	{
		cameraSpd = 100.0f;
	}
	else
	{
		cameraSpd = 2.5f;
	}

	MoveAlongAxis(deltaTime);
}

void CCamera::MoveAlongAxis(float deltaTime)
{
	if (bMoveAlongAxis)
	{
		float deltaCamSpd = cameraSpd * deltaTime;
		m_position += -deltaCamSpd * glm::vec3(-1, 0, 0);
	}

}
