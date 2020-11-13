/******************************************************************************|
| CPSC 4050/6050 Aileen Wei						       |		
| Computer Garphics Final Project, Daljit Singh Dhillon, 2020   	       |		
| Modified from:                                                               |
|                                                                              |
| DPA 8090 Example Code, Eric Patterson, 2017                                  |
|                                                                              |
| This is mostly plain C but uses a few things from C++ so needs C++ compiler. |
| Some OpenGL setup code here and obj_parser, math_funcs, and gl_utils         |
| are from Angton Gerdelan and "Anton's OpenGL 4 Tutorials."                   |
| http://antongerdelan.net/opengl/                                             |
| Email: anton at antongerdelan dot net                                        |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
|******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <math.h>
#include <time.h>

#include <GL/glew.h>       // Include GLEW (or new version of GL if on Windows).
#include <GLFW/glfw3.h>    // GLFW helper library.

#include "maths_funcs.h"   // Anton's maths functions.
#include "gl_utils.h"      // Anton's opengl functions and small utilities like logs
#include "obj_parser.h"    // Anton's little Wavefront .obj mesh loader

#include "stb_image.h"     // Sean Barrett's image loader with Anton's load_texture()

#define _USE_MATH_DEFINES
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

// The view and proj matrices below make-up the camera position, orientation, fov, etc.
// The model matrices moves your surface of revolution to the world coordinate system
extern mat4 view_mat;
extern mat4 proj_mat;
extern mat4 ball_translate_model_mat;
extern mat4 ball_rotate_model_mat;
extern mat4 wall_model_mat;
extern mat4 floor_model_mat;
extern mat4 head_model_mat;
extern int ball_pass_wall_no;
extern int draw_mode;

// the vector below indicates camera placement. 
//It looks at (0,0,0) with (0,1,0) as the up-vector
vec3 cam_pos (-2.0f, -0.3f, 5.0f);

float centerX=-2, centerY=0, centerZ=0;
float ballCenterX=0, ballCenterY=0, ballCenterZ=0;
float speedX_i=0.002, speedY_i=0.0015, speedZ_i=0.0006;
float speedX=speedX_i, speedY=speedY_i, speedZ=0;
float speedX_sign=-1, speedY_sign=1, speedZ_sign=1;
float bounceTimes = 1, bounceBoostCoef = 12, bounceLimit=40;
float runMode = 1, runCenter = 2.0;//runMode: 1~run, 2~retract head, 3~bounce and launch;

// Below are the declarations for three functions from stub.cpp 
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods);

GLfloat* getBallVertices(float radius, int ysteps, int thetasteps);
GLfloat* getBallNormals(float radius, int ysteps, int thetasteps);
GLfloat* getBallTexCoords(int ysteps, int thetasteps);

GLfloat* getWallVertices(float xpos, float ypos, float whl);
GLfloat* getWallNormals(float xpos, float ypos, float whl);
GLfloat* getWallTexCoords();

GLfloat* getfloorVertices(float rfhl, float rfhh, float radius);
GLfloat* getfloorNormals(float rfhl, float rfhh);
GLfloat* getfloorTexCoords();
void loadUniforms(GLuint shader_programme);

// Forward Declaration
// this function loads a .jpg or .png file into a previously activated texture map
bool load_texture (const char* file_name, GLuint* tex);


/******************************************************************************|
| argc: number of input command-line parameters                                |
| argv: array of input command-line character strings                          |
| EXPECTED INPUT                                                               |
|                                                                              |
| argv[0]: automatically asigned the program name cgfinal                      |
| argv[1], argv[2] Vertex & fragment Shader			               |
| argv[3]: texture map for original ball	 			       |
| argv[5-7]: texture map for walls					       |
| argv[8]: texture map for floor		 			       |
|******************************************************************************/

int main (int argc, char *argv[]) {
/*--------------------------------START OPENGL--------------------------------*/
	assert (restart_gl_log ());
	assert (start_gl ());        // Start glfw window with GL context within.

	glfwSetKeyCallback(g_window, keyboardFunction);

/*------------------------------CREATE GEOMETRY-------------------------------*/
	//================for ball==================
	GLuint ballVao, ball_vertex_vbo, ball_normal_vbo, ball_texture_vbo;
	glGenVertexArrays(1, &ballVao);		//vao
	glBindVertexArray(ballVao);
	float radius=0.3; int ysteps=500, thetasteps=500; int ballPoints=18*thetasteps*ysteps;
	// for ball vertex	
	GLfloat* ballVp = getBallVertices(radius, ysteps, thetasteps);
	glGenBuffers(1, &ball_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, ballPoints * sizeof (GLfloat), ballVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] ballVp;
	// for ball normals
	GLfloat* ballVpNormals = getBallNormals(radius, ysteps, thetasteps);
	glGenBuffers(1, &ball_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ball_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, ballPoints * sizeof (GLfloat), ballVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] ballVpNormals;
	// for ball textures
	GLfloat* ballVpTexCoords = getBallTexCoords(ysteps, thetasteps);
	glGenBuffers(1, &ball_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ball_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, ballPoints/3*2 * sizeof (GLfloat), ballVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] ballVpTexCoords;
	//==============for wall===========
	float whl=1.0;
	//==============for left wall===========
	GLuint lwVao, lw_vertex_vbo, lw_normal_vbo, lw_texture_vbo;
	glGenVertexArrays(1, &lwVao);		//vao
	glBindVertexArray(lwVao);
	float xpos=-whl, ypos=0; int lwPoints=18*1;
	// for lw vertex	
	GLfloat* lwVp = getWallVertices(xpos, ypos, whl);
	glGenBuffers(1, &lw_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, lw_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, lwPoints * sizeof (GLfloat), lwVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] lwVp;
	// for lw normals
	GLfloat* lwVpNormals = getWallNormals(xpos, ypos, whl);
	glGenBuffers(1, &lw_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, lw_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, lwPoints * sizeof (GLfloat), lwVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] lwVpNormals;
	// for lw textures
	GLfloat* lwVpTexCoords = getWallTexCoords();
	glGenBuffers(1, &lw_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, lw_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, lwPoints/3*2 * sizeof (GLfloat), lwVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] lwVpTexCoords;
	//==============for bottom wall===========
	GLuint bwVao, bw_vertex_vbo, bw_normal_vbo, bw_texture_vbo;
	glGenVertexArrays(1, &bwVao);		//vao
	glBindVertexArray(bwVao);
	xpos=0, ypos=-whl; int bwPoints=18*1;
	// for bw vertex	
	GLfloat* bwVp = getWallVertices(xpos, ypos, whl);
	glGenBuffers(1, &bw_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, bw_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, bwPoints * sizeof (GLfloat), bwVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] bwVp;
	// for bw normals
	GLfloat* bwVpNormals = getWallNormals(xpos, ypos, whl);
	glGenBuffers(1, &bw_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, bw_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, bwPoints * sizeof (GLfloat), bwVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] bwVpNormals;
	// for bw textures
	GLfloat* bwVpTexCoords = getWallTexCoords();
	glGenBuffers(1, &bw_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, bw_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, bwPoints/3*2 * sizeof (GLfloat), bwVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] bwVpTexCoords;
	//==============for right wall===========
	GLuint rwVao, rw_vertex_vbo, rw_normal_vbo, rw_texture_vbo;
	glGenVertexArrays(1, &rwVao);		//vao
	glBindVertexArray(rwVao);
	xpos=whl, ypos=0; int rwPoints=18*1;
	// for rw vertex	
	GLfloat* rwVp = getWallVertices(xpos, ypos, whl);
	glGenBuffers(1, &rw_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rw_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, rwPoints * sizeof (GLfloat), rwVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] rwVp;
	// for rw normals
	GLfloat* rwVpNormals = getWallNormals(xpos, ypos, whl);
	glGenBuffers(1, &rw_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rw_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, rwPoints * sizeof (GLfloat), rwVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] rwVpNormals;
	// for rw textures
	GLfloat* rwVpTexCoords = getWallTexCoords();
	glGenBuffers(1, &rw_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rw_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, rwPoints/3*2 * sizeof (GLfloat), rwVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] rwVpTexCoords;
	//==============for top wall===========
	GLuint twVao, tw_vertex_vbo, tw_normal_vbo, tw_texture_vbo;
	glGenVertexArrays(1, &twVao);		//vao
	glBindVertexArray(twVao);
	xpos=0, ypos=whl; int twPoints=18*1;
	// for tw vertex	
	GLfloat* twVp = getWallVertices(xpos, ypos, whl);
	glGenBuffers(1, &tw_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tw_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, twPoints * sizeof (GLfloat), twVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] twVp;
	// for tw normals
	GLfloat* twVpNormals = getWallNormals(xpos, ypos, whl);
	glGenBuffers(1, &tw_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tw_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, twPoints * sizeof (GLfloat), twVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] twVpNormals;
	// for tw textures
	GLfloat* twVpTexCoords = getWallTexCoords();
	glGenBuffers(1, &tw_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tw_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, twPoints/3*2 * sizeof (GLfloat), twVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] twVpTexCoords;
	//==============for run floor===========
	GLuint rfVao, rf_vertex_vbo, rf_normal_vbo, rf_texture_vbo;
	glGenVertexArrays(1, &rfVao);		//vao
	glBindVertexArray(rfVao);
	float rfhl=runCenter, rfhh=0.35; int rfPoints=18*1;
	// for rf vertex	
	GLfloat* rfVp = getfloorVertices(rfhl, rfhh, radius);
	glGenBuffers(1, &rf_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rf_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, rfPoints * sizeof (GLfloat), rfVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] rfVp;
	// for rf normals
	GLfloat* rfVpNormals = getfloorNormals(rfhl, rfhh);
	glGenBuffers(1, &rf_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rf_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, rfPoints * sizeof (GLfloat), rfVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] rfVpNormals;
	// for rf textures
	GLfloat* rfVpTexCoords = getfloorTexCoords();
	glGenBuffers(1, &rf_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rf_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, rfPoints/3*2 * sizeof (GLfloat), rfVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] rfVpTexCoords;
	//================for head==================
	GLuint headVao, head_vertex_vbo, head_normal_vbo, head_texture_vbo;
	glGenVertexArrays(1, &headVao);		//vao
	glBindVertexArray(headVao);
	float hradius=0.15;ysteps=100;thetasteps=100; int headPoints=18*thetasteps*ysteps;
	// for head vertex	
	GLfloat* headVp = getBallVertices(hradius, ysteps, thetasteps);
	glGenBuffers(1, &head_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, head_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, headPoints * sizeof (GLfloat), headVp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	delete [] headVp;
	// for head normals
	GLfloat* headVpNormals = getBallNormals(hradius, ysteps, thetasteps);
	glGenBuffers(1, &head_normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, head_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, headPoints * sizeof (GLfloat), headVpNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	delete [] headVpNormals;
	// for head textures
	GLfloat* headVpTexCoords = getBallTexCoords(ysteps, thetasteps);
	glGenBuffers(1, &head_texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, head_texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, headPoints/3*2 * sizeof (GLfloat), headVpTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	delete [] headVpTexCoords;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 
/*-------------------------------CREATE SHADERS-------------------------------*/
    // The vertex shader program generally acts to transform vertices.
    // The fragment shader is where we'll do the actual "shading."
	GLuint shader_programme = create_programme_from_files (
		argv[1], argv[2]
	);
	glUseProgram (shader_programme);
	
/*-------------------------------SETUP TEXTURES-------------------------------*/
	// load textures
	GLuint tex00;
	int tex00location = glGetUniformLocation (shader_programme, "texture00");
	glUniform1i (tex00location, 0);
	glActiveTexture (GL_TEXTURE0);
	assert (load_texture (argv[3], &tex00));
	
	GLuint tex01;
	int tex01location = glGetUniformLocation (shader_programme, "texture01");
	glUniform1i (tex01location, 1);
	glActiveTexture (GL_TEXTURE1);
	assert (load_texture (argv[4], &tex01));
	
	GLuint tex02;
	int tex02location = glGetUniformLocation (shader_programme, "texture02");
	glUniform1i (tex02location, 2);
	glActiveTexture (GL_TEXTURE2);
	assert (load_texture (argv[5], &tex02));
	
	GLuint tex03;
	int tex03location = glGetUniformLocation (shader_programme, "texture03");
	glUniform1i (tex03location, 3);
	glActiveTexture (GL_TEXTURE3);
	assert (load_texture (argv[6], &tex03));

	GLuint tex04;
	int tex04location = glGetUniformLocation (shader_programme, "texture04");
	glUniform1i (tex04location, 4);
	glActiveTexture (GL_TEXTURE4);
	assert (load_texture (argv[7], &tex04));
	
	GLuint tex05;
	int tex05location = glGetUniformLocation (shader_programme, "texture05");
	glUniform1i (tex05location, 5);
	glActiveTexture (GL_TEXTURE5);
	assert (load_texture (argv[8], &tex05));

	GLuint tex06;
	int tex06location = glGetUniformLocation (shader_programme, "texture06");
	glUniform1i (tex06location, 6);
	glActiveTexture (GL_TEXTURE6);
	assert (load_texture (argv[9], &tex06));


/*-------------------------------CREATE CAMERA--------------------------------*/

        // This sets up the matrix transformations that act as the camera lens
        // and sensor would to transform vertices from view space.
	float near = 0.1f;   // clipping planes
	float far = 1000.0f; 
	float fovy = 35.0f;  // vertical field of view, horiz calculated for aspect
	float aspect = (float)g_gl_width / (float)g_gl_height;      // aspect ratio
	proj_mat = perspective (fovy, aspect, near, far);
	//mat4 tmp_view_mat = translate (identity_mat4 (), vec3 (-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2]));
	// would usually use inverse camera orientation with position to construct
	// view matrix, but for simplicity since camera axis-aligned, not needed here
	//view_mat =  tmp_view_mat;
	//view_mat = translate(view_mat, vec3(-runCenter, 0, 0)); 
	//view_mat = look_at(cam_pos, vec3(runCenter, 0, 0), vec3(0, 1, 0));
	view_mat = identity_mat4 ();
	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/
	// The model matrix stores the position and orientation transformations for your SoR
	ball_translate_model_mat = translate(identity_mat4(), vec3(runCenter, 0, 0));
	ball_rotate_model_mat = identity_mat4();
	floor_model_mat = identity_mat4();
	wall_model_mat = translate(identity_mat4(), vec3(centerX, centerY, centerZ));
	head_model_mat = translate(identity_mat4(), vec3(0, 0, radius+hradius));
	head_model_mat = translate(head_model_mat, vec3(runCenter, 0, 0));
	runMode = 1;
	ball_pass_wall_no = 0;
	speedX=speedX_i; speedY=speedY_i; speedZ=0;
	speedX_sign=-1; speedY_sign=1; speedZ_sign=1;
	bounceTimes = 1; 
	// Setup basic GL display attributes.	
	glEnable (GL_DEPTH_TEST);   // enable depth-testing
	glDepthFunc (GL_LESS);      // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE);    // cull face
	glCullFace (GL_BACK);       // cull back face
	glFrontFace (GL_CCW);       // set counter-clock-wise vertex order to mean the front
	glClearColor (0.1, 0.1, 0.1, 1.0);   // non-black background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height); // make sure correct aspect ratio

	float headCenter = radius + hradius;
/*-------------------------------RENDERING LOOP-------------------------------*/
	while (!glfwWindowShouldClose (g_window)) {
		// update timers
		static double previous_seconds = glfwGetTime ();
		double current_seconds = glfwGetTime ();
		double elapsed_seconds = current_seconds - previous_seconds;
		//printf("\nelapsed time: %f\n", elapsed_seconds);
		double rotAmt = -60 * elapsed_seconds; 
		printf("\nrotAmt: %f, radRotAmt: %f\n", rotAmt, rotAmt*ONE_DEG_IN_RAD);
		previous_seconds = current_seconds;
		_update_fps_counter (g_window);
		// clear graphics context
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// setup shader use	
		glUseProgram (shader_programme);
		if (runMode == 1){
		    // ball
		    ball_rotate_model_mat = rotate_y_deg(ball_rotate_model_mat, rotAmt);
		    ball_translate_model_mat = translate(ball_translate_model_mat, vec3(0.3*rotAmt*ONE_DEG_IN_RAD, 0, 0));
		    head_model_mat = translate(head_model_mat, vec3(0.3*rotAmt*ONE_DEG_IN_RAD, 0, 0));
		    //view_mat = translate(view_mat, vec3(-0.3*rotAmt*ONE_DEG_IN_RAD, 0, 0));
		    view_mat = look_at(cam_pos, vec3(runCenter,0,0), vec3(0,0,1));
		    runCenter += 0.3*rotAmt*ONE_DEG_IN_RAD;
		    if (runCenter <= centerX)
			runMode = 2;
		    //floor
		    draw_mode = 5;
		    loadUniforms(shader_programme);
		    glBindVertexArray(rfVao);
		    glDrawArrays(GL_TRIANGLES, 0, rfPoints);
		    // head
		    draw_mode = 6;
		    loadUniforms(shader_programme);
		    glBindVertexArray(headVao);
		    glDrawArrays(GL_TRIANGLES, 0, headPoints);  
		}
		else{
		    if (runMode == 2){
		    	head_model_mat = translate(head_model_mat, vec3(0, 0, 0.15*rotAmt*ONE_DEG_IN_RAD));
			headCenter += 0.15*rotAmt*ONE_DEG_IN_RAD;
			if (headCenter <= radius - hradius)
			    runMode = 3;
			// head
		    	draw_mode = 6;
		    	loadUniforms(shader_programme);
		    	glBindVertexArray(headVao);
		    	glDrawArrays(GL_TRIANGLES, 0, headPoints);  
		    }
		    else{
		    // ball
		    if (bounceTimes <= bounceLimit){
		        ball_translate_model_mat = translate(ball_translate_model_mat, vec3(speedX*speedX_sign, speedY*speedY_sign, speedZ*speedZ_sign));
		        ballCenterX += speedX*speedX_sign; ballCenterY += speedY*speedY_sign; ballCenterZ += speedZ*speedZ_sign;
		        if (ballCenterX <= -whl+radius){bounceTimes += 1; ball_pass_wall_no=1; speedX_sign = 1; speedX *= (1+bounceBoostCoef*elapsed_seconds); speedY *= (1+bounceBoostCoef*elapsed_seconds);}
		        if (ballCenterX >= whl-radius){bounceTimes += 1; ball_pass_wall_no=3; speedX_sign = -1; speedX *= (1+bounceBoostCoef*elapsed_seconds); speedY *= (1+bounceBoostCoef*elapsed_seconds);}
		        if (ballCenterY <= -whl+radius){bounceTimes += 1; ball_pass_wall_no=2; speedY_sign = 1; speedX *= (1+bounceBoostCoef*elapsed_seconds); speedY *= (1+bounceBoostCoef*elapsed_seconds);}
		        if (ballCenterY >= whl-radius){bounceTimes += 1; ball_pass_wall_no=4; speedY_sign = -1; speedX *= (1+bounceBoostCoef*elapsed_seconds); speedY *= (1+bounceBoostCoef*elapsed_seconds);}
		        //printf("\nbounceTimes: %f, Xsign: %f, Ysign: %f, Zsign: %f\n,", bounceTimes, speedX_sign, speedY_sign, speedZ_sign);
			printf("\nbounceTimes: %f, speedX: %f, speedY: %f, speedZ: %f\n,", bounceTimes, speedX, speedY, speedZ);
		      }
		    else{
		        printf("\nspeedX: %f, speedY: %f, speedZ: %f\n", speedX, speedY, speedZ);
		        speedX=speedX_i; speedY=speedY_i; speedZ=speedZ_i;
 		        ball_translate_model_mat = translate(ball_translate_model_mat, vec3(speedX*speedX_sign, speedY*speedY_sign, speedZ*speedZ_sign));
		        ballCenterX += speedX*speedX_sign; ballCenterY += speedY*speedY_sign; ballCenterZ += speedZ*speedZ_sign;
		        if (ballCenterZ <= whl+radius){
		            if (ballCenterX <= -whl+radius){ball_pass_wall_no=1; speedX_sign= 1;}
		            if (ballCenterX >= whl-radius){ball_pass_wall_no=3; speedX_sign= -1;}
		            if (ballCenterY <= -whl+radius){ball_pass_wall_no=2; speedY_sign= 1;}
		            if (ballCenterY >= whl-radius){ball_pass_wall_no=4; speedY_sign= -1;}
		        }
		      }
		    }
		    // right wall
		    draw_mode = 3;
		    loadUniforms(shader_programme);
		    glBindVertexArray(rwVao);
		    glDrawArrays(GL_TRIANGLES, 0, rwPoints);
		}
		draw_mode = 0;
		//ball_rotate_model_mat = rotate_y_deg(ball_rotate_model_mat, 2.5*rotAmt);
		loadUniforms(shader_programme);
		glBindVertexArray(ballVao);
		glDrawArrays(GL_TRIANGLES, 0, ballPoints);
		// left wall
		draw_mode = 1;
		loadUniforms(shader_programme);
		glBindVertexArray(lwVao);
		glDrawArrays(GL_TRIANGLES, 0, lwPoints);
		// bot wall
		draw_mode = 2;
		loadUniforms(shader_programme);
		glBindVertexArray(bwVao);
		glDrawArrays(GL_TRIANGLES, 0, bwPoints);
		// top wall
		draw_mode = 4;
		loadUniforms(shader_programme);
		glBindVertexArray(twVao);
		glDrawArrays(GL_TRIANGLES, 0, twPoints);

		// update other events like input handling 
		glfwPollEvents ();

		aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
		proj_mat = perspective (fovy, aspect, near, far);
	  
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers (g_window);
	}
	
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}

// this function loads a .jpg or .png file into a previously activated texture map
bool load_texture (const char* file_name, GLuint* tex) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load (file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf (stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf (
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures (1, tex);
	glBindTexture (GL_TEXTURE_2D, *tex);
	glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);
	glGenerateMipmap (GL_TEXTURE_2D);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}
