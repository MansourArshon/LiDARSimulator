#version 330 core

layout (location = 0) in vec3 aPos;

out float vHeight;

uniform mat4 uProjection;
uniform mat4 uModelView;

void main()
{
    gl_Position = uProjection * uModelView * vec4(aPos.x, aPos.y, 0.0, 1.0);
    vHeight = aPos.z;      // pass REAL elevation
}
