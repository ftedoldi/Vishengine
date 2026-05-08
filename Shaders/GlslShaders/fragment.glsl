#version 460 core

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16

struct DirectionalLight {
    vec3 Direction;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct PointLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

/*struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float CutOff;
    float SpotExponent;
};*/

uniform int NumDirLights;
uniform int NumPointLights;

uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
//uniform SpotLight spotLight;

in vec2 TexCoord;
in vec3 FragViewPosition;
in vec3 NormalViewPosition;

out vec4 FragColor;

uniform sampler2D TextureDiffuse0;
uniform sampler2D TextureSpecular0;

uniform bool HasTextureDiffuse;
uniform bool HasTextureSpecular;

uniform vec4 DiffuseColor;
uniform vec3 SpecularColor;

vec3 CalculateDirectionalLight(vec4 diffuse, vec3 specular, float shininess, vec3 normal, DirectionalLight light, vec3 viewDirection);
vec3 CalculatePointLight(vec3 diffuse, vec3 specular, float shininess, vec3 normal, PointLight light, vec3 viewDirection);

void main() {
    vec4 diffuseStrength = HasTextureDiffuse ? texture(TextureDiffuse0, TexCoord).rgba : DiffuseColor;
    if (diffuseStrength.a < 0.1) {
        discard;
    }

    vec3 specularStrength = HasTextureSpecular ? texture(TextureSpecular0, TexCoord).rgb : SpecularColor;
    vec3 normal = normalize(NormalViewPosition);
    vec3 viewDirection = normalize(-FragViewPosition);

    vec3 result = vec3(0.0);

    for (int i = 0; i < NumDirLights; i++) {
        result += CalculateDirectionalLight(diffuseStrength, specularStrength, 256.0, normal, dirLights[i], viewDirection);
    }

    for (int i = 0; i < NumPointLights; i++) {
        result += CalculatePointLight(diffuseStrength.rgb, specularStrength, 256.0, normal, pointLights[i], viewDirection);
    }

    FragColor = vec4(result, diffuseStrength.a);
}

vec3 CalculateDirectionalLight(vec4 diffuse, vec3 specular, float shininess, vec3 normal, DirectionalLight light, vec3 viewDirection) {
    // Since the input light direction is the vector from the light source to the objects
    // we need to get its inverse.
    vec3 lightDir = normalize(-light.Direction);

    float diff = max(dot(normal, lightDir), 0.0);

    // Specular with halfway direction by Blinn
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    if (diff == 0.0) {
        spec = 0.0;
    }

    vec3 totalAmbient  = light.Ambient  * diffuse.rgb;
    vec3 totalDiffuse  = light.Diffuse  * diff * diffuse.rgb;
    vec3 totalSpecular = light.Specular * spec * specular;

    return totalAmbient + totalDiffuse + totalSpecular;
}

vec3 CalculatePointLight(vec3 diffuse, vec3 specular, float shininess, vec3 normal, PointLight light, vec3 viewDirection) {
    vec3 lightDir = normalize(light.Position - FragViewPosition);

    float diff = max(dot(lightDir, normal), 0.0);

    // Specular with halfway direction by Blinn
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    // Removes the specular contribution if the diffuse contribution is 0.
    if (diff == 0.0) {
        spec = 0.0;
    }

    // Attenuation simulates real light falloff.
    float distance    = length(light.Position - FragViewPosition);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    vec3 totalAmbient  = light.Ambient  * diffuse;
    vec3 totalDiffuse  = light.Diffuse  * diff * diffuse;
    vec3 totalSpecular = light.Specular * spec * specular;

    return (totalAmbient + totalDiffuse + totalSpecular) * attenuation;
}