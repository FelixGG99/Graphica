#version 330 core
out vec4 color;

uniform vec4 pickingColor; //color determinado a partir del ID del objeto
void main()
{
	color=pickingColor;
}