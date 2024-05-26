#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")


#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "LoadBall.h"
#include "LoadShaders.h"

//definição de largura e altura
#define WIDTH 800
#define HEIGHT 600

//definição de indices
#define NumIndices 36 //36

//definição de zoom
float zoom = 20.0f;



//criaçao das variaveis VAO VBO e Indices
GLuint VAOs;
GLuint Buffers;
GLuint Indices;

vec3 BallPositions[] = //vec3 position od random 15 balls and are on top of the table
{
	vec3(-8.0f, 1.0f, 0.0f),
	vec3(-7.0f, 1.0f, 1.0f),
	vec3(-7.0f, 1.0f, -1.0f),
	vec3(-6.0f, 1.0f, 2.0f),
	vec3(-6.0f, 1.0f, 0.0f),
	vec3(-6.0f, 1.0f, -2.0f),
	vec3(-5.0f, 1.0f, 3.0f),
	vec3(-5.0f, 1.0f, 1.0f),
	vec3(-5.0f, 1.0f, -1.0f),
	vec3(-5.0f, 1.0f, -3.0f),
	vec3(-4.0f, 1.0f, 4.0f),
	vec3(-4.0f, 1.0f, 2.0f),
	vec3(-4.0f, 1.0f, 0.0f),
	vec3(-4.0f, 1.0f, -2.0f),
	vec3(-4.0f, 1.0f, -4.0f)
};


vec3 rotationAngle(0.0f, 0.0f, 0.0f);
vec2 clickPos;
vec2 prevClickPos;

//matrizes de model e projection
mat4 model(1.0f);
mat4 projection(1.0f);

//função de callback para zoom
void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	//zoom in
	if (yoffset == -1)
	{
		zoom += fabs(zoom) * 0.1f;
	}
	//zoom out
	else if (yoffset == 1)
	{
		zoom -= fabs(zoom) * 0.1f;
	}
}



//check do click do rato
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	//se o botão esquerdo do rato for pressionado
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//obter a posição do rato
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		clickPos = vec2(xpos, ypos);
		prevClickPos = clickPos;
	}
	//se o botão esquerdo do rato for libertado
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		//stop rotation
		rotationAngle.x = 0.0f;
		rotationAngle.y = 0.0f;

	}

	model = rotate(model, radians(rotationAngle.y), vec3(1.0f, 0.0f, 0.0f));
}

//função de callback para o movimento do rato
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	//se o botão esquerdo do rato for pressionado
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		prevClickPos = clickPos;
		clickPos = vec2(xpos, ypos);

		//calcular a diferença entre a posição atual e a anterior
		vec2 diff = clickPos - prevClickPos;

		//sensibilidade
		const float sensitivity = 0.01f;

		//calcular a rotação
		rotationAngle.y += diff.x * sensitivity;	
	}
}

//criação da mesa

void drawTable(GLuint tableProgram, mat4 proj, mat4 view)
{

	//Vincular o VAO
	glBindVertexArray(VAOs);

	//usar progama shader escolhido
	glUseProgram(tableProgram);

	//aplica rotação do modelo
	model = rotate(model, radians(rotationAngle.y), vec3(0.0f, 1.0f, 0.0f));

	//defenir as matrizes de model , view e projection
	
	GLuint mvp = glGetUniformLocation(tableProgram, "MVP");

	glUniformMatrix4fv(mvp, 1, GL_FALSE, value_ptr(proj * view * model));

	
	//renderiza os elementos atuais do VAO
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);


	
}

void initTable()
{
	GLfloat vertices[] = {
		// Pos             //cor
		-9.0f,  0.5f,  5.5f ,  0.5f, 0.8f, 0.6f  ,
		 9.0f,  0.5f,  5.5f ,  0.2f, 0.7f, 0.3f  ,
		-9.0f, -0.5f,  5.5f ,  0.3f, 0.7f, 0.4f  ,
		 9.0f, -0.5f,  5.5f ,  0.4f, 0.8f, 0.5f  ,
		-9.0f,  0.5f, -5.5f ,  0.4f, 0.8f, 0.5f  ,
		 9.0f,  0.5f, -5.5f ,  0.4f, 0.8f, 0.5f  ,
		-9.0f, -0.5f, -5.5f ,  0.4f, 0.8f, 0.5f  ,
		 9.0f, -0.5f, -5.5f ,  0.2f, 0.5f, 0.2f  
	};

	GLuint indices[] = {
		// Frente
	  0, 1, 2, 1, 3, 2,
	  // Direita
	  1, 3, 7, 1, 5, 7,
	  // Baixo
	  2, 3, 6, 3, 6, 7,
	  // Esquerda
	  0, 2, 4, 2, 6, 4,
	  // Trás
	  4, 5, 6, 5, 7, 6,
	  // Cima
	  0, 4, 1, 4, 5, 1
	};

	//criar 1 VAO
	glGenVertexArrays(1, &VAOs);

	//gera 1 buffer de VBO
	glGenBuffers(1, &Buffers);

	//gera 1 buffer de EBO
	glGenBuffers(1, &Indices);

	//vincular o VAO
	glBindVertexArray(VAOs);

	//vincular o buffer de VBO
	glBindBuffer(GL_ARRAY_BUFFER, Buffers);

	//carregar os dados do VBO para a memoria da GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //tipo, tamanho, dados, uso

	//vincular o buffer de EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//configura o VAO na ordem do VBO
	//atributo de vertice 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // O 3 especifica numero de atributos (x, y z)
	
	//atributo de vertice 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // O 3 especifica numero de atributos (r, g, b)

	//habilitar os atributos
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	
}


int main(void) 
{
	glfwInit();

	GLFWwindow* window;



	
	window = glfwCreateWindow(WIDTH, HEIGHT, "Bilhar", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	

	// Inicia o gestor de extensões GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	//funçõescallback
	glfwSetScrollCallback(window, scrollCallBack);
	glfwSetMouseButtonCallback(window, mouseClickCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	
	//cor background
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);


	//tem um backbuffer com uma cor e outra com os modelos logo há uma troca de buffers
	glfwSwapBuffers(window);

	//dizer ao opengl para fazer a renderização
	glViewport(0, 0, WIDTH, HEIGHT);

	// Array de informações dos shaders para o programa das bolas
	ShaderInfo ballshaders[] = {
		{ GL_VERTEX_SHADER,   "triangles.vert" },     // Shader de vértice da bola
		{ GL_FRAGMENT_SHADER, "triangles.frag" },     // Shader de fragmento da bola
		{ GL_NONE, NULL }                            // Marcação de fim do array
	};

	// Carrega os shaders e cria o programa das bolas
	GLuint ballProgram = LoadShaders(ballshaders);
	if (!ballProgram) return -1;

	//usa o programa das bolas
	glUseProgram(ballProgram);

	// Array de informações dos shaders para o programa da mesa
	ShaderInfo mesashaders[] = {
		{ GL_VERTEX_SHADER,   "MesaBilhar.vert" },     // Shader de vértice da mesa
		{ GL_FRAGMENT_SHADER, "MesaBilhar.frag" },     // Shader de fragmento da mesa
		{ GL_NONE, NULL }                            // Marcação de fim do array
	};

	// Carrega os shaders e cria o programa da mesa
	GLuint tableProgram = LoadShaders(mesashaders);
	if (!tableProgram) return -1;


	//Função de carregar a mesa
	initTable();

	//habilitar o teste de profundidade
	glEnable(GL_DEPTH_TEST);


	//cria e carrega as bolas
	LoadBall::Ball ball1;
	ball1.BindShader(ballProgram, 1);
	ball1.Load("Objects/Ball1.obj");
	ball1.Install();

	
	LoadBall::Ball ball2;
	ball2.BindShader(ballProgram, 2);
	ball2.Load("Objects/Ball2.obj");
	ball2.Install();
	
	
	LoadBall::Ball ball3;
	ball3.BindShader(ballProgram, 3);
	ball3.Load("Objects/Ball3.obj");
	ball3.Install();

	LoadBall::Ball ball4;
	ball4.BindShader(ballProgram, 4);
	ball4.Load("Objects/Ball4.obj");
	ball4.Install();

	LoadBall::Ball ball5;
	ball5.BindShader(ballProgram, 5);
	ball5.Load("Objects/Ball5.obj");
	ball5.Install();

	LoadBall::Ball ball6;
	ball6.BindShader(ballProgram, 6);
	ball6.Load("Objects/Ball6.obj");
	ball6.Install();

	LoadBall::Ball ball7;
	ball7.BindShader(ballProgram, 7);
	ball7.Load("Objects/Ball7.obj");
	ball7.Install();

	LoadBall::Ball ball8;
	ball8.BindShader(ballProgram, 8);
	ball8.Load("Objects/Ball8.obj");
	ball8.Install();

	LoadBall::Ball ball9;
	ball9.BindShader(ballProgram, 9);
	ball9.Load("Objects/Ball9.obj");
	ball9.Install();

	LoadBall::Ball ball10;
	ball10.BindShader(ballProgram, 10);
	ball10.Load("Objects/Ball10.obj");
	ball10.Install();

	LoadBall::Ball ball11;
	ball11.BindShader(ballProgram, 11);
	ball11.Load("Objects/Ball11.obj");
	ball11.Install();

	LoadBall::Ball ball12;
	ball12.BindShader(ballProgram, 12);
	ball12.Load("Objects/Ball12.obj");
	ball12.Install();

	LoadBall::Ball ball13;
	ball13.BindShader(ballProgram, 13);
	ball13.Load("Objects/Ball13.obj");
	ball13.Install();

	LoadBall::Ball ball14;
	ball14.BindShader(ballProgram, 14);
	ball14.Load("Objects/Ball14.obj");
	ball14.Install();

	LoadBall::Ball ball15;
	ball15.BindShader(ballProgram, 15);
	ball15.Load("Objects/Ball15.obj");
	ball15.Install();


	//matriz de projeção
	mat4 proj = mat4(1.0f);

	//matriz de view
	mat4 view = mat4(1.0f);

	// posição camera
	vec3 position(vec3(0.0f, 10.0f, 0.0f));

	//target da camera
	vec3 target(vec3(0.0f));

	//calculos da camera
	vec3 camFront = position-target;
	vec3 camRight = cross(vec3(0.0f, 1.0f, 0.0f), camFront);
	vec3 camUp(0.0f, 1.0f, 0.0f);

	//definição de matrizes
	proj = perspective(radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 150.0f);


	while (!glfwWindowShouldClose(window)) {

		//limpa o buffer de cor e de profundidade
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// posição camera
		position = (vec3(0.0f, 10.0f, zoom));
		view = lookAt(position, target, camUp);
		//matriz de zoom
		//mat4 matZoom = scale(mat4(1.0f), vec3(zoom));

		//bind do VAO ao OpenGl
		glBindVertexArray(VAOs);

		//usa o programa das bolas
		glUseProgram(ballProgram);

		//desenhar as bolas
		ball1.Render(BallPositions[0], rotationAngle);
		ball2.Render(BallPositions[1], rotationAngle);
		ball3.Render(BallPositions[2], rotationAngle);
		ball4.Render(BallPositions[3], rotationAngle);
		ball5.Render(BallPositions[4], rotationAngle);
		ball6.Render(BallPositions[5], rotationAngle);
		ball7.Render(BallPositions[6], rotationAngle);
		ball8.Render(BallPositions[7], rotationAngle);
		ball9.Render(BallPositions[8], rotationAngle);
		ball10.Render(BallPositions[9], rotationAngle);
		ball11.Render(BallPositions[10], rotationAngle);
		ball12.Render(BallPositions[11], rotationAngle);
		ball13.Render(BallPositions[12], rotationAngle);
		ball14.Render(BallPositions[13], rotationAngle);
		ball15.Render(BallPositions[14], rotationAngle);


		
		//desenha mesa
		drawTable(tableProgram, proj, view);
	

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Limpa os VBOs, o VAO e indices
	glDeleteVertexArrays(1, &VAOs);
	glDeleteBuffers(1, &Buffers);
	glDeleteBuffers(1, &Indices);

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
