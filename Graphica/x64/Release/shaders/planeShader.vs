#version 400 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Color;

out vec3 vertexColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(Pos, 1.0);
	vertexColor = Color;
}