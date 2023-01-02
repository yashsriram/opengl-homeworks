#version 150
in vec4 vcolor;
out vec4 color;

void main() {
	// set output color to interpolated color from vshader
	color = vcolor;
}