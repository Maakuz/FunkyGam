uniform int testSize;
uniform vec2[200] test;

void main()
{
    float[200] results;

    float radius = 300.0;
    vec4 color = vec4(0, 0, 0, 1);

    vec2 coord = vec2(gl_FragCoord.x, 720.0 - gl_FragCoord.y);

    for (int i = 0; i < testSize; ++i)
    {
        color.r += 1 - clamp((length(coord.xy - test[i].xy) / radius), 0.0, 1.0);
    }

    clamp(color.a, 0, 1);


    gl_FragColor = color;
}