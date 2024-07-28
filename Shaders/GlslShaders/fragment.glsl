#version 460 core

in vec2 TexCoord;
in vec3 VertexNormal;
in vec3 FragViewPosition;
in vec3 NormalViewPosition;

out vec4 FragColor;

uniform sampler2D TextureDiffuse0;

uniform bool HasTextureDiffuse;
uniform bool HasTextureSpecular;

uniform vec4 DiffuseColor;
uniform vec3 SpecularColor;

uniform vec3 LightColor;

uniform vec3 LightPosition;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;

    vec3 normalizedVertexNormal = normalize(NormalViewPosition);
    vec3 lightDirection = normalize(LightPosition - FragViewPosition);

    // Calculate the angle between the normal of the vertex and the light direction
    // to get how strong the light should be.
    float diff = max(dot(normalizedVertexNormal, lightDirection), 0.0);
    vec3 diffuse = diff * LightColor;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(-FragViewPosition);
    //vec3 reflectDir = reflect(-lightDirection, normalizedVertexNormal);

    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(normalizedVertexNormal, halfwayDir), 0.0), 32.0);

    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * LightColor;

    vec4 color = HasTextureDiffuse ? texture(TextureDiffuse0, TexCoord) : DiffuseColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0) * color;
} 