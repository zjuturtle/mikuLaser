#pragma once
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"
struct PointCloudPoint {
	// Position
	glm::vec3 Position;
	// Color
	glm::vec4 Color;
};

class PointCloud {
public:
	/*  Laser Data  */
	std::vector<PointCloudPoint> points;

	/*  Functions  */
	// Constructor
	PointCloud(std::vector<PointCloudPoint> points)
	{
		this->points = points;

		// Now that we have all the required data, set the vertex buffers and its attribute pointers.
	}

	PointCloud(void) {
		
	}

	// Render the mesh
	void Draw(Shader shader)
	{
		// Draw points

		glBindVertexArray(this->VAO);
		glDrawElements(GL_POINTS, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupPointCloud()
	{
		// Create buffers/arrays
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		for (int i = 0; i < points.size(); ++i) {
			indices.push_back(i);
		}

		glBindVertexArray(this->VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, this->points.size() * sizeof(PointCloudPoint), &this->points[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Point Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PointCloudPoint), (GLvoid*)0);
		// Point Color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PointCloudPoint), (GLvoid*)offsetof(PointCloudPoint, Color));

		glBindVertexArray(0);
	}
private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	std::vector<GLuint> indices;

};
