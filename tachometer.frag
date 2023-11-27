#version 330 core

out vec4 fragColor;

uniform vec4 needleColor; 

void main()
{
    fragColor = needleColor;
}
