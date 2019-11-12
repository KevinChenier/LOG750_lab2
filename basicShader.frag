#version 400 core
uniform bool drawingSelectedCubeOnClick;
uniform bool drawingSelectedFace;
uniform vec3 cubeAmbiant;
uniform float cubeDiffuse;
uniform float cubeSpecular;

uniform vec3 cubeColor;
uniform bool newCube;

in vec3 fNormal;
in vec3 fPosition;
out vec4 fColor;
void
main()
{
    // Get lighting vectors
    vec3 pointLightDirection = normalize(vec3(0.0)-fPosition);	// We assume light is at camera position
    vec3 nfNormal = normalize(fNormal);
    vec3 nviewDirection = normalize(vec3(0.0)-fPosition);

    // Get directional light vectors
    vec3 directionalLightDirection = normalize(vec3(-1, -1, -1) - fPosition); // Directional light is at (-1,-1,-1)
    float directionalLightIntensity = max(0.3, dot(nfNormal, normalize(-directionalLightDirection.xyz)));

    // Compute diffuse component
    float diffuse = max(0.0, dot(nfNormal, pointLightDirection));

    // Compute specular component
    vec3 Rl = normalize(-pointLightDirection+2.0*nfNormal*dot(nfNormal, pointLightDirection));
    float specular = 0.1*pow(max(0.0, dot(Rl, nviewDirection)), 16);

    // Compute final color
    vec3 color;

    if(newCube){
        color = cubeColor;
    }

    if (drawingSelectedFace)
    {
        color = vec3(0.0 , 0.0, 1.0);
    }
    else if (drawingSelectedCubeOnClick)
    {
        color = vec3(1.0, 0.0, 0.0);
    }
    else if(!newCube)
    {
        color = vec3(0.0, 1.0, 0.0);
    }

    // Add cube "texture"
    if(!drawingSelectedCubeOnClick && !drawingSelectedFace)
        color += cubeAmbiant;

    diffuse += cubeDiffuse;
    specular += cubeSpecular;

    fColor = vec4(color * directionalLightIntensity * (diffuse +  specular), 1);
}
