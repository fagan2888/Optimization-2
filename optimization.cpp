#include "optimization.hpp"

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{  
	//Get graphics pointer (assumed to bound to GLFW "window pointer" associated with var. window)
	Graphics* graphics{(Graphics*)(glfwGetWindowUserPointer(window))};
	assert(graphics);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        graphics->done = true;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		glm::mat4x4 vMat = glm::lookAt(glm::vec3(0.0, 0.0, -7), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
		glUniformMatrix4fv(graphics->v_mat_loc, 1, GL_FALSE, glm::value_ptr(vMat));
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		glm::mat4x4 vMat = glm::lookAt(glm::vec3(3.0, 3.0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
		glUniformMatrix4fv(graphics->v_mat_loc, 1, GL_FALSE, glm::value_ptr(vMat));
	}
}

void DrawChart (Chart chart, float u0, float v0, int un, int vn, float umax, float vmax, std::vector<float>& tex, bool normalize=true)
{
    float ut{(umax-u0)/un}, vt{(vmax-v0)/vn};

    int ui = 0;
    std::vector<float>::iterator it = tex.begin();
    for(float u = u0; ui < un; ++ui, u += ut)
    {
        int vi = 0;
        for(float v = v0; vi < vn; ++vi, v += vt)
        {
			float col{chart(u, v)};
			*it++ = col;	
        }
    }

	if(!normalize)
		return;

	//Put all distance into range [0,1].
	float min_dist{*std::min_element(tex.begin(), tex.end())};
	float max_dist{*std::max_element(tex.begin(), tex.end())};

	if(max_dist - min_dist <= FLT_EPSILON) //Blank image
		return;

	//Normalize points
	std::transform(tex.begin(), tex.end(), tex.begin(), [&](float const& p){return (p - min_dist) / (max_dist - min_dist);});
}

float Circle (float u, float v)
{
	return 0.1f - sqrtf(u*u + v*v);
}

float Bouncy (float u, float v)
{
	return std::max(
			std::max(
			 cos(u + cos(v) * cos(v + sin(u))), 
			 2*sin(10*(sin(2*u)-v)/2)),
			2*cos(10*(u+cos(v))/2));
}

typedef glm::vec3 (*Chart3D)(float u, float v);

std::vector<float> GraphChart3D (Chart3D chart, Chart3D norm_chart, float u0, float v0, int un, int vn, float umax, float vmax)
{
    float ut{(umax-u0)/un}, vt{(vmax-v0)/vn};

    std::vector<float> mesh(un * vn * 8 * 6);
    int ui = 0;
    std::vector<float>::iterator it = mesh.begin();
    for(float u = u0; ui < un; ++ui, u += ut)
    {
        int vi = 0;
        for(float v = v0; vi < vn; ++vi, v += vt)
        {
            float u1   {u}, v1   {v},
                  u2{u+ut}, v2   {v},
                  u3   {u}, v3{v+vt},
                  u4{u+ut}, v4{v+vt};

            glm::vec3 p1{chart(u1,v1)}, 
                      p2{chart(u2,v2)}, 
                      p3{chart(u3,v3)},
                      p4{chart(u4,v4)};

            glm::vec3 n1{norm_chart(u1,v1)}, 
                      n2{norm_chart(u2,v2)}, 
                      n3{norm_chart(u3,v3)},
                      n4{norm_chart(u4,v4)};

            *it++ = p1.x; *it++ = p1.y; *it++ = p1.z;
            *it++ = n1.x; *it++ = n1.y; *it++ = n1.z;
            *it++ = (u1 - u0) / (umax-u0); *it++ = (v1 - v0) / (vmax-v0); 

            *it++ = p2.x; *it++ = p2.y; *it++ = p2.z;
            *it++ = n2.x; *it++ = n2.y; *it++ = n2.z;
            *it++ = (u2 - u0) / (umax-u0); *it++ = (v2 - v0) / (vmax-v0); 

            *it++ = p4.x; *it++ = p4.y; *it++ = p4.z;
            *it++ = n4.x; *it++ = n4.y; *it++ = n4.z;
            *it++ = (u4 - u0) / (umax-u0); *it++ = (v4 - v0) / (vmax-v0); 

            *it++ = p3.x; *it++ = p3.y; *it++ = p3.z;
            *it++ = n3.x; *it++ = n3.y; *it++ = n3.z;
            *it++ = (u3 - u0) / (umax-u0); *it++ = (v3 - v0) / (vmax-v0); 

            *it++ = p1.x; *it++ = p1.y; *it++ = p1.z;
            *it++ = n1.x; *it++ = n1.y; *it++ = n1.z;
            *it++ = (u1 - u0) / (umax-u0); *it++ = (v1 - v0) / (vmax-v0); 

            *it++ = p4.x; *it++ = p4.y; *it++ = p4.z;
            *it++ = n4.x; *it++ = n4.y; *it++ = n4.z;
            *it++ = (u4 - u0) / (umax-u0); *it++ = (v4 - v0) / (vmax-v0); 
        }
    }
    return mesh;
}

glm::vec3 YPlane (float u, float v)
{
    return {u, 0.0f, v};
}

glm::vec3 Sphere (float u, float v)
{
    return 3.0f * glm::vec3(cos(u) * cos(v), cos(u) * sin(v), sin(u));
}

glm::vec3 Torus (float u, float v)
{
    return 2.0f*glm::vec3((0.75f + 0.5f * cos(v)) * cos(u),
            (0.75f + 0.5f * cos(v)) * sin(u),
            0.5f * sin(v));
}

glm::vec3 BasicNorm (float, float)
{
    return {0.0f, 1.0f, 0.0f};
}

bool Render (double const& t, Graphics& graphics)
{
	if(graphics.done) 
		return false; 

    GLfloat const color [4] {0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0.0f, color);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUniform1f(2, t);

    glm::mat4x4 m_mat = glm::rotate(0*glm::radians((GLfloat)t), glm::vec3(0.5f, 1.0f, 0.5f));

	//Push texture 
	DrawChart(graphics.chart, -4.0f, -4.0f, graphics.tex_w, graphics.tex_h, 4.0f, 4.0f, graphics.tex_data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, graphics.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, graphics.tex_w, graphics.tex_w, 0, GL_RED, GL_FLOAT, graphics.tex_data.data()); 
    glUniform1i(graphics.sampler_loc, 0);

	//Shouldn't be necessary but included these lines anyways
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D); 

    glUniformMatrix4fv(graphics.m_mat_loc, 1, GL_FALSE, glm::value_ptr(m_mat));
    glDrawArrays(GL_TRIANGLES, 0, graphics.num_verts);

    glfwPollEvents();
    glfwSwapBuffers(graphics.window);

	return true;
}

int main ()
{   
	Graphics graphics;
    graphics.Startup(1920, 1080, "./Shaders/vert.glsl", "./Shaders/frag.glsl", "Optimization Stuff"); 


//	graphics.mesh = GraphChart3D(Torus, BasicNorm, 0.0f, 0.0f, 50, 50, 2*M_PI, 2*M_PI);
	graphics.mesh = GraphChart3D(YPlane, BasicNorm, -4.0f, -4.0f, 1, 1, 4.0f, 4.0f);
    graphics.num_verts = graphics.mesh.size() / 8;

    graphics.chart = Bouncy;
	graphics.tex_w = 400;
	graphics.tex_h = 400; 
	graphics.tex_data.resize(graphics.tex_h * graphics.tex_w);
	DrawChart(graphics.chart, -4.0f, -4.0f, graphics.tex_w, graphics.tex_h, 4.0f, 4.0f, graphics.tex_data);

    //For pallete: https://stackoverflow.com/questions/16500656/which-color-gradient-is-used-to-color-mandelbrot-in-wikipedia
	graphics.pallete_len = 48;
	graphics.pallete_data = std::vector<float>
	{
         66.0f,  30.0f,  15.0f,   // brown 3
         25.0f,   7.0f,  26.0f,   // dark violett
          9.0f,   1.0f,  47.0f,   // darkest blue
          4.0f,   4.0f,  73.0f,   // blue 5
          0.0f,   7.0f, 100.0f,   // blue 4
         12.0f,  44.0f, 138.0f,   // blue 3
         24.0f,  82.0f, 177.0f,   // blue 2
         57.0f, 125.0f, 209.0f,   // blue 1
        134.0f, 181.0f, 229.0f,   // blue 0
        211.0f, 236.0f, 248.0f,   // lightest blue
        241.0f, 233.0f, 191.0f,   // lightest yellow
        248.0f, 201.0f,  95.0f,   // light yellow
        255.0f, 170.0f,   0.0f,   // dirty yellow
        204.0f, 128.0f,   0.0f,   // brown 0
        153.0f,  87.0f,   0.0f,   // brown 1
        106.0f,  52.0f,   3.0f    // brown 2
	};

	std::transform(graphics.pallete_data.begin(), graphics.pallete_data.end(), graphics.pallete_data.begin(), [](float const& c){return c / 255.0f;});

	//RENDERING AND GRAPHICS STUFF BELOW

    glUseProgram(graphics.shader_program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	//Setup buffer and attributes
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
            graphics.mesh.size() * sizeof(glm::vec3),
            graphics.mesh.data(),
            GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), NULL); //Position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid const*)(3*sizeof(GLfloat))); //Normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid const*)(6*sizeof(GLfloat))); //UV coords
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

	//Set GLFW window pointer 
	glfwSetWindowUserPointer(graphics.window, &graphics);

	//Generate texture and pallete
    glGenTextures(1, &graphics.texture);
    glGenTextures(1, &graphics.pallete);

	//Set location of uniforms in shader
    graphics.p_mat_loc = glGetUniformLocation(graphics.shader_program, "pMat");
    graphics.v_mat_loc = glGetUniformLocation(graphics.shader_program, "vMat");
    graphics.m_mat_loc = glGetUniformLocation(graphics.shader_program, "mMat");
	graphics.sampler_loc = glGetUniformLocation(graphics.shader_program, "sampler");
	graphics.pallete_loc = glGetUniformLocation(graphics.shader_program, "pallete");

    // Init matrix uniforms 
	glm::mat4x4 pMat = glm::perspective(glm::radians(45.0f),(GLfloat)graphics.width/graphics.height, 0.1f, 100.0f); 
    glm::mat4x4 vMat = glm::lookAt(glm::vec3(0.0, 10.0, -5.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
    glUniformMatrix4fv(graphics.p_mat_loc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(graphics.v_mat_loc, 1, GL_FALSE, glm::value_ptr(vMat));

	//Set key callback
    glfwSetKeyCallback(graphics.window, KeyCallback);

	//Blending for transparency
    glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

    //These lines prevent seg fault on glTexImage2D
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	//Push pallete 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, graphics.pallete);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, graphics.pallete_len, 0, GL_RGB, GL_FLOAT, graphics.pallete_data.data()); 
    glUniform1i(graphics.pallete_loc, 1);

    glGenerateMipmap(GL_TEXTURE_1D); 

	//Main loop
	for(float t = glfwGetTime(); ; t = glfwGetTime())
	{
		if(!Render(t, graphics))
			break;
	}

	graphics.Shutdown();
}
