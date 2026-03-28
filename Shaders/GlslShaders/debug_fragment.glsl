#version 460 core

out vec4 FragColor;

uniform vec3 DebugColor;

void main() {
    FragColor = vec4(DebugColor, 1.0);
}
