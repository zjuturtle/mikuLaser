
/*About coordinate system:
This program use classic OpenGL coordinate system(Right-handed System) which looks like this:

^y
|
|                (z coordinate is point to you)
|
|
------------->x

*/
#define LASER_FAR 0.f
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
#include "PointCloud.h"
#include "Model.h"
using namespace std;

// Function prototypes
void init();
GLFWwindow* initViewWindow();
GLFWwindow* initPointCloudWindow();

//call back
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//caculate laser
void caculate_laser(Laser &laser, PointCloud &pointCloud);
void fill_barrier_tank(const vector<Mesh> &meshs,const glm::mat4 &model);
void fill_barrier_tank(const Mesh &mesh, const glm::mat4 &model);
void fill_barrier_tank(const vector<Mesh> &meshs);
void fill_barrier_tank(const Mesh &mesh);
bool intersectTriangle(const glm::vec3& orig, const glm::vec3& dir,
	glm::vec3& v0, glm::vec3& v1, glm::vec3& v2,
	float* t, float* u, float* v);

// static settings
const GLuint VIEW_WIDTH = 800, VIEW_HEIGHT = 600;
const GLfloat BACK_GROUND_COLOR[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const glm::vec4 LASER_COLOR = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
const GLfloat GROUND_WIDTH = 1.0f, GROUND_HEIGHT = 1.0f;

// Shaders File Path
const char environmentVertexShaderSourceFileName[] = "shader/environment.vertexShader";
const char environmentFragmentShaderSourceFileName[] = "shader/environment.fragmentShader";
const char laserVertexShaderSourceFileName[] = "shader/laser.vertexShader";
const char laserFragmentShaderSourceFileName[] = "shader/laser.fragmentShader";
const char pointCloudVertexShaderSourceFileName[] = "shader/pointCloud.vertexShader";
const char pointCloudFragmentShaderSourceFileName[] = "shader/pointCloud.fragmentShader";
const char humanModelVertexShaderSourceFileName[] = "shader/humanModel.vertexShader";
const char humanModelFragmentShaderSourceFileName[] = "shader/humanModel.fragmentShader";

// Model file path
const char humanModelPath[] = "resource/nanosuit/nanosuit.obj";

//camera 
Camera camera(glm::vec3(-10.f,0.f,2.f));
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

//all stuff barrier triangles
vector<glm::vec3> laserBarrierTriangles;

//point cloud
PointCloud pointCloud;


// The MAIN function, from here we start the application and run the game loop
int main()
{
	//Initialize laser simulation window
	init();
	GLFWwindow* pointCloudWindow = initPointCloudWindow();

	GLFWwindow* viewWindow = initViewWindow();

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Set up view window:
	//NOTICE:function glfwMakeContextCurrent is very important during multi window,please be careful with calling sequence
	glfwMakeContextCurrent(viewWindow);

	//compile and link shader
	Shader laserShader(laserVertexShaderSourceFileName, laserFragmentShaderSourceFileName);
	Shader environmentShader(environmentVertexShaderSourceFileName, environmentFragmentShaderSourceFileName);
	Shader humanModelShader(humanModelVertexShaderSourceFileName, humanModelFragmentShaderSourceFileName);

	//load environment model
	vector<Vertex> vertices;
	vector<Texture> textures;
	vector<GLuint> indices;
	loadGroundMesh(vertices, indices, textures);
	Mesh environmentMesh(vertices, indices, textures);
	fill_barrier_tank(environmentMesh);//load environment into memory for laser compute

	//load human model
	Model humanModel("resource/nanosuit/nanosuit.obj",0.115f);//scale the human model so the height is able 1.78m

	//model matrix default to be identity
	glm::mat4 publicModelMatrix;
	glm::mat4 humanModelMatrix;

	humanModelMatrix = glm::rotate(humanModelMatrix, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
	humanModelMatrix = glm::rotate(humanModelMatrix, glm::radians(-90.f), glm::vec3(0.0f, 1.0f, 0.0f));

	//caculate and load laser
	Laser testLaser;
	GLfloat startAngle = -10.f;
	GLfloat endAngle = 45.f;
	int scanNum = 13;
	for (int i = 0; i < scanNum;i++ ) {
		GLfloat angle = (GLfloat)i / (GLfloat)scanNum*abs(endAngle-startAngle)+startAngle;
		testLaser.addLaser(glm::vec3(-1.f, 0.f, 1.f), 360, 180.f, 0.f, angle, 0.f);
	}

	fill_barrier_tank(humanModel.meshes, humanModelMatrix);
	caculate_laser(testLaser, pointCloud);
	testLaser.setupLaser();

	// Set up pointCloud window:
	glfwMakeContextCurrent(pointCloudWindow);
	Shader pointCloudShader(pointCloudVertexShaderSourceFileName, pointCloudFragmentShaderSourceFileName);
	pointCloud.setupPointCloud();

	

	//point cloud view

	// Game loop
	while (!glfwWindowShouldClose(viewWindow))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Process view window
		glfwMakeContextCurrent(viewWindow);

		// Clear the colorbuffer
		glClearColor(BACK_GROUND_COLOR[0], BACK_GROUND_COLOR[1], BACK_GROUND_COLOR[2], BACK_GROUND_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Environment Render
		environmentShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)VIEW_WIDTH / (float)VIEW_HEIGHT, 0.1f, 100.f);//frustum
		glUniformMatrix4fv(glGetUniformLocation(environmentShader.program, "model"), 1, GL_FALSE, glm::value_ptr(publicModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(environmentShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(environmentShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		environmentMesh.Draw(environmentShader);

		//Human Model Reader
		humanModelShader.use();	
		glUniformMatrix4fv(glGetUniformLocation(humanModelShader.program, "model"), 1, GL_FALSE, glm::value_ptr(humanModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(humanModelShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(humanModelShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		humanModel.Draw(humanModelShader);

		//Laser Render
		laserShader.use();
		glUniformMatrix4fv(glGetUniformLocation(laserShader.program, "model"), 1, GL_FALSE, glm::value_ptr(publicModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(laserShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(laserShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform4f(glGetUniformLocation(laserShader.program, "laserColor"), LASER_COLOR[0], LASER_COLOR[1], LASER_COLOR[2], LASER_COLOR[3]);
		testLaser.Draw(laserShader);



		// Process point cloud view window
		glfwMakeContextCurrent(pointCloudWindow);
		// Clear the colorbuffer
		glClearColor(BACK_GROUND_COLOR[0], BACK_GROUND_COLOR[1], BACK_GROUND_COLOR[2], BACK_GROUND_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// PointCloud Render
		pointCloudShader.use();
		glUniformMatrix4fv(glGetUniformLocation(pointCloudShader.program, "model"), 1, GL_FALSE, glm::value_ptr(publicModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(pointCloudShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(pointCloudShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		pointCloud.Draw(pointCloudShader);


		// Swap the view screen buffers
		glfwSwapBuffers(viewWindow);
		glfwSwapBuffers(pointCloudWindow);
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	camera.ProcessMousePress(button, action, mods);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void init() {
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

}

GLFWwindow* initViewWindow() {

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, "mikuLaserSimulation", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	// Define the viewport dimensions
	glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

	glEnable(GL_DEPTH_TEST);//open depth test
	return window;
}

GLFWwindow* initPointCloudWindow() {

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, "mikuPointCloudView", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Define the viewport dimensions
	glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

	glEnable(GL_DEPTH_TEST);//open depth test
	return window;
}

void caculate_laser(Laser &laser, PointCloud &pointCloud) {
	vector<glm::vec3>::iterator originPointIter = laser.originPoints.begin();
	vector<glm::vec3>::iterator directIter = laser.dirctions.begin();
	while (originPointIter != laser.originPoints.end()) {
		vector<glm::vec3>::iterator barrierTrianglesVertex = laserBarrierTriangles.begin();
		bool intersectFlag = false;
		int test = 0;
		GLfloat intersectDistance = 10000.f;
		while (barrierTrianglesVertex != laserBarrierTriangles.end()) {
			glm::vec3 v0 = *(barrierTrianglesVertex); ++barrierTrianglesVertex;
			glm::vec3 v1 = *(barrierTrianglesVertex); ++barrierTrianglesVertex;
			glm::vec3 v2 = *(barrierTrianglesVertex); ++barrierTrianglesVertex;
			glm::vec3 orig = *(originPointIter);
			glm::vec3 dir = *(directIter);
			float t, u, v;
			if (intersectTriangle(orig, dir, v0, v1, v2, &t, &u, &v)) {
				intersectFlag = true;
				intersectDistance = intersectDistance > t ? t : intersectDistance;
				test++;
			}
		}
		if (!intersectFlag) {
			laser.endPoints.push_back(LASER_FAR*glm::normalize(*directIter));
			(*directIter) = glm::vec3(0, 0, 0);// if no intersection, set direction to (0,0,0)
		}else {
			laser.endPoints.push_back(*(originPointIter)+intersectDistance*(*directIter));
			intersectFlag = true;
			PointCloudPoint p;
			p.Position = *(originPointIter)+intersectDistance*(*directIter);
			p.Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			pointCloud.points.push_back(p);
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
	if (det < 0.000001f)
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
	*t = glm::dot(E2, Q);


	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;
	if (*t < 0)
		return false;
	return true;
}
//fill triangles to compute buffer tank
//TODO cancel buffer tank, get data dircetly from laserBarrierMesh to accelerate
void fill_barrier_tank(const vector<Mesh> &meshs)
{
	vector<Mesh>::const_iterator first = meshs.begin();
	int triangleNum = 0;
	while (first != meshs.end()) {
		triangleNum += (*first).indices.size() / 3;
		vector<GLuint>::const_iterator it = (*first).indices.begin();
		while (it != (*first).indices.end()) {

			laserBarrierTriangles.push_back((*first).vertices[(*it)].Position);
			++it;
		}
		++first;
	}
}

void fill_barrier_tank(const vector<Mesh> &meshs,const glm::mat4 &model)
{
	vector<Mesh>::const_iterator first = meshs.begin();
	int triangleNum = 0;
	while (first != meshs.end()) {
		triangleNum += (*first).indices.size() / 3;
		vector<GLuint>::const_iterator it = (*first).indices.begin();
		while (it != (*first).indices.end()) {
			glm::vec4 tmp((*first).vertices[(*it)].Position, 1.0);
			glm::vec4 tmp2;
			tmp2 = model*tmp;
			laserBarrierTriangles.push_back(glm::vec3(tmp2.x / tmp2.w, tmp2.y / tmp2.w, tmp2.z / tmp2.w));
			++it;
		}
		++first;
	}
}

void fill_barrier_tank(const Mesh &mesh)
{
	int triangleNum = 0;
	triangleNum += mesh.indices.size() / 3;
	vector<GLuint>::const_iterator it = mesh.indices.begin();
	while (it != (mesh).indices.end()) {
		glm::vec4 tmp((mesh).vertices[(*it)].Position, 1.0);
		laserBarrierTriangles.push_back((mesh).vertices[(*it)].Position);
		++it;
	}
}

void fill_barrier_tank(const Mesh &mesh, const glm::mat4 &model)
{
	int triangleNum = 0;
	triangleNum += mesh.indices.size() / 3;
	vector<GLuint>::const_iterator it = mesh.indices.begin();
	while (it != (mesh).indices.end()) {
		glm::vec4 tmp((mesh).vertices[(*it)].Position,1.0);
		glm::vec4 tmp2;
		tmp2 = model*tmp;
		laserBarrierTriangles.push_back(glm::vec3(tmp2.x/tmp2.w, tmp2.y/tmp2.w, tmp2.z/tmp2.w));
		++it;
	}
}