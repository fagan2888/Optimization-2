#pragma once 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <stack>
#include <random>

typedef float (*Chart)(float, float);

//Graphics struct holding all the graphics stuff we care about. An amalgamation of mostly old reused code. 
//Doesn't do rendering or initialization of program-specific stuff.

struct Graphics 
{
    Chart chart; 
    std::vector<float> mesh;
    size_t num_verts;
    GLuint vao;
    GLint width, height;
    GLuint shader_program;
    GLuint p_mat_loc, v_mat_loc, m_mat_loc;
    GLFWwindow* window;
	bool done;

    GLuint texture;
	size_t tex_w, tex_h;
    GLuint sampler_loc;
	std::vector<float> tex_data; 

	GLuint pallete;
	size_t pallete_len;
	GLuint pallete_loc;
	std::vector<float> pallete_data; 

    Graphics () = default;

    GLuint LoadInShader(GLenum const &shaderType, char const *fname) 
	{
       std::vector<char> buffer;
       std::ifstream in;
       in.open(fname, std::ios::binary);

       if (in.is_open()) {
          in.seekg(0, std::ios::end);
          size_t const &length = in.tellg();

          in.seekg(0, std::ios::beg);

          buffer.resize(length + 1);
          in.read(&buffer[0], length);
          in.close();
          buffer[length] = '\0';
       } else {
          std::cerr << "Unable to open " << fname << std::endl;
          exit(-1);
       }

       GLchar const *src = &buffer[0];

       GLuint shader = glCreateShader(shaderType);
       glShaderSource(shader, 1, &src, NULL);
       glCompileShader(shader);
       GLint test;
       glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

       if (!test) {
          std::cerr << "Shader compilation failed with this message:" << std::endl;
          std::vector<char> compilationLog(512);
          glGetShaderInfoLog(shader, compilationLog.size(), NULL, &compilationLog[0]);
          std::cerr << &compilationLog[0] << std::endl;
          glfwTerminate();
          exit(-1);
       }

       return shader;
    }

    void InitShaders (char const* vert_loc, char const* frag_loc)
    {   
        shader_program = glCreateProgram();

        GLuint vertShader = LoadInShader(GL_VERTEX_SHADER, vert_loc);
        GLuint fragShader = LoadInShader(GL_FRAGMENT_SHADER, frag_loc);

        glAttachShader(shader_program, vertShader);
        glAttachShader(shader_program, fragShader);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        glLinkProgram(shader_program);
    }

    void Startup (GLfloat const& width_, GLfloat const& height_, char const* vert_loc, char const* frag_loc, const char* title="Untitled Window")
    {   
		done = false;
        width = width_; 
        height = height_;

        if(!glfwInit()) 
		{
            std::cerr<<"failed to initialize GLFW"<<std::endl;
            exit(EXIT_SUCCESS);
        }

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        window = glfwCreateWindow(width_, height_, title, NULL, NULL);
        if(!window) 
		{
            std::cerr<<"failed to initialize window"<<std::endl;
            exit(EXIT_SUCCESS);
        }
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        if(glewInit() != 0) 
		{
            std::cerr<<"failed to initialize GLEW"<<std::endl;
            exit(EXIT_SUCCESS);
        }

        InitShaders(vert_loc, frag_loc);
    }

	void Shutdown () 
	{   
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(shader_program);
	}
};
