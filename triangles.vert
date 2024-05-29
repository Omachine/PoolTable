#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

layout( location = 0 ) in vec3 vPosition;
layout( location = 2 ) in vec2 vTextureCoords;
layout( location = 1 ) in vec3 vNormal;

out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;
out vec2 textureCoord;

void main()
{ 
	mat4 ModelView = View * Model;
	// Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0f)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);

	// Posição final do vértice (em coordenadas de clip)
	gl_Position = Projection * ModelView * vec4(vPosition, 1.0f);
	
	// Coordenada de textura para o CubeMap
	textureCoord = vTextureCoords;
}
