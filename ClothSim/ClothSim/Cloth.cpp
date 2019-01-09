#include "Cloth.h"

CCloth::CCloth(int m_width, int m_height)
{
	this->m_height = m_height;
	this->m_width = m_width;

	m_NumOfParticles = m_height * m_width;
	m_vertices.resize(m_NumOfParticles);
	m_normals.resize(m_NumOfParticles);

	/*Create Texture Coordinates*/
	float xPos = 1.f / m_width;
	float yPos = 1.f / m_height;

	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			CParticle* Particle;
			Particle = new CParticle(glm::vec3(((float)x / 2.f) - (m_width), ((float)y / 2.f) - (m_height), 0));
			m_ArrayOfParticles.push_back(Particle);

			m_UVs.push_back(glm::vec2(xPos * float(x), -yPos * (float)y));
		}
	}

	/*Shear Constraints*/
	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			CParticle* ParticleA = m_ArrayOfParticles[GetIndexFromGridCoord(x, y)];

			/*Stretch Constraints*/
			if (x + 1 < m_width)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB,STRETCH));
			}

			if (y + 1 < m_height)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x, y + 1)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB, STRETCH));
			}

			/*Shear Constraints*/
			if (x + 1 < m_width && y + 1 < m_height)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y + 1)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB, SHEAR));
			}

			if (x - 1 >= 0 && y + 1 < m_height)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x - 1, y + 1)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB, SHEAR));
			}

			/*Bend constraints*/
			if (x + 2 < m_width && y + 2 < m_height)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x + 2, y + 2)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB, BEND));
			}

			if (x + 2 < m_width && y < m_height)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x + 2, y)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB,BEND));
			}

			if (x < m_width && y + 2 < m_height)
			{
				CParticle* ParticleB = m_ArrayOfParticles[GetIndexFromGridCoord(x, y + 2)];
				m_ArrayOfConstraints.push_back(new CConstraint(ParticleA, ParticleB, BEND));
			}
		}
	}

	//for (unsigned int i = 0; i < m_width - 1; i++)
	//{
	//	m_ArrayOfParticles[GetIndexFromGridCoord(i, m_height - 1)]->m_moveable = false;
	//}

	for (unsigned int i = 0; i < 1; i++)
	{
		m_ArrayOfParticles[GetIndexFromGridCoord(i, m_height - 1)]->m_moveable = false;
		m_ArrayOfParticles[GetIndexFromGridCoord((m_width - 1) - i, m_height - 1)]->m_moveable = false;
	}

	int CentrePoint = (m_width - 1) / 2;
	CentrePoint = floor(CentrePoint);
	m_ArrayOfParticles[GetIndexFromGridCoord(CentrePoint, m_height - 1)]->m_moveable = false;
	m_ArrayOfParticles[GetIndexFromGridCoord(CentrePoint + 1, m_height - 1)]->m_moveable = false;
	m_ArrayOfParticles[GetIndexFromGridCoord(CentrePoint - 1, m_height - 1)]->m_moveable = false;



	/*Setup Buffers*/
	glGenVertexArrays(1, &VertexArrayBuffer);
	glGenBuffers(NUMOFBUFFERS, BufferArrayObjects);

	/*Stream the indices*/
	for (unsigned int x = 0; x < m_width - 1; x++)
	{
		for (unsigned int z = 0; z < m_height - 1; z++)
		{
			//First triangle
			m_indices.push_back(x * m_height + z);
			m_indices.push_back(x * m_height + z + 1);
			m_indices.push_back((x + 1) * m_height + z);

			//Second triangle
			m_indices.push_back((x + 1) * m_height + z);
			m_indices.push_back(x * m_height + z + 1);
			m_indices.push_back((x + 1) * m_height + z + 1);
		}
	}

	/*Create the faces*/
	for (int x = 0; x < m_width - 1; x++)
	{
		for (int y = 0; y < m_height - 1; y++)
		{
			ClothFace Face;
			Face.p1 = m_ArrayOfParticles[GetIndexFromGridCoord(x, y)];
			Face.p2 = m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y)];
			Face.p3 = m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y + 1)];
			Face.p4 = m_ArrayOfParticles[GetIndexFromGridCoord(x, y + 1)];
			m_Faces.push_back(Face);
		}
	}
}

CCloth::~CCloth()
{
	for (size_t i = 0; i < m_NumOfParticles; i++)
	{
		delete m_ArrayOfParticles[i];
	}

	for (size_t i = 0; i < m_ArrayOfConstraints.size(); i++)
	{
		delete m_ArrayOfConstraints[i];
	}
}

void CCloth::Update(float deltaTime)
{
	for (size_t i = 0; i < m_ArrayOfParticles.size(); i++)
	{
		m_ArrayOfParticles[i]->AddForce(glm::vec3(0, -0.04f, 0));
		m_ArrayOfParticles[i]->Update(deltaTime);
	}

	for (size_t i = 0; i < m_ArrayOfConstraints.size(); i++)
	{
		m_ArrayOfConstraints[i]->Update(deltaTime);
		
	}
	
	Wind(glm::vec3(0.01,-0.0001,-0.001f));

	/*stream the vertices*/
	for (unsigned int i = 0; i < m_ArrayOfParticles.size(); i++)
	{
		m_vertices[i] = m_ArrayOfParticles[i]->GetPosition();
	}

	if (!bUseOldShading)
	{
		/*Stream the normals*/
		for (size_t i = 0; i < m_Faces.size(); i++)
		{
			m_Faces[i].normal = ComputeNormalNormalized(m_Faces[i].p1, m_Faces[i].p2, m_Faces[i].p3);
		}

		/*For Each Particle*/
		for (size_t i = 0; i < m_ArrayOfParticles.size(); i++)
		{
			CParticle* particle = m_ArrayOfParticles[i];
			glm::vec3 n(0, 0, 0);

			/*Go through every face*/
			for (unsigned int k = 0; k < m_Faces.size(); k++)
			{
				/*and check if the particle is connected to face*/
				if (particle == m_Faces[k].p1 || particle == m_Faces[k].p2 || particle == m_Faces[k].p3 || particle == m_Faces[k].p4)
				{
					//if it is, go ahead and add it to n
					n += m_Faces[k].normal;
				}
			}

			m_normals[i] = n;
		}
	}
	else
	{

		int count = 0;
		for (int x = 0; x < m_width; x++)
		{
			for (int y = 0; y < m_height; y++)
			{
				glm::vec3 a;
				glm::vec3 b;
				glm::vec3 c;

				a = m_ArrayOfParticles[GetIndexFromGridCoord(x, y)]->GetPosition();

				if (x < m_width - 1)
				{
					b = m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y)]->GetPosition();
				}
				else
				{
					b = m_ArrayOfParticles[GetIndexFromGridCoord(x - 1, y)]->GetPosition();
					b *= -1;
				}

				if (y < m_height - 1)
				{
					c = m_ArrayOfParticles[GetIndexFromGridCoord(x, y + 1)]->GetPosition();
				}
				else
				{
					c = m_ArrayOfParticles[GetIndexFromGridCoord(x, y - 1)]->GetPosition();
					c *= -1;
				}

				m_normals[count] = ComputeNormalNormalized(a,b,c);
				count++;
			}
		}
	}



	/*Set up GL calls*/
	glBindVertexArray(VertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,BufferArrayObjects[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferArrayObjects[ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferArrayObjects[NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void CCloth::Render(CCamera Camera, GLuint Shader)
{
	glUseProgram(Shader);

	//Lets make our models matrix first
	glm::mat4 m_positionMatrix = glm::translate(glm::vec3(0, 0, 0));
	glm::mat4 m_scaleMatrix = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 m_modelMatrix = m_positionMatrix * m_scaleMatrix;	//No Rotations needed!

	glUniformMatrix4fv(glGetUniformLocation(Shader, "Model"), 1, GL_FALSE, &m_modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Shader, "View"), 1, GL_FALSE, &Camera.ViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Shader, "Projection"), 1, GL_FALSE,&Camera.ProjectMatrix[0][0]);

	/*Gonna use an element buffer*/
	glBindVertexArray(VertexArrayBuffer);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	if (iDrawType == 0)
	{
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
	}
	else if (iDrawType == 1)
	{
		glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
	}
	else if (iDrawType == 2)
	{
		glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
	}
	

	glBindVertexArray(0);
}

void CCloth::AddWindForces(CParticle* p1, CParticle* p2,CParticle* p3, glm::vec3 direction)
{
	glm::vec3 Normal = ComputeNormal(p1->GetPosition(), p2->GetPosition(), p3->GetPosition());
	glm::vec3 d = glm::normalize(Normal);
	glm::vec3 force = Normal * (glm::dot(d, direction));
	p1->AddForce(force);
	p2->AddForce(force);
	p3->AddForce(force);
}

void CCloth::Wind(glm::vec3 Direction)
{
	for (int x = 0; x < m_width - 1; x++)
	{
		for (int y = 0; y < m_height - 1; y++)
		{
			
			AddWindForces
			(
				m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y)], 
				m_ArrayOfParticles[GetIndexFromGridCoord(x, y)],
				m_ArrayOfParticles[GetIndexFromGridCoord(x, y + 1)],
				Direction
			);

			AddWindForces
			(
				m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y + 1)],
				m_ArrayOfParticles[GetIndexFromGridCoord(x + 1, y)],
				m_ArrayOfParticles[GetIndexFromGridCoord(x, y + 1)],
				Direction
			);
		}
	}
}

CParticle * CCloth::GetParticle(int Index)
{
	return m_ArrayOfParticles[Index];
}

void CCloth::AddTexture(std::string path)
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width;
	int height;
	int Channels;
	unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &Channels, 0);

	if (data)
	{
		std::cout << "texture success" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		delete data;
	}
	else
	{
		std::cout << "texture failed" << std::endl;
	}

	glBindVertexArray(VertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, BufferArrayObjects[TEXTURE_COORDS]);
	glBufferData(GL_ARRAY_BUFFER, m_UVs.size() * sizeof(glm::vec2), &m_UVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

glm::vec3 CCloth::ComputeNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	return glm::cross(b - a, c - a);
}

glm::vec3 CCloth::ComputeNormalNormalized(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	return glm::normalize(glm::cross(b - a, c - a));
}

glm::vec3 CCloth::ComputeNormalNormalized(CParticle * a, CParticle * b, CParticle * c)
{
	return glm::normalize(glm::cross(b->GetPosition() - a->GetPosition(), c->GetPosition() - a->GetPosition()));
}

int CCloth::GetIndexFromGridCoord(int x, int y)
{
	return (x * m_height + y);
}
