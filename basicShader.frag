#version 400 core
struct material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

Light light;

uniform vec4 lightPos;
uniform vec3 lightDirection;

uniform bool drawingSelectedCubeOnClick;
uniform bool drawingSelectedFace;

uniform vec3 cubeAmbient;
uniform vec3 cubeDiffuse;
uniform vec3 cubeSpecular;

uniform mat4 mvMatrix;
uniform mat3 normalMatrix;

uniform vec3 cubeColor;
uniform bool newCube;

//in vec2 fTexCoords;

in vec3 fNormal;
in vec3 fPosition;
out vec4 fColor;

void
main()
{
    vec4 t = mvMatrix * lightPos;

    light.position = vec3(lightPos);
    light.direction = normalMatrix * lightDirection;
    light.cutOff = 0.9978f;
    light.outerCutOff = 0.2194f;
    light.constant = 1.0f;
    light.linear = 0.09;
    light.quadratic = 0.032;

    if(newCube)
    {
        light.ambient = cubeColor;
    }

    if (drawingSelectedFace)
    {
        light.ambient = vec3(0.0, 0.0, 1.0);
    }
    else if (drawingSelectedCubeOnClick)
    {
        light.ambient = vec3(1.0, 0.0, 0.0);
    }

    // Add cube own lighting
    if(!drawingSelectedCubeOnClick && !drawingSelectedFace)
        light.ambient = cubeAmbient;

    light.diffuse = cubeDiffuse;
    light.specular = cubeSpecular;

    // Ambient
    vec3 ambient = light.ambient /* vec3(texture(material.diffuse, fTexCoords))*/;

    // Diffuse
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(light.position - fPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff /* vec3(texture(material.diffuse, fTexCoords))*/;

    // Specular
    vec3 nviewDirection = normalize(vec3(0.0) - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(nviewDirection, reflectDir), 0.0), 5/*material.shininess*/);
    vec3 specular = light.specular * spec /* vec3(texture(material.specular, fTexCoords))*/;

    // Spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // Attenuation
    float distance = length(light.position - fPosition);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    fColor = vec4(ambient + diffuse + specular, 1.0f);
}
