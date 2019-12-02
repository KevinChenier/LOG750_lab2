#version 400 core
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

uniform sampler2D texColor;
uniform sampler2D texNormal;

uniform bool drawingSelectedCubeOnClick;
uniform bool drawingSelectedFace;

uniform vec3 cubeAmbient;
uniform vec3 cubeDiffuse;
uniform vec3 cubeSpecular;

uniform mat4 mvMatrix;
uniform mat3 normalMatrix;

uniform vec3 cubeColor;
uniform bool newCube;

in vec2 fUV;
in vec3 fTangent;
in vec3 fBinormal;
in vec3 fNormal;
in vec3 fPosition;

out vec4 fColor;

void
main()
{
    vec4 t = mvMatrix * lightPos;

    light.position = vec3(t/t.r);
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
        light.ambient = vec3(0.0, 1.0, 0.0);
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

    // Build the matrix to transform from XYZ (normal map) space to TBN (tangent) space
    // Each vector fills a column of the matrix
    mat3 tbn = mat3(normalize(fTangent), normalize(fBinormal), normalize(fNormal));
    vec3 normalFromTexture = texture(texNormal, fUV).rgb * 2.0 - vec3(1.0);
    vec3 norm = normalize(tbn * normalFromTexture);

    // Ambient
    vec4 ambient = vec4(light.ambient, 1.0) * texture(texColor, fUV);

    // Diffuse
    vec3 lightDir = normalize(light.position - fPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // Specular
    vec3 nviewDirection = normalize(vec3(0.0) - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(nviewDirection, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec;

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

    fColor = ambient + vec4(diffuse + specular, 1.0f);
}
