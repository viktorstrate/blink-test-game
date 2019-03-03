#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

layout (std140) uniform Camera
{
    mat4 view;
    mat4 projection;
    // Position of the camera
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

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 result = vec3(0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);

    result += CalcDirLight(dirLight, norm, viewDir);

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