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

uniform sampler2D texShadowMap;
in vec3 fLightPosition;
in vec4 ShadowCoord;

void
main()
{
    /*vec4 t = mvMatrix * lightPos;

    if (drawingSelectedFace)
    {
        light.ambient = vec3(0.0, 1.0, 0.0);
    }
    else if (drawingSelectedCubeOnClick)
    {
        light.ambient = vec3(1.0, 0.0, 0.0);
    }*/

    // Build the matrix to transform from XYZ (normal map) space to TBN (tangent) space
    // Each vector fills a column of the matrix
    mat3 tbn = mat3(normalize(fTangent), normalize(fBinormal), normalize(fNormal));
    vec3 normalFromTexture = texture(texNormal, fUV).rgb * 2.0 - vec3(1.0);
    vec3 normal = normalize(tbn * normalFromTexture);

    // Diffuse
    vec3 lightDir = normalize(vec3(0.0)-fPosition);
    vec3 nfNormal = fNormal;
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 nviewDirection = normalize(vec3(0.0) - fPosition);
    vec3 reflectDir = normalize(-lightDir+2.0*normal*dot(normal,lightDir)); // reflect(-lightDir, fNormal);
    float specular = pow(max(dot(nviewDirection, reflectDir), 0.0), 32);

    vec4 materialColor = texture(texColor, fUV);
    float visible = 1.0;
    float bias = 0.005f;

    // Project the shadow coordinate by dividing by parameter w.
    // The .xy values are the uv coordinates of the texture, and
    //  the .z value is the detpth.
    //
    // We also need to map uv coordinates to the range [0...1, 0...1]
    //  and depth values (z) to the range [0...1] by multiplying by 0.5
    //  and adding 0.5
    //
    vec3 coord = 0.5*(ShadowCoord.xyz / ShadowCoord.w)+0.5;

    // Check if the surface is visible.
    //  If not, reduce the illumination factor.
    float shadowDepth = texture(texShadowMap, coord.xy).r;
    if( coord.z > shadowDepth + bias )
    {
        visible = 0.0;
    }

    // Diffuse lighting only.
    fColor = visible * materialColor * diffuse;

    //fColor = texture(texColor, fUV) * diffuse * 0.8 + vec4(1.0) * specular * 0.2;

    // Cubes werden wie einzelne Cubes und nicht wie gesamte Fläche behandelt ??

}

