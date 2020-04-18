extern "C" {
#include <glad/glad.h>
};


#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "main.h"
#include "cuda_nomalize.cuh"

#define PI 3.14159


TriAngle TriangleVector[4096*3];
TriAngle_Norm TriangleNorm[4096*3];

GLfloat yRot = 0;
int pt_a = 0;//����ָ��

void ReadStlModel()
{
	pt_a = 0;
	std::ifstream infile("C:\\Users\\cyh\\Desktop\\test\\qiu.txt");
	if (!infile.is_open())
	{
		return;
	}

	std::string temp, modelname;
	char dump[256];
	int trinumber = 0;

	infile >> temp;
	int test = temp.compare("solid");
	if (test != 0)
	{
		return;
	}

	infile.getline(dump, 25);
	infile >> temp;
	while (temp.compare("facet") == 0)
	{
		trinumber++;//��������Ŀ
		infile >> temp;//get rid of "normal "

		infile >> TriangleNorm[pt_a].x;
		infile >> TriangleNorm[pt_a].y;
		infile >> TriangleNorm[pt_a].z;

		//infile >> temp;//get rid of "normal "
		//infile >> temp;//get rid of "normal "
		//infile >> temp;//get rid of "normal "

		infile.getline(dump, 256); infile.getline(dump, 256);//get rid of "outer loop"
		{
			infile >> temp;
			infile >> TriangleVector[pt_a].vertex1.x;
			infile >> TriangleVector[pt_a].vertex1.y;
			infile >> TriangleVector[pt_a].vertex1.z;
			infile >> temp;
			infile >> TriangleVector[pt_a].vertex2.x;
			infile >> TriangleVector[pt_a].vertex2.y;
			infile >> TriangleVector[pt_a].vertex2.z;
			infile >> temp;
			infile >> TriangleVector[pt_a].vertex3.x;
			infile >> TriangleVector[pt_a].vertex3.y;
			infile >> TriangleVector[pt_a].vertex3.z;
			/*std::cout << TriangleVector[pt_a].vertex1.x << "   " << TriangleVector[pt_a].vertex1.y << "   " << TriangleVector[pt_a].vertex1.z << std::endl;*/
		}

		TriangleVector[pt_a].color1.x = 1;//��ɫ
		TriangleVector[pt_a].color1.y = 0;
		TriangleVector[pt_a].color1.z = 0;
		TriangleVector[pt_a].color2 = TriangleVector[pt_a].color1;
		TriangleVector[pt_a].color3 = TriangleVector[pt_a].color1;
		pt_a++;
		infile >> temp;
		infile >> temp;
		infile >> temp;
	}//while()
}

sunVec sun_array_ex1 = { 1,-1,1 };
sunVec sun_array_ex2 = { 1,1,-1 };
sunVec sun_array[2] = { sun_array_ex1,sun_array_ex2 };
//̫���Ƕ�
void sunAngle();

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";
static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

//����ص�����
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//���̻ص�����
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//������
int main(void)
{

	ReadStlModel();

	nomalize(TriangleVector, TriangleNorm,pt_a, sun_array);

	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	//���ô���ص�����
	glfwSetErrorCallback(error_callback);

	//���initʧ��
	if (!glfwInit())exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);

	//���windows����ʧ��
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//ָ��key��Ӧ�Ļص�����
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGL();

	glfwSwapInterval(1);

	// NOTE: OpenGL error checks have been omitted for brevity

	//��TriangleVector���붥�㻺����
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVector), TriangleVector, GL_STATIC_DRAW);

	//������Ⱦ��
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	//vertex_shader_text��GLSL ����Դ��
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	//Ƭ��Ⱦ��
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	//��shader
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);


	//���־���
	mvp_location = glGetUniformLocation(program, "MVP");//modelview * projection * window ͶӰ����
	vpos_location = glGetAttribLocation(program, "vPos");//vertex position
	vcol_location = glGetAttribLocation(program, "vCol");//vertex color

												
	//�������ݽ�����ʽ
	/*��һ������ָ��������0��ʼȡ���ݣ��붥����ɫ����layout(location = 0)��Ӧ��
	�ڶ�������ָ���������Դ�С��
	����������ָ���������͡�
	���ĸ����������Ƿ�ϣ�����ݱ���׼������һ������ֻ��ʾ���򲻱�ʾ��С��
	����������ǲ�����Stride����ָ���������Ķ�������֮��ļ�������洫0�ʹ�4Ч����ͬ�������1ȡֵ��ʽΪ0123��1234��2345
	������������ʾ���ǵ�λ�������ڻ�������ʼλ�õ�ƫ������*/


	glEnableVertexAttribArray(vpos_location);//position
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Point) * 2, (void*)0);
	glEnableVertexAttribArray(vcol_location);//color
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Point) * 2, (void*)(sizeof(float) * 3));

	//��Ҫ��ѭ������
	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float ratio;
		int width, height;
		mat4x4 m, p, mvp;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		mat4x4_identity(m);
		//mat4x4_rotate_Z(m, m, (float)glfwGetTime());

		mat4x4_rotate_Y(m, m, (float)glfwGetTime());
		//mat4x4_rotate_Y(m, m, (float)3.4);
		mat4x4_rotate_X(m, m, (float)1.8);
		mat4x4_rotate_X(m, m, (float)3.1415926);

		float win_ratio = 150;
		mat4x4_ortho(p, -ratio*win_ratio, ratio*win_ratio, -win_ratio, win_ratio, win_ratio, -win_ratio);//��Ұx_left,x_right,y_left,y_right,z_front,z_back
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);

		glDrawArrays(GL_TRIANGLES, 0, (int)12 * pt_a);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}