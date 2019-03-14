#version 450

layout (location = 0) in vec3 position;
out vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

void main()
{
    gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1.0); 
    color = vec4(1.0, 1.0, 0.0, 1.0);
}
