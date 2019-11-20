#version 150
in vec4 vertex_position;
in vec4 vertex_color;
out vec4 vcolor;
uniform mat4 M;

void main()  {
    gl_Position = M * vertex_position;
    vcolor = vertex_color;
}
