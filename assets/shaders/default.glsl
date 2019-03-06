#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;

layout (std140) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 ViewPos;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    ViewPos = viewPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 ViewPos;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    float range;

    vec3 position; // 0,1,2
    // PADDING 3

    vec3 ambient; // 4,5,6
    // PADDING 7

    vec3 diffuse; // 8,9,10
    // PADDING 11

    vec3 specular; // 12,13,14

};

#define MAX_DIRECTIONAL_LIGHTS 50
#define MAX_POINT_LIGHTS 20

layout (std140) uniform DirectionalLights
{
    DirLight lights[MAX_DIRECTIONAL_LIGHTS];
    int amount;
} directionalLights;

layout (std140) uniform PointLights
{
    int amount;
    PointLight lights[MAX_POINT_LIGHTS];
} pointLights;


out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

/*
struct SpotLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
    float cutOff;
    float outerCutOff;
};

#define NR_POINT_LIGHTS 4

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;*/

uniform Material material;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 result = vec3(0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);

    for(int i = 0; i < directionalLights.amount; i++){
        //result += CalcDirLight(directionalLights.lights[i], norm, viewDir);
    }

    for(int i = 0; i < pointLights.amount; i++){
        //result += CalcPointLight(pointLights.lights[i], norm, FragPos, viewDir);
        //result += vec3(pointLights.lights[i].range);
        //result = vec3(1.0f, 1.0f, 1.0f);
    }

    /*result += CalcSpotLight(spotLight, norm, FragPos, viewDir);*/

    result = vec3(0.2);
    result += vec3(pointLights.amount);
//    result = pointLights.lights[1].diffuse;
//    result = vec3(pointLights.lights[1].range);
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

/*vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float constant = 1;
    float linear = 4.7 / light.range;
    float quadratic = 82.5 / (light.range * light.range);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (constant + linear * distance +
  			     quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

/*vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

    vec3 lightDir = normalize(light.position - fragPos);

    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    if (intensity == 0.0f) {
        return vec3(0.0f);
    }

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular) * intensity;

}*/