#version 400 core
in vec4 fPosition;
out vec4 fColor;

void main(void)

{
    vec3 fragCoord = (fPosition.xyz / fPosition.w);

    // Compute the depth in the range [0...1] by
    //  multiplying by 0.5 and adding 0.5.
    //
    fColor = vec4(vec3(0.5 * fragCoord.z + 0.5), 1);
}
