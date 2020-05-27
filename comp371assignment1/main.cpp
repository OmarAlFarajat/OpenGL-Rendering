// Assignment 3, Omar Al-Farajat, 29603387
// Modified from Lab 1, "skeleton_code", COMP 371 Section UU, Fall 2018, Concordia
// ^ In turn, modified from http://learnopengl.com/
// Using code exerpts from http://opengl-tutorial.org/, indicated by comment marker: [*]
// Also using code exerpts from modified from http://learnopengl.com/

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "..\glew\glew.h"

#include "..\glfw\glfw3.h"

#include <..\glm\glm.hpp>
#include <..\glm\gtc\matrix_transform.hpp>
#include <..\glm\gtx\transform.hpp>
#include <..\glm\gtc\type_ptr.hpp>
using namespace glm;

#include "objloader.h"
#include "shaloader.h"
#include "vboindexer.hpp"
#include "texture.hpp"

GLFWwindow* window;
const GLuint WIDTH = 800, HEIGHT = 800;
static glm::vec2 lookPosition = vec2(800.0f / 2.0f, 800.0f / 2.0f);
static glm::mat4 Projection = glm::perspective(glm::radians(75.0f), 1.0f, 0.1f, 300.0f);
static glm::vec3 CameraPosition = vec3(0, 2.0f, 10.0f);
static glm::vec3 direction = vec3(0, 0.0f, 0);
static glm::vec3 up = vec3(0, 1, 0);
static float hAngle = 3.14f;
static float vAngle = 0.0f;
static glm::mat4 View;
static glm::mat4 Model;	
static bool FlagA = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	// Scale -10%
	if (key == GLFW_KEY_O && action == GLFW_RELEASE)
		Model = glm::scale(vec3(0.9f, 0.9f, 0.9f))*Model;
	// Scale +10%
	if (key == GLFW_KEY_P && action == GLFW_RELEASE)
		Model = glm::scale(vec3(1.1f, 1.1f, 1.1f))*Model;
	// Toggle Scene A
	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
		FlagA = true;
		std::cout << "SCENE: A, 4 Point Lights" << std::endl;
	}
	// Toggle Scene B
	if (key == GLFW_KEY_F2 && action == GLFW_RELEASE) {
		FlagA = false;
		std::cout << "SCENE: B, 1 Point Light, Shadow Mapping" << std::endl;
	}
}

// Check user inputs and apply transformations to model. Smoother as a simple void function. 
static void CheckInputs() {
	// Look up
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lookPosition.y -= 0.05f;
	// Look down
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lookPosition.y += 0.05f;
	// Look right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lookPosition.x += 0.05f;
	// Look left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lookPosition.x -= 0.05f;

	// Same look code from assignments 1 and 2
	hAngle += 0.1f * float(800 / 2 - lookPosition.x);
	vAngle += 0.1f * float(800 / 2 - lookPosition.y);

	lookPosition = vec2(800.0f / 2.0f, 800.0f / 2.0f);

	direction = vec3(
		cos(vAngle) * sin(hAngle),
		sin(vAngle),
		cos(vAngle) * cos(hAngle));

	glm::vec3 right = glm::vec3(
		sin(hAngle - 3.14f / 2.0f),
		0,
		cos(hAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::cross(right, direction);

	// Click and drag to move in
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	glfwSetCursorPos(window, 800 / 2, 800 / 2);

	if (ypos > 402)
		CameraPosition -= direction * 0.0375f;
	else if(ypos < 398)
		CameraPosition += direction * 0.0375f;
	}

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		CameraPosition += direction*0.0375f;
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
		CameraPosition -= direction *0.0375f;
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		CameraPosition += right *0.0375f;
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
		CameraPosition -= right *0.0375f;

	// Translate +X
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		Model = glm::translate(vec3(0.05, 0, 0))*Model;
	// Translate -X
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		Model = glm::translate(vec3(-0.05, 0, 0))*Model;
	// Translate +Y
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		Model = glm::translate(vec3(0, 0.05, 0))*Model;
	// Translate -Y
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		Model = glm::translate(vec3(0, -0.05, 0))*Model;
	// Translate +Z
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		Model = glm::translate(vec3(0, 0, 0.05))*Model;
	// Translate -Z
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		Model = glm::translate(vec3(0, 0, -0.05))*Model;
	// Rotate X
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		Model = glm::rotate(0.025f, vec3(1, 0, 0))*Model;
	// Rotate Y
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		Model = glm::rotate(0.025f, vec3(0, 1, 0))*Model;
	// Rotate Z
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		Model = glm::rotate(0.025f, vec3(0, 0, 1))*Model;

	// RESET MODEL
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		Model = glm::mat4(1.0f);
	}

	// RESET CAMERA
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		CameraPosition = vec3(0, 1.5f, 10.0f);
		//direction = vec3(0, 0, 0);
		up = vec3(0, 1, 0);
		// Initial horizontal angle : toward -Z
		hAngle = glm::radians(180.0f);
		// Initial vertical angle : none
		vAngle = glm::radians(0.0f);
	}

	View = glm::lookAt(CameraPosition, CameraPosition + direction, up);

}

int main(int argc, char* argv[])
{

	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

	// [*] Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_SAMPLES, 4);	//4x anti-aliasing for smoother edges
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Ensures that legacy OpenGL functions are not accessible and thereby forcing retained mode. 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "COMP 371 Assignment 3", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		getchar();
		glfwTerminate();
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set background color to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enables depth test and sets to GL_LESS to only render objects viewable closest to the camera
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Enables face culling and set to GL_BACK, meaning that faces with their normals not facing the camera are not rendered.
	glEnable(GL_CULL_FACE);

	// Define our vertex array buffer, generate 1 of them, and bind it. 
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint depthShader = loadSHADER("depthVertex.shader", "depthFragment.shader");
	
	// Get a handle for our "MVP" uniform
	GLuint depthMatrixID = glGetUniformLocation(depthShader, "depthMVP");

	// Load vertex and fragment shaders
	GLuint shader = loadSHADER("vertex.shader", "fragment.shader");

	// Get a handles for uniforms to be sent to the shaders
	GLuint model = glGetUniformLocation(shader, "Model");
	GLuint view = glGetUniformLocation(shader, "View");
	GLuint projection = glGetUniformLocation(shader, "Projection");
	GLuint cameraPosition = glGetUniformLocation(shader, "CameraPosition");
	GLuint flagA = glGetUniformLocation(shader, "FlagA");

	// Load the texture
	GLuint Texture = loadBMP_custom("../whitemarble.bmp");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(shader, "myTextureSampler");

	// Load the appropriate obj data into our vec3 arrays
	// Take our unindexed obj data and use indexVBO to index our data
	// IMPORTANT NOTE: The obj file loaded before NEEDS to have uv data, even if textures aren't being used, otherwise indexVBO will fail. 
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;		//unused in this program, but loadOBJ requires it as an input parameter 
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;	//unused in this program, but indexVBO requires it as an input parameter
	std::vector<glm::vec3> indexed_normals;
	loadOBJ("../scene.obj", vertices, normals, uvs);
	// OBJ Source: https://www.turbosquid.com/FullPreview/Index.cfm/ID/942377
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Define the index buffer, generate 1 of them, bind it, and load the indexed vertices to it. 
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	// Define the normal buffer, genrate 1 of them, bind it, and load the indexed normals to it. 
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Define the element buffer, generate 1 of them, bind it, and load the indices to it. 
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// [*] 
	// ---------------------------------------------
	// Render to Texture - specific code begins here
	// ---------------------------------------------

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	GLuint shadowRes = 2048; 

	// [*] Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowRes, shadowRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;


	GLuint DepthBiasID = glGetUniformLocation(shader, "DepthBiasMVP");
	GLuint ShadowMapID = glGetUniformLocation(shader, "shadowMap");

	// Sets the swap buffer interval to lock with the screen's refresh rate
	glfwSwapInterval(1);

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// [*] FPS Counter
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {
			printf("%.0f fps\n", double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		CheckInputs();

// FIRST PASS

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render to our framebuffer 
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, shadowRes, shadowRes); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); 

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(depthShader);

		// "Not-a-Spot-light", i.e. a point light with projecting to the origin of the scene.
		glm::vec3 lightPos(0, 20, 10);
		glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 500.0f);
		glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0, 1, 0));

		// depth matrix multiplications
		glm::mat4 depthModelMatrix = Model;
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// 1st Pass Draw Call, Draw to Depth Buffer
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

		glDisableVertexAttribArray(0);


		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 800, 800); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use our shader
		glUseProgram(shader);

		CheckInputs();

		// [*] Will be used to realize the depthMVP, in turn giving us shadow coordinates to be used in second pass
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(model, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(view, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(projection, 1, GL_FALSE, &Projection[0][0]);

		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);
		// same for 1 now, but the texture in this case is a shadow map of uniform type sampler2DShadow
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(ShadowMapID, 1);
		
// SECOND PASS

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind our above loaded shaders
		glUseProgram(shader);

		glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(Projection));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(Model));
		glUniform3fv(cameraPosition, 1, glm::value_ptr(CameraPosition));
		glUniform1i(flagA, FlagA);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// 2nd Pass Draw Call, business as usual
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();

	}

	// Cleanup buffers and shaders, good practice. 
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(shader);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();


	return 0;
}
