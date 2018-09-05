#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
//in vec2 TexCoords;

uniform vec3 cameraPos;
//uniform sampler2D texture_diffuse1;
uniform samplerCube skybox;

void main()
{    
//    FragColor = texture(texture_diffuse1, TexCoords);
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
