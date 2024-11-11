#version 460 core

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

uniform DirectionalLight dirLight;
uniform PointLight pointLight;
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

uniform vec3 LightColor;

uniform vec3 LightPosition;

vec3 CalculateDirectionalLight(vec3 diffuse, vec3 specular, float shininess, vec3 normal, DirectionalLight dirLight, vec3 viewDirection);
vec3 CalculatePointLight(vec3 diffuse, vec3 specular, float shininess, vec3 normal, PointLight pointLight, vec3 viewDirection);

void main() {
    vec3 diffuseStrength = HasTextureDiffuse ? texture(TextureDiffuse0, TexCoord).rgb : DiffuseColor.rgb;
    vec3 specularStrength = HasTextureSpecular ? texture(TextureSpecular0, TexCoord).rgb : SpecularColor;

    vec3 viewDirection = normalize(-FragViewPosition);

    vec3 dirLightContribution = CalculateDirectionalLight(diffuseStrength, specularStrength, 256.f, normalize(NormalViewPosition), dirLight, viewDirection);
    vec3 pointLightContribution = CalculatePointLight(diffuseStrength, specularStrength, 256.f, normalize(NormalViewPosition), pointLight, viewDirection);

    FragColor = vec4(pointLightContribution, 1.f);
}

vec3 CalculateDirectionalLight(vec3 diffuse, vec3 specular, float shininess, vec3 normal, DirectionalLight dirLight, vec3 viewDirection) {
    // Since the input light direction is the vector from the light source to the objects
    // we need to get it's inverse.
    vec3 lightDir = normalize(-dirLight.Direction);

    float diff = max(dot(normal, lightDir), 0.f);

    // Specular with halfway direction by Blinn
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.f), shininess);

    if(diff == 0.f) {
        spec = 0.f;
    }

    vec3 totalDiffuse = dirLight.Diffuse * diff * diffuse;
    vec3 totalSpecular = dirLight.Specular * spec * specular;
    vec3 totalAmbient = dirLight.Ambient * diffuse;

    return totalAmbient + totalDiffuse + totalSpecular;
}

vec3 CalculatePointLight(vec3 diffuse, vec3 specular, float shininess, vec3 normal, PointLight pointLight, vec3 viewDirection) {
    vec3 lightDir = normalize(pointLight.Position - FragViewPosition);

    // We use max between the dot product and 0 to make sure the value of diff is not negative (if we got 0, we get a black object with no light)
    float diff = max(dot(lightDir, normal), 0.f);

    // Specular with halfway direction by Blinn
    vec3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.f), shininess);

    // Revomes the specular contribuition if the diffuse contribution is 0.
    if(diff == 0.f) {
        spec = 0.f;
    }

    // Attenuation is used to obtain smoother edges simulating real lights.
    float distance = length(pointLight.Position - FragViewPosition);
    float attenuation = 1.f / (pointLight.Constant + pointLight.Linear * distance + pointLight.Quadratic * (distance * distance));

    vec3 totalAmbient = pointLight.Ambient * diffuse;
    vec3 totalDiffuse = pointLight.Diffuse * diff * diffuse;
    vec3 totalSpecular = pointLight.Specular * spec * specular;

    totalAmbient *= attenuation;
    totalDiffuse *= attenuation;
    totalSpecular *= attenuation;
    return totalAmbient + totalDiffuse + totalSpecular;
}