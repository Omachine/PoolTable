#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "stb_image.h"
#include "LoadShaders.h"
#include "LoadBall.h"

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

namespace LoadBall
{	
	//chama o shader das bolas e indices da textura
	void Ball::BindShader(GLuint shaderProgram, GLuint textureIndices)
	{
		ShaderProgram = shaderProgram;
		textureIndices = textureIndices;
	}
	

	//função de carregar o ficheiro obj
	void Ball::Load(const std::string obj_model_filepath)
	{
		std::cout << "Loading ball from file: " << obj_model_filepath << std::endl;

		FILE* file;
		errno_t err;
		//abrir o ficheiro obj
		err = fopen_s(&file, obj_model_filepath.c_str(), "r");
		if (file == NULL)
		{
			throw("Impossible to open the file !\n");
			return;
		}

		//variaveis para guardar os valores do ficheiro obj
		vector<vec3> temp_vertices;
		vector<vec2> temp_uvs;
		vector<vec3> temp_normals;
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;



		//ler o ficheiro obj
		while (1)
		{
			char lineHeader[128];
			//ler a primeira palavra da linha
			int res = fscanf_s(file, "%s", lineHeader, (unsigned int)_countof(lineHeader));
			if (res == EOF)
			{
				break;
			}
			if (strcmp(lineHeader, "mtllib") == 0) {
				char materialsFilename[128];

				fscanf_s(file, "%s\n", materialsFilename, (unsigned int)_countof(materialsFilename));
				ReadMTL(materialsFilename);
			}
			//se a linha começa com v, então é um vertice
			if (strcmp(lineHeader, "v") == 0)
			{
				vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			//se a linha começa com vt, então é um uv
			else if (strcmp(lineHeader, "vt") == 0)
			{
				vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			//se a linha começa com vn, então é um normal
			else if (strcmp(lineHeader, "vn") == 0)
			{
				vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			//se a linha começa com f, então é um face
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					throw("Failed to read face information\n");
					return;
				}

				for (int i = 0; i < 3; i++)
				{
					vertices.push_back(temp_vertices.at(vertexIndex[i] - 1));
					uvs.push_back(temp_uvs.at(uvIndex[i] - 1));
					normals.push_back(temp_normals.at(normalIndex[i] - 1));
				}
			};

		}


		//fechar o ficheiro obj
		fclose(file);

	
		std::cout << "Loaded ball from file: " << obj_model_filepath << std::endl;


	}

	//função de envia os dados para a placa gráfica
	void Ball::Install(void)
	{
		// Generate and bind the VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		
		// Generate and bind the VBO for vertices
		glGenBuffers(1, &BufferV);
		glBindBuffer(GL_ARRAY_BUFFER, BufferV);
		glBufferStorage(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), 0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(0);

		// Generate and bind the VBO for vertices
		glGenBuffers(1, &BufferU);
		glBindBuffer(GL_ARRAY_BUFFER, BufferU);
		glBufferStorage(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), uvs.data(), 0);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(2);


		// Generate and bind the VBO for vertices
		glGenBuffers(1, &BufferN);
		glBindBuffer(GL_ARRAY_BUFFER, BufferN);
		glBufferStorage(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), 0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		// Apply a scale to reduce the size
		float scale = 0.004f;
		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i] *= scale;
		}
	}


	//função de renderizar o ficheiro obj 
	void Ball::Render(glm::vec3 position, glm::vec3 orientation)
	{
		// Vincula o Vertex Array Object (VAO). O VAO contém as configurações dos buffers de dados e atributos do vértice.
		glBindVertexArray(VAO);

		// Vincula o programa de shader. Este programa é usado para renderizar o modelo.
		glUseProgram(ShaderProgram);

		// Cria a matriz de modelo. Esta matriz é usada para transformar os vértices do modelo.
		mat4 model = mat4(1.0f); // Inicia com a matriz identidade
		model = rotate(model, orientation.x, vec3(1.0f, 0.0f, 0.0f)); // Aplica uma rotação no eixo x
		model = rotate(model, orientation.y, vec3(0.0f, 1.0f, 0.0f)); // Aplica uma rotação no eixo y
		model = rotate(model, orientation.z, vec3(0.0f, 0.0f, 1.0f)); // Aplica uma rotação no eixo z
		model = translate(model, position); // Aplica uma translação para a posição especificada

		// Cria a matriz de projeção. Esta matriz é usada para transformar as coordenadas 3D do modelo para coordenadas 2D na tela.
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		// Cria a matriz de visualização. Esta matriz é usada para transformar as coordenadas do modelo para o espaço da câmera.
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 10.0f, zoom), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		mat4 ModelView = view * model;

		mat3 NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));

		// Passa as matrizes para o shader. O shader usa essas matrizes para transformar os vértices do modelo.
		GLuint m = glGetUniformLocation(ShaderProgram, "Model");
		GLuint v = glGetUniformLocation(ShaderProgram, "View");
		GLuint p = glGetUniformLocation(ShaderProgram, "Projection");
		GLuint mv = glGetUniformLocation(ShaderProgram, "ModelView");
		GLuint n = glGetUniformLocation(ShaderProgram, "NormalMatrix");

		glUniformMatrix4fv(m, 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(v, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(p, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(mv, 1, GL_FALSE, value_ptr(ModelView));
		glUniformMatrix3fv(n, 1, GL_FALSE, value_ptr(projection));

		// Vincula a textura
		glBindTexture(GL_TEXTURE_2D, textureIndices);

		// Desenha o objeto. Esta chamada renderiza os triângulos do modelo usando os dados do VAO.
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());


	}


	void Ball::ReadMTL(char* mtl_model_filepath)
	{
		
		std::string folder = "Objects/";
		std::string fullPath = folder + mtl_model_filepath;

		char lineHeader[128];

		FILE* mtlFile;
		errno_t err;
		err = fopen_s(&mtlFile, fullPath.c_str(), "r");
		if (mtlFile == NULL) {
			printf("Impossible to open the file !\n");
			return;
		}

		//ler o ficheiro mtl
		while (lineHeader[0] != EOF)
		{
			
			//ler a primeira palavra da linha
			int res = fscanf_s(mtlFile, "%s", lineHeader, sizeof(lineHeader));
			if (res == EOF)
				break;
			
			//se a linha começa com Ka, então é um ambient
			if (strcmp(lineHeader, "Ka") == 0)
			{
				vec3 ambient;
				fscanf_s(mtlFile, "%f %f %f\n", &ambient.x, &ambient.y, &ambient.z);
			}
			//se a linha começa com Kd, então é um diffuse
			else if (strcmp(lineHeader, "Kd") == 0)
			{
				vec3 diffuse;
				fscanf_s(mtlFile, "%f %f %f\n", &diffuse.x, &diffuse.y, &diffuse.z);
			}
			//se a linha começa com Ks, então é um specular
			else if (strcmp(lineHeader, "Ks") == 0)
			{
				vec3 specular;
				fscanf_s(mtlFile, "%f %f %f\n", &specular.x, &specular.y, &specular.z);
			}
			//se a linha começa com Ns, então é um shininess
			else if (strcmp(lineHeader, "Ns") == 0)
			{
				float shininess;
				fscanf_s(mtlFile, "%f\n", &shininess);
			}
			else if (strcmp(lineHeader, "map_Kd") == 0) {
				char textureFilename[128];
				fscanf_s(mtlFile, "%s", textureFilename, sizeof(textureFilename));
				LoadTexture(textureFilename);
			}
		}
	}

	void Ball::LoadTexture(const char* texture_filepath) {
		// Generate a texture name
		glGenTextures(1, &textureIndices);

		// Bind this texture name to the GL_TEXTURE_2D target of the active Texture Unit.
		glBindTexture(GL_TEXTURE_2D, textureIndices);

		// Set the filtering parameters (wrapping and size adjustment)
		// for the texture that is bound to the GL_TEXTURE_2D target of the active Texture Unit.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Reading/decompression of the file with texture image
		int width, height, nChannels;
		// Activate the vertical inversion of the image, when reading it into CPU memory.
		stbi_set_flip_vertically_on_load(true);

		// Prepend the folder name to the texture filename
		std::string folder = "Textures/";
		std::string fullPath = folder + texture_filepath;

		// Read the image into CPU memory
		unsigned char* imageData = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);
		if (imageData) {
			// Load the image data into the Texture Object bound to the GL_TEXTURE_2D target of the active Texture Unit.
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

			// Generate the Mipmap for this texture
			glGenerateMipmap(GL_TEXTURE_2D);

			// Free the image from CPU memory
			stbi_image_free(imageData);
		}
		else {
			std::cout << "Error loading texture!" << std::endl;
		}
	}


}
