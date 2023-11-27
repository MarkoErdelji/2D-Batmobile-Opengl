#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textureSampler;

void main() {
    vec4 originalColor = texture(textureSampler, TexCoord);

    vec3 modifiedColor = originalColor.rgb * 0.9;

    FragColor = vec4(modifiedColor, originalColor.a);
}