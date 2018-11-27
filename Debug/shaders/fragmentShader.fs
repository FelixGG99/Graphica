#version 400 core
in vec3 vertexColor;
out vec4 fragColor;
uniform vec4 ourColor;

void main()
{
	fragColor=vec4(vertexColor,1.0f)*ourColor;
}