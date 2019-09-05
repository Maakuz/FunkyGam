#version 120
uniform sampler2D texture;

void main()
{
    const int KERNEL_SIZE = 9;
    float[KERNEL_SIZE] kernel  = float[KERNEL_SIZE](0.100840, 0.108306, 0.113975, 0.117518, 0.118723, 0.117518, 0.113975, 0.108306, 0.100840  );

    const vec2 SCREEN_SIZE = vec2(1280, 720);

    vec4 final = vec4(0, 0, 0, 0);

    vec2 offset = vec2(KERNEL_SIZE * 0.5, 0);

    for (int x = 0; x < KERNEL_SIZE; ++x)
    {
       vec2 pos = vec2(gl_FragCoord.x - offset.x + x, gl_FragCoord.y) / SCREEN_SIZE;


       final += texture2D(texture, pos) * kernel[x];
    }
    
    
    gl_FragColor = final;
}