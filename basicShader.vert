#version 400 core
uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvpLightMatrix;
uniform vec4 spotLightPosition;

in vec4 vPosition;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;

out vec3 fNormal;
out vec3 fPosition;
out vec2 fUV;
out vec3 fTangent;
out vec3 fBinormal;
out vec3 fLightPosition;
out vec4 fShadowCoord;

void
main()
{
    vec4 vEyeCoord = mvMatrix * vPosition;

    gl_Position = projMatrix * vEyeCoord;
    fPosition = vEyeCoord.xyz;
    fNormal = normalMatrix*vNormal;
    fUV = vUV;
    fTangent.xyz = mat3(mvMatrix)*vTangent;
    fBinormal = mat3(mvMatrix)*cross(vNormal, vTangent);
    fLightPosition = spotLightPosition.xyz;
    fShadowCoord = mvpLightMatrix * vPosition;
}
