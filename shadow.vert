#version 400 core

uniform mat4 mvpMatrix;     // model-view-projection matrix
in vec4 vPosition;          // input vertex position
out vec4 fPosition;         // output vertex position

// A pass through vertex shader.
void main()
{
    fPosition = mvpMatrix * vPosition;
    gl_Position = mvpMatrix * vPosition;
}
