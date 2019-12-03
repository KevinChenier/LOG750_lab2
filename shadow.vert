#version 400 core
uniform mat4 mvpMatrix;
in vec4 vPosition;
out vec4 fPosition;

void main()
{
    fPosition = mvpMatrix * vPosition;
    gl_Position = mvpMatrix * vPosition;
}
