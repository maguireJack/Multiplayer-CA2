//Taken from: https://learnopengl.com/In-Practice/2D-Game/Postprocessing

uniform sampler2D source;
uniform vec2      offsets[9];
uniform float     blur_kernel[9];

void main()
{
    vec4 color = vec4(0.0);
    vec3 sample[9];
    // sample from texture offsets if using convolution matrix
    for(int i = 0; i < 9; i++)
        sample[i] = vec3(texture(source, gl_TexCoord[0].xy + offsets[i]));
    
    for(int i = 0; i < 9; i++)
        color += vec4(sample[i] * blur_kernel[i], 0.0f);
    color.a = 1.0f;
	gl_FragColor = color;
}