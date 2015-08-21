#pragma once
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Laser {
public:
	/*  Laser Data  */
	std::vector<glm::vec3> originPoints;
	std::vector<glm::vec3> endPoints;
	std::vector<glm::vec3> dirctions;
	/*  Functions  */
	// Constructor
	Laser(std::vector<glm::vec3> originPoints, std::vector<glm::vec3> dirctions)
	{
		this->originPoints = originPoints;
		this->dirctions = dirctions;

		// Now that we have all the required data, set the vertex buffers and its attribute pointers.
	}

	// Render the mesh
	void Draw(Shader shader)
	{

		// Draw laser

		glBindVertexArray(this->VAO);
		glDrawElements(GL_LINES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupLaser()
	{
		std::vector<glm::vec3> vertices(originPoints);
		vertices.insert(vertices.end(), endPoints.begin(), endPoints.end());

		for (int i = 0; i < originPoints.size(); ++i) {
			indices.push_back(i);
			indices.push_back(i + originPoints.size());
		}
		// Create buffers/arrays
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

		glBindVertexArray(0);
	}
private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	std::vector<GLuint> indices;

	
};
