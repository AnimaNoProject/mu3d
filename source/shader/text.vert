#version 450
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 orgModel;

void main()
{
    //gl_Position = projection * orgModel * model * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = projection * orgModel * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}  
