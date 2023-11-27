#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 rectPos; 
out vec4 fragColor;

void main()
{
    gl_Position = vec4(aPos+rectPos, 0.0, 1.0);
    fragColor = fragColor; 
}
