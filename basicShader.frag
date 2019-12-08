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

Light spotLight;

uniform vec4 spotLightPosition;
uniform vec3 spotLightDirection;

uniform sampler2D texColor;
uniform sampler2D texNormal;
uniform sampler2D texShadowMap;

uniform bool drawingSelectedCubeOnClick;
uniform bool drawingSelectedFace;


uniform vec3 cubeAmbient;
uniform vec3 cubeDiffuse;
uniform vec3 cubeSpecular;

uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;

uniform vec3 cubeColor;
uniform bool newCube;
uniform int Ns;
uniform bool isTool;

in vec2 fUV;
in vec3 fTangent;
in vec3 fBinormal;
in vec3 fNormal;
in vec3 fPosition;
in vec4 fShadowCoord;

out vec4 fColor;

uniform sampler2D texShadowMap;
in vec3 fLightPosition;
in vec4 ShadowCoord;

void
main()
{

    vec4 t = viewMatrix * spotLightPosition;

    spotLight.position = vec3(t/t.w);
    spotLight.direction = normalMatrix * spotLightDirection;
    spotLight.cutOff = 0.9978f;
    spotLight.outerCutOff = 0.2194f;
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09;
    spotLight.quadratic = 0.032;

    if(newCube)
    {
        spotLight.ambient = cubeColor;
    }


    if (drawingSelectedFace)
    {
        spotLight.ambient = vec3(0.0, 1.0, 0.0);
    }
    else if (drawingSelectedCubeOnClick)
    {

        spotLight.ambient = vec3(1.0, 0.0, 0.0);
    }

    // Add cube own lighting
    if(!drawingSelectedCubeOnClick && !drawingSelectedFace)
        spotLight.ambient = cubeAmbient;

    spotLight.diffuse = cubeDiffuse;
    spotLight.specular = cubeSpecular;


    // Normal mapping
    mat3 tbn = mat3(normalize(fTangent), normalize(fBinormal), normalize(fNormal));
    vec3 normalFromTexture = texture(texNormal, fUV).rgb * 2.0 - vec3(1.0);

    vec3 normalRecalculated = normalize(tbn * normalFromTexture);

    // Ambient
    vec4 ambient = vec4(spotLight.ambient, 1.0) * texture(texColor, fUV);

    // Diffuse
    vec3 lightDirectionOnPixel = normalize(spotLight.position - fPosition);
    float diff = max(dot(normalRecalculated, lightDirectionOnPixel), 0.0);
    vec3 diffuse = spotLight.diffuse * diff;

    // Specular
    vec3 nviewDirection = normalize(vec3(0.0) - fPosition);
    vec3 reflectDir = reflect(-lightDirectionOnPixel, normalRecalculated);
    float spec = pow(max(dot(nviewDirection, reflectDir), 0.0), 32);
    vec3 specular = spotLight.specular * spec;

    // Spotlight (soft edges)
    float theta = dot(lightDirectionOnPixel, normalize(-spotLight.direction));
    float epsilon = (spotLight.cutOff - spotLight.outerCutOff);
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // Spotlight attenuation
    float distance = length(spotLight.position - fPosition);
    float attenuation = 1.0f / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));
     ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Shadow mapping
    float bias = 0.005f;
    vec3 coord = 0.5*(fShadowCoord.xyz / fShadowCoord.w)+0.5;
    float shadowDepth = texture(texShadowMap, coord.xy).r;
    float visible = coord.z > (shadowDepth + bias) ? 0.0 : 1.0;
    if (!isTool){
         fColor = visible * (ambient + vec4(diffuse + specular, 1.0f));

    }else // tool color
    {
        fColor = vec4( cubeAmbient + (cubeDiffuse + cubeSpecular * Ns), 1);
    }
    }

