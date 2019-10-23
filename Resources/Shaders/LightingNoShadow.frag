uniform vec2 pos;
uniform float radius;
uniform vec3 color;

void main()
{
    //Something is off with this one
    float noFadePercentage = 0.1;

    vec2 coord = vec2(gl_FragCoord.x, 1080.0 - gl_FragCoord.y);

    float distanceFromLight = 0;
    float percentageFromLight = 0;
    float alpha = 0;
    
    distanceFromLight = length(coord.xy - pos.xy);
    percentageFromLight = distanceFromLight / radius;

    alpha = 1 - clamp(percentageFromLight, 0.0, 1.0);

    //gl_FragColor = gl_Color;
    gl_FragColor = vec4(color.x, color.y, color.z, alpha);
}