#version 330

out vec4 color;
uniform vec3 lightColor;

void main(void)
{ 
	color = vec4(lightColor, 1.0f);
}; 