#version 150
in vec4 vertexPosition;
in vec4 vertexColor;
out vec4 vcolor;
uniform mat4 M;

void main()  {
    gl_Position = M * vertexPosition;
    vcolor = vertexColor;
}
