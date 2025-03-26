#version 410 core

in vec2 vTexCoord;
out vec4 color;

uniform sampler2D uColorImage;

void main() {
    color = texture(uColorImage, vTexCoord);
}