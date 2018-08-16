#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_box;
uniform sampler2D texture_face;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	//FragColor = mix(texture(texture_box, TexCoord), texture(texture_face, TexCoord), 0.2);
    FragColor = mix(texture(texture_box, TexCoord), texture(texture_face, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.7);
//    FragColor = texture(texture_box, TexCoord) * vec4(ourColor, 1.0);
}
