//Taken from: https://learnopengl.com/In-Practice/2D-Game/Postprocessing
uniform float time;
uniform float intensity;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
   
    float strength = 0.01;
    gl_Position.x += cos(time * 10) * intensity * strength; 
    gl_Position.y += cos(time * 15) * intensity * strength;
	
}  