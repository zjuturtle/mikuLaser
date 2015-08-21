
#include <iostream>
#include <fstream>


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//SOIL
#include <SOIL.h>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//user include
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "load.h"
#include "Laser.h"
using namespace std;

// Function prototypes
GLFWwindow* initViewWindow();

//call back
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window,int button, int action, int mods);

//caculate laser
void caculate_laser(Laser &laser);
void fill_barrier_tank();
bool intersectTriangle(const glm::vec3& orig, const glm::vec3& dir,
	glm::vec3& v0, glm::vec3& v1, glm::vec3& v2,
	float* t, float* u, float* v);

// static settings
const GLuint VIEW_WIDTH = 800, VIEW_HEIGHT = 600;
const GLfloat BACK_GROUND_COLOR[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const glm::vec4 LASER_COLOR = glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
const GLfloat GROUND_WIDTH = 1.0f, GROUND_HEIGHT = 1.0f;

// ShadersFileName
const char environmentVertexShaderSourceFileName[] = "shader/environment.vertexShader";
const char environmentFragmentShaderSourceFileName[] = "shader/environment.fragmentShader";
const char environmentGeometryShaderSourceFileName[] = "shader/environment.geometryShader";
const char laserVertexShaderSourceFileName[] = "shader/laser.vertexShader";
const char laserFragmentShaderSourceFileName[] = "shader/laser.fragmentShader";
const char laserGeometryShaderSourceFileName[] = "shader/laser.geometryShader";

//camera 
Camera camera=Camera::Camera();
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

//all stuff meshs
vector<Mesh> laserBarrierMesh;
vector<glm::vec3> laserBarrierTriangles;


// The MAIN function, from here we start the application and run the game loop
int main()
{

	//Initialize laser simulation window
	GLFWwindow* viewWindow=initViewWindow();

	//compile and link shader
	Shader laserShader(laserVertexShaderSourceFileName,laserFragmentShaderSourceFileName );
	Shader environmentShader(environmentVertexShaderSourceFileName,environmentFragmentShaderSourceFileName);

	//load environment model
	vector<Vertex> vertices;
	vector<Texture> textures;
	vector<GLuint> indices;
	loadGroundMesh(vertices,indices,textures);
	Mesh environmentMesh(vertices,indices,textures);
	laserBarrierMesh.push_back(environmentMesh);//load environment into memory for laser compute
	fill_barrier_tank();

	//caculate laser
	vector<glm::vec3> originPoints;
	vector<glm::vec3> direction;
	originPoints.push_back(glm::vec3(0.f,0.0f,5.0f));
	direction.push_back(glm::vec3(0.f,0.f,-1.f));

	Laser testLaser(originPoints, direction);
	fill_barrier_tank();
	caculate_laser(testLaser);
	testLaser.setupLaser();  
	glm::mat4 model;

	// Game loop
	while (!glfwWindowShouldClose(viewWindow))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		
		// Clear the colorbuffer
		glClearColor(BACK_GROUND_COLOR[0], BACK_GROUND_COLOR[1], BACK_GROUND_COLOR[2], BACK_GROUND_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Environment Render
		environmentShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)VIEW_WIDTH / (float)VIEW_HEIGHT, 0.1f, 1000.f);//frustum
		glUniformMatrix4fv(glGetUniformLocation(environmentShader.program, "model"),1,GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(environmentShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(environmentShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		environmentMesh.Draw(environmentShader);

		//Laser Render
		laserShader.use();
		glUniformMatrix4fv(glGetUniformLocation(laserShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(laserShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(laserShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform4f(glGetUniformLocation(laserShader.program, "laserColor"), LASER_COLOR[0], LASER_COLOR[1], LASER_COLOR[2], LASER_COLOR[3]);
		testLaser.Draw(laserShader);

		// Swap the screen buffers
		glfwSwapBuffers(viewWindow);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) 
		camera.ProcessKeyboard(Camera_Movement::FORWARD, 0.02f);	
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) 
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, 0.02f);
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, 0.02f);
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, 0.02f);
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, 0.02f);
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, 0.02f);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window,int button, int action, int mods) {
	camera.ProcessMousePress(button, action, mods);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

GLFWwindow* initViewWindow(){
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, "mikuLaserSimulation", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window,mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

	glEnable(GL_DEPTH_TEST);//open depth test
	return window;
}
void caculate_laser(Laser &laser) {
	vector<glm::vec3>::iterator originPointIter = laser.originPoints.begin();
	vector<glm::vec3>::iterator directIter = laser.dirctions.begin();
	while (originPointIter!=laser.originPoints.end()) {
		vector<glm::vec3>::iterator barrierTrianglesVertex = laserBarrierTriangles.begin();
		bool intersectFlag = false;
		while (barrierTrianglesVertex!=laserBarrierTriangles.end()) {
			glm::vec3 v0 = *(barrierTrianglesVertex); ++barrierTrianglesVertex;
			glm::vec3 v1 = *(barrierTrianglesVertex); ++barrierTrianglesVertex;
			glm::vec3 v2 = *(barrierTrianglesVertex); ++barrierTrianglesVertex;
			glm::vec3 orig = *(originPointIter);
			glm::vec3 dir = *(directIter);
			float t, u, v;
			if (intersectTriangle(orig, dir, v0, v1, v2, &t, &u, &v)) {
				laser.endPoints.push_back(orig + t*dir);
				intersectFlag = true;
				break;
			}else {
				continue;
			}
		}
		if (!intersectFlag) {
			laser.endPoints.push_back(10000.f*(*directIter));
		}
		++originPointIter;
		++directIter;
	}
}
// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection

bool intersectTriangle(const glm::vec3& orig, const glm::vec3& dir,
	glm::vec3& v0, glm::vec3& v1, glm::vec3& v2,
	float* t, float* u, float* v)
{
	// E1
	glm::vec3 E1 = v1 - v0;

	// E2
	glm::vec3 E2 = v2 - v0;

	// P
	glm::vec3 P = glm::cross(dir, E2);

	// determinant
	float det = glm::dot(E1, P);

	// keep det > 0, modify T accordingly
	glm::vec3 T;
	if (det >0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;

	// Calculate u and make sure u <= 1
	*u = glm::dot(T, P); 
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	glm::vec3 Q = glm::cross(T, E1);

	// Calculate v and make sure u + v <= 1
	*v = glm::dot(dir, Q); 
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = glm::dot(E2,Q);

	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}
//fill triangles to compute buffer tank
//TODO cancel buffer tank, get data dircetly from laserBarrierMesh to accelerate
void fill_barrier_tank()
{
	vector<Mesh>::iterator first = laserBarrierMesh.begin();
	int triangleNum=0;
	while (first != laserBarrierMesh.end()) {
		triangleNum += (*first).indices.size()/3;
		vector<GLuint>::iterator it = (*first).indices.begin();
		while (it!=(*first).indices.end()) {
			laserBarrierTriangles.push_back((*first).vertices[(*it)].Position);
			++it;
		}
		++first;
	}
}