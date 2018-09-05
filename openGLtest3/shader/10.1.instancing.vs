#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset; //using instance array

out vec3 fColor;
//uniform vec2 offsets[100]; // just display using instance,not using array
void main()
{
    fColor = aColor;

    //using instance array
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    
    // just display using instance,not using array
    /*
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(aPos + offset, 0.0, 1.0);
     */
}
