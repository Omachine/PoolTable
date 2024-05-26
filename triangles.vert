 
#version 440 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 2 ) in vec2 vTextureCoords;

out vec2 textureCoord;

uniform mat4 MVP;

void main() {
    
    gl_Position = MVP * vec4(vPosition, 1.0f);
    textureCoord = vTextureCoords;
}