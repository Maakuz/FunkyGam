uniform int testSize;
uniform vec2[200] test;

void main()
{
    float[200] results;

    float radius = 200.0;
    float noFadePercentage = 0.5;

    vec4 color = vec4(0, 0, 0, 0);

    vec2 coord = vec2(gl_FragCoord.x, 720.0 - gl_FragCoord.y);

    float distanceFromLight = 0;

    for (int i = 0; i < testSize; ++i)
    {
        distanceFromLight = length(coord.xy - test[i].xy);

        color.a += 1 - clamp((distanceFromLight / (radius * (1 - noFadePercentage))) - noFadePercentage, 0.0, 1.0);
    }

    color.a = 1 - color.a;

    clamp(color.a, 0, 1);

    gl_FragColor = color;
}