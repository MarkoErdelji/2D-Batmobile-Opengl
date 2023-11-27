#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform vec2 transform;


void main() {
    gl_Position = vec4(aPos + transform, 0.0, 1);
        gl_PointSize = 10;
    TexCoord = aTexCoord;
}
