#version 120
uniform sampler2D texture;

void main()
{
    const int KERNEL_SIZE = 15;
    float[KERNEL_SIZE] kernel  = float[KERNEL_SIZE](
0.058677, 0.061915, 0.064794, 0.067249, 0.069223, 0.070668, 0.071550, 0.071846, 0.071550, 0.070668, 0.069223, 0.067249, 0.064794, 0.061915, 0.058677     
);

    const vec2 SCREEN_SIZE = vec2(1920, 1080);

    vec4 final = vec4(0, 0, 0, 0);

    vec2 offset = vec2(KERNEL_SIZE * 0.5, 0);

    for (int x = 0; x < KERNEL_SIZE; ++x)
    {
       vec2 pos = vec2(gl_FragCoord.x - offset.x + x, gl_FragCoord.y) / SCREEN_SIZE;


       final += texture2D(texture, pos) * kernel[x];
    }
    
    
    gl_FragColor = final;
}