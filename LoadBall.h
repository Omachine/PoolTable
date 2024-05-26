#ifndef LOADBALL_H
#define LOADBALL_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

namespace LoadBall
{
	class Ball
	{
	private: 
		GLuint VAOs;
		GLuint Buffers;
		GLuint ShaderProgram;
		GLuint textureIndices;

	public:
		//variaveis dentro do ficheiro obj
		vector<vec3> vertices;
		vector<vec2> uvs;
		vector<vec3> normals;

		//declaração das funções
		void Install(void);
		void Render(glm::vec3 position, glm::vec3 orientation);
		void Load(const std::string obj_model_filepath);
		void LoadTexture(const char* texture_filepath);
		void ReadMTL(char* mtl_model_filepath);
		void BindShader(GLuint shaderProgram, GLuint textureIndices);
		
	};
}
#endif // !LOADBALL_H