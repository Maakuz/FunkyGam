#version 120
uniform sampler2D texture;

void main()
{
    const int KERNEL_SIZE = 31;
    float[KERNEL_SIZE] kernel  = float[KERNEL_SIZE](
    0.028679, 0.029352, 0.029993, 0.030599, 0.031167, 0.031695, 0.032180, 0.032621, 0.033015, 0.033360, 0.033655, 0.033898, 0.034088, 0.034225, 0.034307, 0.034335, 0.034307, 0.034225, 0.034088, 0.033898, 0.033655, 0.033360, 0.033015, 0.032621, 0.032180, 0.031695, 0.031167, 0.030599, 0.029993, 0.029352, 0.028679  
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