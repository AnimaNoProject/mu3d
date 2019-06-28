#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

out vec4 fragColor;

void main()
{
    gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1.0); 
    fragColor = vec4(color, 0.3);
}
