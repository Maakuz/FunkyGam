uniform int nrOfLights;
uniform vec3[200] lights;
uniform sampler2D shadowMap;

void main()
{
    float noFadePercentage = 0.5;

    vec4 color = vec4(0, 0, 0, 0);

    vec2 coord = vec2(gl_FragCoord.x, 1080.0 - gl_FragCoord.y);

    float distanceFromLight = 0;
    float radius = 0;

    for (int i = 0; i < nrOfLights; ++i)
    {
        radius = lights[i].z;
    
        distanceFromLight = length(coord.xy - lights[i].xy);

        color.a += 1 - clamp((distanceFromLight / (radius * (1 - noFadePercentage))) - noFadePercentage, 0.0, 1.0);
    }

    color.a = 1 - color.a;

    clamp(color.a, 0, 1);

    //gl_FragColor = texture2D(shadowMap, gl_TexCoord[0].xy); så gör man
    gl_FragColor = color;
}