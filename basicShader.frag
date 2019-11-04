#version 400 core
uniform bool drawingSelected;
in vec3 fNormal;
in vec3 fPosition;
out vec4 fColor;
void
main()
{
    // Get lighting vectors
    vec3 LightDirection = normalize(vec3(0.0)-fPosition);	// We assume light is at camera position
    vec3 nfNormal = normalize(fNormal);
    vec3 nviewDirection = normalize(vec3(0.0)-fPosition);

    // Compute diffuse component
    float diffuse = max(0.0, dot(nfNormal, LightDirection));

    // Compute specular component
    vec3 Rl = normalize(-LightDirection+2.0*nfNormal*dot(nfNormal,LightDirection));
    float specular = 0.0;//0.1*pow(max(0.0, dot(Rl, nviewDirection)), 16);

    // Compute final color
    vec3 color = drawingSelected ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);

    fColor = vec4(color * (diffuse +  specular), 1);
}
