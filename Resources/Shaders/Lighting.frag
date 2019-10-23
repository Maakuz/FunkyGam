uniform vec2 pos;
uniform float radius;
uniform vec3 color;

uniform sampler2D shadowMap;

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

    //alpha = 1 - clamp((distanceFromLight / (radius * (1 - noFadePercentage))) - noFadePercentage, 0.0, 1.0);

    float shadowAlpha = texture2DLod(shadowMap, gl_TexCoord[0].xy, 0).r;
    
    alpha = alpha - (clamp((1 - shadowAlpha) * 0.5, 0, 1));

    clamp(alpha, 0, 1);

    //gl_FragColor = gl_Color;
    gl_FragColor = vec4(color.x, color.y, color.z, alpha);
}