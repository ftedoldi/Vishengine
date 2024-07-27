#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D TextureDiffuse0;

uniform bool HasTextureDiffuse;
uniform bool HasTextureSpecular;

uniform vec4 DiffuseColor;
uniform vec3 SpecularColor;

void main() {
    vec4 color = HasTextureDiffuse ? texture(TextureDiffuse0, TexCoord) : DiffuseColor;

    FragColor = color;
} 