#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

Utils::Utils() {}
unsigned int Utils::loadTexture(const char * textImagePath)
{
	GLuint textureID; //for the id
	int width, height, noOfComponents; //noOfComp for RGB or RGBA

	//load image
	unsigned char *textdata = stbi_load(textImagePath, &width, &height
		, &noOfComponents, 0);

	if (textdata) {
		GLenum format;
		if (noOfComponents == 1)
			format = GL_RED;
		else if (noOfComponents == 3)
			format = GL_RGB;
		else if (noOfComponents == 4)
			format = GL_RGBA;

		glGenTextures(1, &textureID); //type and place to store returned id
		glBindTexture(GL_TEXTURE_2D, textureID);

		//set up params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
			GL_UNSIGNED_BYTE, textdata);
		glGenerateMipmap(GL_TEXTURE_2D);

		//generate texture 2D
		stbi_image_free(textdata);
	}

	else //check for error
	{
		printf("Failed to find texture: %s ", textImagePath);
		stbi_image_free(textdata);
	}

	return textureID;
}