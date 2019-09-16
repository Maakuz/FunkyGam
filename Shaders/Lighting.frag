uniform int nrOfLights;

//first vec is pos and radius, second vector is color, third is padding/USELESS
uniform mat3[200] lights;

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
        radius = lights[i][0].z;
        color.rgb = lights[i][1].xyz;
    
        distanceFromLight = length(coord.xy - lights[i][0].xy);

        color.a += 1 - clamp((distanceFromLight / (radius * (1 - noFadePercentage))) - noFadePercentage, 0.0, 1.0);
    }

    float alpha = 1 - texture2D(shadowMap, gl_TexCoord[0].xy).r;
    
    color.a = color.a - alpha;

    clamp(color.a, 0, 1);

    gl_FragColor = gl_Color;
    gl_FragColor = color;
}