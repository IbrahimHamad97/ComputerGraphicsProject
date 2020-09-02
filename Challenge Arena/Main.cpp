//
//  main.cpp
//  Window
//
//  Created by Osama Halabi on 2019/06/27.
//  Copyright © 2019 Osama Halabi. All rights reserved.
//
// creat window.cpp
// Getting started with OpenGL, GLFW, and GLEW setting
#include <stdio.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Utils.h"
#include "Camera.h"

#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window ヂメンシオンs
const GLint WIDTH = 1200, HEIGHT = 900;

// all mouse keyboard stuff are in GLFW
// keyboard functions prototypes
void keyCallback(GLFWwindow *window, int key, int scancode,
	int action, int mode);
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovements();

// camera's initial position
Camera camera(glm::vec3(10.0f, 10.0f, 350.0f));

// how much it is moving (we subtract the new position by the old)
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;

// if the key is pressed we check if it is true or false
bool keys[1024];

// first click of the mouse (to avoid overshooting)
bool mouseClick = true;

// claculate delta time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// exp with translation (make it bigger until it reaches a limit)
bool transDir; //dir positive => increase and the opp
float offset = 0.0f; //pass to matrix
float maxOffset = 98.0f;
float increment = 0.75f;

//exp with rotation 
float curAngle = 0.0f;

//exp with scalling 
bool sizeDir = true;
float curSize = 2.0f;
float maxSize = 4.0f;
float minSize = 0.5f;

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW initialisation failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile = No Backwards Compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow Forward Compatbility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create the window
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Ibrahim Hamad Project", nullptr, nullptr);

	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();

		return EXIT_FAILURE;
	}

	// Get Buffer Size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSwapInterval(1);
	// callback functions from glfw
	glfwSetKeyCallback(mainWindow, keyCallback);
	glfwSetCursorPosCallback(mainWindow, MouseCallback);
	glfwSetScrollCallback(mainWindow, ScrollCallback);

	// depth test
	glEnable(GL_DEPTH_TEST);

	// this shader is for the main box which contains the arena rings
	Shader arenaShader("res/shaders/arena.vert", "res/shaders/arena.frag");

	// for the rings inside the arena
	Shader arenaRingsShader("res/shaders/arenaRings.vert", "res/shaders/arenaRings.frag");

	// for the four light boxes that we have
	Shader boxLightShader("res/shaders/boxLight.vert", "res/shaders/boxLight.frag");

	// position normal texture
	// arena details
	GLfloat arenaVertices[] = {
		-100.0f,  100.0f,  300.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-100.0f,  100.0f, -300.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-100.0f, -100.0f, -300.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-100.0f, -100.0f, -300.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-100.0f, -100.0f,  300.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-100.0f,  100.0f,  300.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 100.0f,  100.0f,  300.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 100.0f,  100.0f, -300.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 100.0f, -100.0f, -300.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 100.0f, -100.0f, -300.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 100.0f, -100.0f,  300.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 100.0f,  100.0f,  300.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-100.0f, -100.0f, -300.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 100.0f, -100.0f, -300.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 100.0f, -100.0f,  300.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 100.0f, -100.0f,  300.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-100.0f, -100.0f,  300.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-100.0f, -100.0f, -300.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

		-100.0f,  100.0f, -300.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 100.0f,  100.0f, -300.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 100.0f,  100.0f,  300.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 100.0f,  100.0f,  300.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-100.0f,  100.0f,  300.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-100.0f,  100.0f, -300.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
	};

	// rings details
	GLfloat ringsVertices[] = {
		-5.0f,  5.0f,  5.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-5.0f,  5.0f, -5.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-5.0f, -5.0f,  5.0f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-5.0f,  5.0f,  5.0f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 5.0f,  5.0f,  5.0f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 5.0f,  5.0f, -5.0f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-5.0f, -5.0f, -5.0f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 5.0f, -5.0f,  5.0f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-5.0f, -5.0f,  5.0f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-5.0f, -5.0f, -5.0f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-5.0f,  5.0f, -5.0f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	     5.0f,  5.0f, -5.0f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 	 5.0f,  5.0f,  5.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-5.0f,  5.0f,  5.0f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-5.0f,  5.0f, -5.0f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// roof lights details
	GLfloat lightBoxesVertices[] = {
		-5.0f, -5.0f, -5.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 5.0f, -5.0f, -5.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 5.0f,  5.0f, -5.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 5.0f,  5.0f, -5.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-5.0f,  5.0f, -5.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-5.0f, -5.0f, -5.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 			   
		-5.0f, -5.0f,  5.0f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 5.0f, -5.0f,  5.0f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 5.0f,  5.0f,  5.0f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-5.0f,  5.0f,  5.0f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-5.0f, -5.0f,  5.0f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-5.0f,  5.0f,  5.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-5.0f,  5.0f, -5.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-5.0f, -5.0f, -5.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-5.0f, -5.0f, -5.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-5.0f, -5.0f,  5.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-5.0f,  5.0f,  5.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 5.0f,  5.0f,  5.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 5.0f,  5.0f, -5.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-5.0f, -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 5.0f, -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 5.0f, -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-5.0f, -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-5.0f, -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-5.0f,  5.0f, -5.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 5.0f,  5.0f, -5.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 5.0f,  5.0f,  5.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 5.0f,  5.0f,  5.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-5.0f,  5.0f,  5.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-5.0f,  5.0f, -5.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// positions of the rings
	glm::vec3 ringsPositions[] = {
		glm::vec3(10.0f, 10.0f, 250.0f),
		glm::vec3(70.0f, -70.0f, 200.0f),
		glm::vec3(-70.0f, 70.0f, 150.0f),
		glm::vec3(10.0f, 10.0f, 100.0f),
		glm::vec3(30.0f, 30.0f, 50.0f),
		glm::vec3(10.0f, 10.0f, 0.0f),
		glm::vec3(-30.0f, -30.0f, -50.0f),
		glm::vec3(10.0f, 10.0f, -100.0f),
		glm::vec3(70.0f, 70.0f, -150.0f),
		glm::vec3(-70.0f, -70.0f, -200.0f),
		glm::vec3(10.0f, 10.0f, -250.0f),
		glm::vec3(10.0f, 10.0f, -300.0f),
	};

	// roof lights positions
	glm::vec3 pointLightPositions[] = {
		glm::vec3(10.0f, 90.0f, 175.0f),
		glm::vec3(10.0f, 90.0f, 50.0f),
		glm::vec3(10.0f, 90.0f, -75.0f),
		glm::vec3(10.0f, 90.0f, -200.0f)
	};

	// roof lights colors
	glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 0.6f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0, 0.0),
		glm::vec3(0.2f, 0.2f, 1.0f)
	};

	//generate vertex array (used different VAO and VBO for each vertices)
	GLuint arenaVAO, arenaVBO;
	glGenVertexArrays(1, &arenaVAO);
	glGenBuffers(1, &arenaVBO);
	glBindBuffer(GL_ARRAY_BUFFER, arenaVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arenaVertices), arenaVertices, GL_STATIC_DRAW);
	glBindVertexArray(arenaVAO);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)
		(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)
		(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	//unibnd after use
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// rings VAO and VBO
	GLuint ringsVAO, ringsVBO;
	glGenVertexArrays(1, &ringsVAO);
	glGenBuffers(1, &ringsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ringsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ringsVertices), ringsVertices, GL_STATIC_DRAW);
	glBindVertexArray(ringsVAO);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)
		(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)
		(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	//unibnd after use
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// lights data
	GLuint lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightBoxesVertices), lightBoxesVertices, GL_STATIC_DRAW);
	glBindVertexArray(lightVAO);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)
		(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)
		(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	//unibnd after use
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// loading the textures for the arena
	arenaShader.Use();
	GLuint arenaDiffuseMap = Utils::loadTexture("res/images/crate2_diffuse.png");
	glUniform1i(glGetUniformLocation(arenaShader.Program, "material.diffuse"), 0);
	GLuint arenaSpecularMap = Utils::loadTexture("res/images/crate2_bump.png");
	glUniform1i(glGetUniformLocation(arenaShader.Program, "material.specular"), 1);
	
	// loading the textures for the rings
	arenaRingsShader.Use();
	GLuint ringsDiffuseMap = Utils::loadTexture("res/images/crate2_diffuse.png");
	glUniform1i(glGetUniformLocation(arenaRingsShader.Program, "material.diffuse"), 2);
	GLuint ringsSpecularMap = Utils::loadTexture("res/images/crate2_bump.png");
	glUniform1i(glGetUniformLocation(arenaRingsShader.Program, "material.specular"), 3);

	// Loop until window closed (Game loop)
	while (!glfwWindowShouldClose(mainWindow))
	{
		// frames for smooth animation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Get + Handle user input events
		glfwPollEvents();
		DoMovements();

		//Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		//cleat the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// passing arena the values to the shader 
		arenaShader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(camera.GetZoom(),
			(GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.f);

		GLint modelLoc = glGetUniformLocation(arenaShader.Program,"model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GLint viewlLoc = glGetUniformLocation(arenaShader.Program,"view");
		glUniformMatrix4fv(viewlLoc, 1, GL_FALSE, glm::value_ptr(view));
		GLint projectionLoc = glGetUniformLocation(arenaShader.Program,"projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, arenaDiffuseMap);

		// bind texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, arenaSpecularMap);

		glUniform1f(glGetUniformLocation(arenaShader.Program,
			"material.shininess"), 56.0f);

		glUniform3f(glGetUniformLocation(arenaShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "dirLight.ambient"),  0.75f, 0.75f, 0.75f);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "dirLight.diffuse"),  1.5f, 1.5f, 1.5f);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "dirLight.specular"), 0.75f, 0.75f, 0.75f);
										 
		// Point light 1				 						 
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[0].ambient"),  pointLightColors[0].x * 75, pointLightColors[0].y * 75, pointLightColors[0].z * 75);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[0].diffuse"),  pointLightColors[0].x * 75, pointLightColors[0].y * 75, pointLightColors[0].z * 75);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[0].specular"), pointLightColors[0].x * 75, pointLightColors[0].y * 75, pointLightColors[0].z * 75);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[0].quadratic"), 0.032);
										
		// Point light 2				 					
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[1].ambient"),  pointLightColors[1].x * 100, pointLightColors[1].y * 100, pointLightColors[1].z * 100);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[1].diffuse"),  pointLightColors[1].x * 100, pointLightColors[1].y * 100, pointLightColors[1].z * 100);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[1].specular"), pointLightColors[1].x * 100, pointLightColors[1].y * 100, pointLightColors[1].z * 100);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[1].quadratic"), 0.032);
										
		// Point light 3				 
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[2].ambient"),  pointLightColors[2].x * 75, pointLightColors[2].y * 75, pointLightColors[2].z * 75);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[2].diffuse"),  pointLightColors[2].x * 75, pointLightColors[2].y * 75, pointLightColors[2].z * 75);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[2].specular"), pointLightColors[2].x * 75, pointLightColors[2].y * 75, pointLightColors[2].z * 75);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[2].quadratic"), 0.032);
										
		// Point light 4				 
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[3].ambient"),  pointLightColors[3].x * 100, pointLightColors[3].y * 100, pointLightColors[3].z * 100);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[3].diffuse"),  pointLightColors[3].x * 100, pointLightColors[3].y * 100, pointLightColors[3].z * 100);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "pointLights[3].specular"), pointLightColors[3].x * 100, pointLightColors[3].y * 100, pointLightColors[3].z * 100);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "pointLights[3].quadratic"), 0.032);
										 
		// SpotLight					 
		glUniform3f(glGetUniformLocation(arenaShader.Program, "spotLight.position"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "spotLight.direction"), camera.getFront().x, camera.getFront().y, camera.getFront().z);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "spotLight.ambient"),  3.0f, 3.0f, 3.0f);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "spotLight.diffuse"),  3.8f, 3.8f, 3.0f);
		glUniform3f(glGetUniformLocation(arenaShader.Program, "spotLight.specular"), 30.8f, 30.8f, 30.0f);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "spotLight.linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "spotLight.quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(arenaShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(arenaShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(13.0f)));

		glBindVertexArray(arenaVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// rings settings
		arenaRingsShader.Use();
		viewlLoc = glGetUniformLocation(arenaRingsShader.Program, "view");
		glUniformMatrix4fv(viewlLoc, 1, GL_FALSE, glm::value_ptr(view));
		projectionLoc = glGetUniformLocation(arenaRingsShader.Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// bind texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, ringsDiffuseMap);

		// bind texture
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ringsSpecularMap);

		glUniform1f(glGetUniformLocation(arenaRingsShader.Program,
			"material.shininess"), 56.0f);

		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "dirLight.ambient"), 0.3f, 0.24f, 0.14f);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "dirLight.diffuse"), 1.7f, 1.42f, 1.26f);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		
		// Point light 1				 									 
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].ambient"),  pointLightColors[0].x * 200, pointLightColors[0].y * 200, pointLightColors[0].z * 75);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].diffuse"),  pointLightColors[0].x * 200, pointLightColors[0].y * 200, pointLightColors[0].z * 75);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].specular"), pointLightColors[0].x * 200, pointLightColors[0].y * 200, pointLightColors[0].z * 75);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[0].quadratic"), 0.032);
		
		// Point light 2				 								
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].ambient"),  pointLightColors[1].x * 300, pointLightColors[1].y * 300, pointLightColors[1].z * 300);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].diffuse"),  pointLightColors[1].x * 300, pointLightColors[1].y * 300, pointLightColors[1].z * 300);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].specular"), pointLightColors[1].x * 300, pointLightColors[1].y * 300, pointLightColors[1].z * 300);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[1].quadratic"), 0.032);

		// Point light 3				 
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].ambient"),  pointLightColors[2].x * 200, pointLightColors[2].y * 200, pointLightColors[2].z * 200);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].diffuse"),  pointLightColors[2].x * 200, pointLightColors[2].y * 200, pointLightColors[2].z * 200);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].specular"), pointLightColors[2].x * 200, pointLightColors[2].y * 200, pointLightColors[2].z * 200);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[2].quadratic"), 0.032);

		// Point light 4				 
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].ambient"),  pointLightColors[3].x * 300, pointLightColors[3].y * 300, pointLightColors[3].z * 300);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].diffuse"),  pointLightColors[3].x * 300, pointLightColors[3].y * 300, pointLightColors[3].z * 300);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].specular"), pointLightColors[3].x * 300, pointLightColors[3].y * 300, pointLightColors[3].z * 300);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "pointLights[3].quadratic"), 0.032);
		
		// SpotLight					 
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.position"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.direction"), camera.getFront().x, camera.getFront().y, camera.getFront().z);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.ambient"),  3.0f, 3.0f, 3.0f);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.diffuse"),  3.8f, 3.8f, 3.0f);
		glUniform3f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.specular"), 30.8f, 30.8f, 30.0f);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.linear"), 0.09);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(arenaRingsShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(13.0f)));

		// different transforms to different rings to give it a lot of effects
		if (transDir)
			offset += increment;
		else
			offset -= increment;

		if (abs(offset) >= maxOffset)
			transDir = !transDir;

		if (sizeDir)
			curSize += 0.02f;
		else
			curSize -= 0.02f;

		if (curSize >= maxSize || curSize <= minSize)
			sizeDir = !sizeDir;

		// for each ring we have different settings
		// that is why we have different conditions
		glBindVertexArray(ringsVAO);
		for (unsigned int i = 0; i < 12; i++)
		{
			// this gets scaled
			if (ringsPositions[i] == glm::vec3(10.0f, 10.0f, 250.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(ringsPositions[i].x, ringsPositions[i].y, ringsPositions[i].z));
				model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);

				// tried different ways to make the interaction work but didn't
				// you can see here many ways of trying but none worked ;-;
				if (camera.getPosition() == ringsPositions[i])
					glfwSetWindowShouldClose(mainWindow, GL_TRUE);
				if (camera.getPosition().z == ringsPositions[i].z)
					glfwSetWindowShouldClose(mainWindow, GL_TRUE);
				if (camera.getFront() == ringsPositions[i])
					glfwSetWindowShouldClose(mainWindow, GL_TRUE);
				if (camera.getFront().z == ringsPositions[i].z)
					glfwSetWindowShouldClose(mainWindow, GL_TRUE);
				if (camera.getPosition().x == ringsPositions[i].x &&
					camera.getPosition().y == ringsPositions[i].y &&
					camera.getPosition().z == ringsPositions[i].z) {
					glfwSetWindowShouldClose(mainWindow, GL_TRUE);
				}
			}

			else if (ringsPositions[i] == glm::vec3(10.0f, 10.0f, -100.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(offset, ringsPositions[i].y, ringsPositions[i].z));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			else if (ringsPositions[i] == glm::vec3(10.0f, 10.0f, 100.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(ringsPositions[i].x, offset, ringsPositions[i].z));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// those get rotated
			else if (ringsPositions[i] == glm::vec3(30.0f, 30.0f, 50.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(ringsPositions[i].x, ringsPositions[i].y, ringsPositions[i].z));
				model = glm::rotate(model, (GLfloat)glfwGetTime() * -2.0f, 
					glm::vec3(1.0f,1.0f,0.0f));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			else if (ringsPositions[i] == glm::vec3(10.0f, 10.0f, 0.0f)){
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(ringsPositions[i].x, ringsPositions[i].y, ringsPositions[i].z));
				model = glm::scale(model, glm::vec3(1.0f, curSize, 1.0f));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			else if (ringsPositions[i] == glm::vec3(-30.0f, -30.0f, -50.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(ringsPositions[i].x, ringsPositions[i].y, ringsPositions[i].z));
				model = glm::rotate(model, (GLfloat)glfwGetTime() * 2.0f,
					glm::vec3(0.0f, 1.0f, 1.0f));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			else if (ringsPositions[i] == glm::vec3(10.0f, 10.0f, -250.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(offset, offset, ringsPositions[i].z));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			else if (ringsPositions[i] == glm::vec3(10.0f, 10.0f, -300.0f)) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model,
					glm::vec3(ringsPositions[i].x, ringsPositions[i].y, ringsPositions[i].z));
				model = glm::scale(model, glm::vec3(curSize, curSize, curSize/2));
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			else {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, ringsPositions[i]);
				modelLoc = glGetUniformLocation(arenaRingsShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

		}
		glBindVertexArray(0);

		// the lamps settings
		boxLightShader.Use();
		viewlLoc = glGetUniformLocation(boxLightShader.Program, "view");
		glUniformMatrix4fv(viewlLoc, 1, GL_FALSE, glm::value_ptr(view));
		projectionLoc = glGetUniformLocation(boxLightShader.Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			GLint modelLoc = glGetUniformLocation(boxLightShader.Program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(glGetUniformLocation(boxLightShader.Program,
				"lightColor"), pointLightColors[i].x,
				pointLightColors[i].y, pointLightColors[i].z);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		glfwSwapBuffers(mainWindow);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	glDeleteVertexArrays(1, &arenaVAO);
	glDeleteVertexArrays(1, &ringsVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &arenaVBO);
	glDeleteBuffers(1, &ringsVBO);
	glDeleteBuffers(1, &lightVBO);
	return EXIT_SUCCESS;
}

// handle keys here
void DoMovements()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE){
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (mouseClick)
	{
		lastX = xPos;
		lastY = yPos;
		mouseClick = false;
	}
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}