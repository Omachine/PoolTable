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

//defini��o de largura e altura
#define WIDTH 800
#define HEIGHT 600

//defini��o de indices
#define NumIndices 36 //36

//defini��o de zoom
float zoom = 20.0f;



//cria�ao das variaveis VAO VBO e Indices
GLuint VAOs;
GLuint Buffers;
GLuint Indices;

vec3 BallPositions[] = //vec3 position od random 15 balls and are on top of the table
{
	vec3(3.0f, 1.4f, 0.0f),
	vec3(4.0f, 1.4f, 1.0f),
	vec3(4.0f, 1.4f, -1.0f),
	vec3(5.0f, 1.4f, 2.0f),
	vec3(5.0f, 1.4f, 0.0f),
	vec3(5.0f, 1.4f, -2.0f),
	vec3(6.0f, 1.4f, 3.0f),
	vec3(6.0f, 1.4f, 1.0f),
	vec3(6.0f, 1.4f, -1.0f),
	vec3(6.0f, 1.4f, -3.0f),
	vec3(7.0f, 1.4f, 4.0f),
	vec3(7.0f, 1.4f, 2.0f),
	vec3(7.0f, 1.4f, 0.0f),
	vec3(7.0f, 1.4f, -2.0f),
	vec3(7.0f, 1.4f, -4.0f)
};
vec2 clickPos;
vec2 prevClickPos;

//matrizes de model e projection
float rotation = 0.0f;
mat4 projection(1.0f);

//fun��o de callback para o scroll do rato para zoom
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
	cout << "Zoom: " << zoom << endl;
}

bool ambientLightOn = true;
bool directionalLightOn = true;
bool pointLightOn = true;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1:
			// Toggle ambient light
			ambientLightOn = !ambientLightOn;
			break;
		case GLFW_KEY_2:
			// Toggle directional light
			directionalLightOn = !directionalLightOn;
			break;
		case GLFW_KEY_3:
			// Toggle point light
			pointLightOn = !pointLightOn;
			break;
		}
	}
}


//check do click do rato
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	//se o bot�o esquerdo do rato for pressionado
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//obter a posi��o do rato
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		clickPos = vec2(xpos, ypos);
		prevClickPos = clickPos;
	}
}

//fun��o de callback para o movimento do rato
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	//se o bot�o esquerdo do rato for pressionado
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		prevClickPos = clickPos;
		clickPos = vec2(xpos, ypos);

		//calcular a diferen�a entre a posi��o atual e a anterior
		vec2 diff = clickPos - prevClickPos;

		//sensibilidade
		const float sensitivity = 0.01f;

		//calcular a rota��o
		rotation += diff.x * sensitivity;
	}
}

//cria��o da mesa

void drawTable(GLuint tableProgram)
{

	//Vincular o VAO
	glBindVertexArray(VAOs);

	//usar progama shader escolhido
	glUseProgram(tableProgram);

	//aplica rota��o do modelo
	mat4 model = mat4(1.0f); // Inicia com a matriz identidade
	model = rotate(model, rotation, vec3(0.0f, 1.0f, 0.0f)); // Aplica uma rota��o no eixo y

	// Cria a matriz de proje��o. Esta matriz � usada para transformar as coordenadas 3D do modelo para coordenadas 2D na tela.
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	// Cria a matriz de visualiza��o. Esta matriz � usada para transformar as coordenadas do modelo para o espa�o da c�mera.
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 10.0f, zoom), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	GLuint mvp = glGetUniformLocation(tableProgram, "MVP");

	glUniformMatrix4fv(mvp, 1, GL_FALSE, value_ptr(projection * view * model));

	
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
	  // Tr�s
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
	

	// Inicia o gestor de extens�es GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	//fun��escallback
	glfwSetScrollCallback(window, scrollCallBack);
	glfwSetMouseButtonCallback(window, mouseClickCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetKeyCallback(window, keyCallback);

	
	//cor background
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);


	//tem um backbuffer com uma cor e outra com os modelos logo h� uma troca de buffers
	glfwSwapBuffers(window);

	//dizer ao opengl para fazer a renderiza��o
	glViewport(0, 0, WIDTH, HEIGHT);

	// Array de informa��es dos shaders para o programa das bolas
	ShaderInfo ballshaders[] = {
		{ GL_VERTEX_SHADER,   "triangles.vert" },     // Shader de v�rtice da bola
		{ GL_FRAGMENT_SHADER, "triangles.frag" },     // Shader de fragmento da bola
		{ GL_NONE, NULL }                            // Marca��o de fim do array
	};

	// Carrega os shaders e cria o programa das bolas
	GLuint ballProgram = LoadShaders(ballshaders);
	if (!ballProgram) return -1;

	//usa o programa das bolas
	glUseProgram(ballProgram);

	// Fonte de luz ambiente global
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));

	// Fonte de luz direcional
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

	// Fonte de luz pontual #1
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 5.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

	// Fonte de luz pontual #2
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(-2.0, 2.0, 5.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].constant"), 1.0f);
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].linear"), 0.06f);
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "pointLight[1].quadratic"), 0.02f);

	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "material.emissive"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(ballProgram, glGetProgramResourceLocation(ballProgram, GL_UNIFORM, "material.shininess"), 12.0f);

	// Array de informa��es dos shaders para o programa da mesa
	ShaderInfo mesashaders[] = {
		{ GL_VERTEX_SHADER,   "MesaBilhar.vert" },     // Shader de v�rtice da mesa
		{ GL_FRAGMENT_SHADER, "MesaBilhar.frag" },     // Shader de fragmento da mesa
		{ GL_NONE, NULL }                            // Marca��o de fim do array
	};

	// Carrega os shaders e cria o programa da mesa
	GLuint tableProgram = LoadShaders(mesashaders);
	if (!tableProgram) return -1;


	//Fun��o de carregar a mesa
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


	//matriz de proje��o
	mat4 proj = mat4(1.0f);

	//matriz de view
	mat4 view = mat4(1.0f);

	// posi��o camera
	vec3 position(vec3(0.0f, 10.0f, 0.0f));

	//target da camera
	vec3 target(vec3(0.0f));

	//calculos da camera
	vec3 camFront = position;
	vec3 camRight = cross(vec3(0.0f, 1.0f, 0.0f), camFront);
	vec3 camUp(0.0f, 1.0f, 0.0f);

	//defini��o de matrizes
	proj = perspective(radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 150.0f);

	// Get the location of the uniform variables
	GLint ambientLightUniform = glGetUniformLocation(ballProgram, "ambientLightOn");
	GLint directionalLightUniform = glGetUniformLocation(ballProgram, "directionalLightOn");
	GLint pointLightUniform = glGetUniformLocation(ballProgram, "pointLightOn");

	while (!glfwWindowShouldClose(window)) {

		//limpa o buffer de cor e de profundidade
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// posi��o camera
		position = vec3(0.0f, 10.0f, zoom);
		view = lookAt(position, target, camUp);
		//matriz de zoom
		//mat4 matZoom = scale(mat4(1.0f), vec3(zoom));
		 // Bind your shader program
		glUseProgram(ballProgram);

		// Set the uniform variables
		glUniform1i(ambientLightUniform, ambientLightOn);
		glUniform1i(directionalLightUniform, directionalLightOn);
		glUniform1i(pointLightUniform, pointLightOn);


		//bind do VAO ao OpenGl
		glBindVertexArray(VAOs);

		//usa o programa das bolas
		glUseProgram(ballProgram);

		//desenhar as bolas
		ball1.zoom = zoom;
		ball1.Render(BallPositions[0], vec3(0.0f, rotation, 0.0f));
		ball2.zoom = zoom;
		ball2.Render(BallPositions[1], vec3(0.0f, rotation, 0.0f));
		ball3.zoom = zoom;
		ball3.Render(BallPositions[2], vec3(0.0f, rotation, 0.0f));
		ball4.zoom = zoom;
		ball4.Render(BallPositions[3], vec3(0.0f, rotation, 0.0f));
		ball5.zoom = zoom;
		ball5.Render(BallPositions[4], vec3(0.0f, rotation, 0.0f));
		ball6.zoom = zoom;
		ball6.Render(BallPositions[5], vec3(0.0f, rotation, 0.0f));
		ball7.zoom = zoom;
		ball7.Render(BallPositions[6], vec3(0.0f, rotation, 0.0f));
		ball8.zoom = zoom;
		ball8.Render(BallPositions[7], vec3(0.0f, rotation, 0.0f));
		ball9.zoom = zoom;
		ball9.Render(BallPositions[8], vec3(0.0f, rotation, 0.0f));
		ball10.zoom = zoom;
		ball10.Render(BallPositions[9], vec3(0.0f, rotation, 0.0f));
		ball11.zoom = zoom;
		ball11.Render(BallPositions[10], vec3(0.0f, rotation, 0.0f));
		ball12.zoom = zoom;
		ball12.Render(BallPositions[11], vec3(0.0f, rotation, 0.0f));
		ball13.zoom = zoom;
		ball13.Render(BallPositions[12], vec3(0.0f, rotation, 0.0f));
		ball14.zoom = zoom;
		ball14.Render(BallPositions[13], vec3(0.0f, rotation, 0.0f));
		ball15.zoom = zoom;
		ball15.Render(BallPositions[14], vec3(0.0f, rotation, 0.0f));


		
		//desenha mesa
		drawTable(tableProgram);
	

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
