#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;


#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"

const std::string VERTEX_SHADER =
#include "LED.vertexshader"
;

const std::string FRAGMENT_SHADER =
#include "LED.fragmentshader"
;

#include "LedTexture.cpp"


// CPU representation of a particle
struct Particle{
   Particle(double x, double y, double z): pos(glm::vec3(x,y,z)), size(0.5)
      {
      }
	glm::vec3 pos;
	unsigned char r,g,b,a; // Color
	float size;
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};


float vsc = 0.5;
float hsc = 0.3;

Particle ParticlesContainer[] =
{
   Particle(hsc*0,  vsc*30, -20),
   Particle(hsc*1,  vsc*29, -20),
   Particle(hsc*2,  vsc*28, -20),
   Particle(hsc*3,  vsc*27, -20),
   Particle(hsc*4,  vsc*26, -20),
   Particle(hsc*5,  vsc*25, -20),
   Particle(hsc*6,  vsc*24, -20),
   Particle(hsc*7,  vsc*23, -20),
   Particle(hsc*8,  vsc*22, -20),
   Particle(hsc*9,  vsc*21, -20),
   Particle(hsc*10, vsc*20, -20),
   Particle(hsc*11, vsc*19, -20),
   Particle(hsc*12, vsc*18, -20),
   Particle(hsc*13, vsc*17, -20),
   Particle(hsc*14, vsc*16, -20),
   Particle(hsc*15, vsc*15, -20),
   Particle(hsc*16, vsc*14, -20),
   Particle(hsc*17, vsc*13, -20),
   Particle(hsc*18, vsc*12, -20),
   Particle(hsc*19, vsc*11, -20),
   Particle(hsc*20, vsc*10, -20),
   Particle(hsc*21, vsc*9, -20),
   Particle(hsc*22, vsc*8, -20),
   Particle(hsc*23, vsc*7, -20),
   Particle(hsc*24, vsc*6, -20),
   Particle(hsc*25, vsc*5, -20),
   Particle(hsc*26, vsc*4, -20),
   Particle(hsc*27, vsc*3, -20),
   Particle(hsc*28, vsc*2, -20),
   Particle(hsc*29, vsc*1, -20),
   Particle(hsc*30, vsc*0, -20),

   Particle(hsc*28,  0, -20),
   Particle(hsc*26,  0, -20),
   Particle(hsc*24,  0, -20),
   Particle(hsc*22,  0, -20),
   Particle(hsc*20,  0, -20),
   Particle(hsc*18,  0, -20),
   Particle(hsc*16,  0, -20),
   Particle(hsc*14,  0, -20),
   Particle(hsc*12,  0, -20),
   Particle(hsc*10,  0, -20),
   Particle(hsc*8,   0, -20),
   Particle(hsc*6,   0, -20),
   Particle(hsc*4,   0, -20),
   Particle(hsc*2,   0, -20),
   Particle(hsc*0,   0, -20),
   Particle(-hsc*2,   0, -20),
   Particle(-hsc*4,   0, -20),
   Particle(-hsc*6,   0, -20),
   Particle(-hsc*8,   0, -20),
   Particle(-hsc*10,  0, -20),
   Particle(-hsc*12,  0, -20),
   Particle(-hsc*14,  0, -20),
   Particle(-hsc*16,  0, -20),
   Particle(-hsc*18,  0, -20),
   Particle(-hsc*20,  0, -20),
   Particle(-hsc*22,  0, -20),
   Particle(-hsc*24,  0, -20),
   Particle(-hsc*26,  0, -20),
   Particle(-hsc*28,  0, -20),

   
   Particle(-hsc*30, vsc*0, -20),
   Particle(-hsc*29, vsc*1, -20),
   Particle(-hsc*28, vsc*2, -20),
   Particle(-hsc*27, vsc*3, -20),
   Particle(-hsc*26, vsc*4, -20),
   Particle(-hsc*25, vsc*5, -20),
   Particle(-hsc*24, vsc*6, -20),
   Particle(-hsc*23, vsc*7, -20),
   Particle(-hsc*22, vsc*8, -20),
   Particle(-hsc*21, vsc*9, -20),
   Particle(-hsc*20, vsc*10, -20),
   Particle(-hsc*19, vsc*11, -20),
   Particle(-hsc*18, vsc*12, -20),
   Particle(-hsc*17, vsc*13, -20),
   Particle(-hsc*16, vsc*14, -20),
   Particle(-hsc*15, vsc*15, -20),
   Particle(-hsc*14, vsc*16, -20),
   Particle(-hsc*13, vsc*17, -20),
   Particle(-hsc*12, vsc*18, -20),
   Particle(-hsc*11, vsc*19, -20),
   Particle(-hsc*10, vsc*20, -20),
   Particle(-hsc*9,  vsc*21, -20),
   Particle(-hsc*8,  vsc*22, -20),
   Particle(-hsc*7,  vsc*23, -20),
   Particle(-hsc*6,  vsc*24, -20),
   Particle(-hsc*5,  vsc*25, -20),
   Particle(-hsc*4,  vsc*26, -20),
   Particle(-hsc*3,  vsc*27, -20),
   Particle(-hsc*2,  vsc*28, -20),
   Particle(-hsc*1,  vsc*29, -20),
   Particle(-hsc*0,  vsc*30, -20),
};

int MaxParticles = sizeof(ParticlesContainer)/sizeof(*ParticlesContainer);

void SortParticles(){
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "FastLED OpenGL", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
   //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
   
	// Vertex shader
	GLuint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

	// fragment shader
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	
	static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];

	GLuint Texture = loadDDS(blobs::LedTexture, sizeof(blobs::LedTexture));

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = { 
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	
	double lastTime = glfwGetTime();
	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;


		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
      

		// We will need the camera's position in order to sort the particles
		// w.r.t the camera's distance.
		// There should be a getCameraPosition() function in common/controls.cpp, 
		// but this works too.
		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
 

		for(int i=0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

         p.r = (sin(5*lastTime+float(i)/(10))+1)*127;
         p.g = (cos(5*lastTime+float(i)/(10))+1)*127;
         p.b = (sin(10*lastTime+float(i)/(10))+1)*127;
         p.a = 255;
         
         // Simulate simple physics : gravity only, no collisions
         //p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
         // p.pos += p.speed * (float)delta;
         p.cameradistance = glm::length2( p.pos - CameraPosition );
         //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

         // Fill the GPU buffer
         g_particule_position_size_data[4*i+0] = p.pos.x;
         g_particule_position_size_data[4*i+1] = p.pos.y;
         g_particule_position_size_data[4*i+2] = p.pos.z;
			
         g_particule_position_size_data[4*i+3] = p.size;
			
         g_particule_color_data[4*i+0] = p.r;
         g_particule_color_data[4*i+1] = p.g;
         g_particule_color_data[4*i+2] = p.b;
         g_particule_color_data[4*i+3] = p.a;
         
      }

		//SortParticles();
      //printf("%f, %f, %f\n", CameraPosition.x, CameraPosition.y, CameraPosition.z);



		// Update the buffers that OpenGL uses for rendering.
		// There are much more sophisticated means to stream data from the CPU to the GPU, 
		// but this is outside the scope of this tutorial.
		// http://www.opengl.org/wiki/Buffer_Object_Streaming


		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, MaxParticles * sizeof(GLfloat) * 4, g_particule_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, MaxParticles * sizeof(GLubyte) * 4, g_particule_color_data);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// Same as the billboards tutorial
		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		
		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// These functions are specific to glDrawArrays*Instanced*.
		// The first parameter is the attribute buffer we're talking about.
		// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
		// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

		// Draw the particules !
		// This draws many times a small triangle_strip (which looks like a quad).
		// This is equivalent to :
		// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
		// but faster.
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, MaxParticles);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	delete[] g_particule_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
	
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

