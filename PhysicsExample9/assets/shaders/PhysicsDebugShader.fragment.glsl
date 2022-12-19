#version 400

// in from vertex shader
in vec4 ex_Position;
in vec4 ex_PositionWorld;
in vec4 ex_Normal;
in vec4 ex_UV_x2;

// Default colour output
layout ( location = 0 ) out vec4 out_Colour;

// Color
uniform vec3 Color;

void main(void)
{
	out_Colour = vec4(Color.rgb, 1);
}
