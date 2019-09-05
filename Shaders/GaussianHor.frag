#version 120
uniform sampler2D texture;

void main()
{
    const int KERNEL_SIZE = 5;
    float[KERNEL_SIZE] kernel  =float[KERNEL_SIZE](0.178203, 0.210522, 0.222549, 0.210522, 0.178203);

    vec4 final = vec4(0, 0, 0, 0);

    vec2 offset = vec2(0, KERNEL_SIZE / 2);

    for (int y = 0; y < KERNEL_SIZE; ++y)
    {
       final += texture2D(texture, gl_TexCoord[0].xy - offset + vec2(0, y)) * kernel[y];
    }
    
    
    gl_FragColor = final;
}