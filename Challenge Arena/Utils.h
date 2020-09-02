#ifndef Utils_h
#define Utils_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>

class Utils
{
	public:
		Utils();
		static GLuint loadTexture(const char * textImagePath);
};
#endif // !1