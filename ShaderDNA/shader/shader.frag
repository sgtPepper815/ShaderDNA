#version 330 core

out vec4 FragColor;

uniform vec2 uResolution;
uniform float uTime;
uniform float uFrequency;
uniform float uDistortion;
uniform float uSpeed;
uniform vec3 uColorA;
uniform vec3 uColorB;

void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0
             - uResolution) / uResolution.y;

    float r = length(uv);
    float a = atan(uv.y, uv.x);

    float pattern = sin(
        r * uFrequency
        + sin(a * 6.0 + uTime * uSpeed)
        * uDistortion
    );

    vec3 color = mix(
        uColorA,
        uColorB,
        pattern * 0.5 + 0.5
    );

    FragColor = vec4(color, 1.0);
}