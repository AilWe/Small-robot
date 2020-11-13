#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>


#include <math.h>
#include <time.h>

#include "maths_funcs.h"   // Anton's maths functions.
#include "gl_utils.h"      // Anton's opengl functions and small utilities like logs
#include "stb_image.h"     // Sean Barrett's image loader with Anton's load_texture()

#define _USE_MATH_DEFINES
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
#define MY_PI 3.1415926

mat4 view_mat;
mat4 proj_mat;
mat4 ball_translate_model_mat;
mat4 ball_rotate_model_mat;
mat4 wall_model_mat;
mat4 floor_model_mat;
mat4 head_model_mat;
int ball_pass_wall_no = 0;
int draw_mode = 0;

//forwarding definition
GLfloat* getBallVertices(float radius, int ysteps, int thetasteps);
GLfloat* getBallNormals(float radius, int ysteps, int thetasteps);
GLfloat* getBallTexCoords(int ysteps, int thetasteps);
void loadUniforms(GLuint shader_programme);

GLfloat* getWallVertices(float xpos, float ypos, float whl);
GLfloat* getWallNormals(float xpos, float ypos, float whl);
GLfloat* getWallTexCoords();

GLfloat* getfloorVertices(float rfhl, float rfhh, float radius);
GLfloat* getfloorNormals(float rfhl, float rfhh);
GLfloat* getfloorTexCoords();
void loadUniforms(GLuint shader_programme)
{	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/

	// Choose vertex and fragment shaders to use as well as view and proj matrices.
	int ball_model_mat_location = glGetUniformLocation (shader_programme, "ball_model_mat");
	int wall_model_mat_location = glGetUniformLocation (shader_programme, "wall_model_mat");
	int floor_model_mat_location = glGetUniformLocation (shader_programme, "floor_model_mat");
	int head_model_mat_location = glGetUniformLocation (shader_programme, "head_model_mat");
	int view_mat_location  = glGetUniformLocation (shader_programme, "view_mat");
	int proj_mat_location  = glGetUniformLocation (shader_programme, "proj_mat");
	
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat.m);
	glUniformMatrix4fv (ball_model_mat_location, 1, GL_FALSE, (ball_translate_model_mat*ball_rotate_model_mat).m);
	glUniformMatrix4fv (wall_model_mat_location, 1, GL_FALSE, wall_model_mat.m);
	glUniformMatrix4fv (floor_model_mat_location, 1, GL_FALSE, floor_model_mat.m);
	glUniformMatrix4fv (head_model_mat_location, 1, GL_FALSE, head_model_mat.m);
	
	// WRITE CODE TO LOAD OTHER UNIFORM VARIABLES LIKE FLAGS FOR ENABLING OR DISABLING CERTAIN FUNCTIONALITIES
	int ball_pass_wall_no_location = glGetUniformLocation (shader_programme, "ball_pass_wall_no");
	int draw_mode_location = glGetUniformLocation (shader_programme, "draw_mode");
	glUniform1i(ball_pass_wall_no_location, ball_pass_wall_no);
	glUniform1i(draw_mode_location, draw_mode);
}

	
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// MODIFY THIS FUNCTION FOR KEYBOARD INTERACTIVITY
	//GLFW Reference Links:
	// Callback Example: https://www.glfw.org/docs/3.3/input_guide.html#input_key
	// List of Keys: https://www.glfw.org/docs/3.3/group__keys.html
	
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
	printf("\nKey 'A' pressed, move light source left.\n");

    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
	printf("\nKey 'R' pressed, reset everything.\n");
	ball_pass_wall_no = 0;
    }
	
    if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
	// Close window when esacape is pressed		
	glfwSetWindowShouldClose (g_window, 1);
    }

}

GLfloat* getBallVertices(float radius, int ysteps, int thetasteps){
    float *pairY, *pairR;   // save (y,r)pair
    pairY = new float[ysteps+1]; pairR = new float[ysteps+1];
    float deltaY = 2 * radius / ysteps;
    //printf("deltaY:%f", deltaY);
    int k, j;
    for (k=0; k<=ysteps; k++){
        pairY[k] = -radius + k * deltaY;
        pairR[k] = sqrt(radius*radius - pairY[k]*pairY[k]);
	//printf("pairY, pairR: %f, %f", pairY[k], pairR[k]);
    }
    // after get (y, r) pair, start drawing revolution with 2 for loops
    float theta1, theta2, posx1, posz1, posx2, posz2;
    float delta = 2 * MY_PI / thetasteps;
    GLfloat * myVp = new GLfloat[6*3*ysteps*thetasteps];
    int pntCnt = 0; 
    for (j = 0; j < thetasteps; j++){
        theta1 = j * delta;
        theta2 = theta1 + delta;
        //glColor3f (0.0, 0.5*theta1/MY_PI, 0.0);                 
        for (k = 0; k < ysteps; k++){
            posx1 = pairR[k]*cos(theta1); posz1 = pairR[k]*sin(theta1);
            posx2 = pairR[k]*cos(theta2); posz2 = pairR[k]*sin(theta2);
	    //pntCnt += 18 * j * k;
            //face 1, vertex 1
	    myVp[pntCnt++] = posx1; //x
	    myVp[pntCnt++] = pairY[k+1]; //y
	    myVp[pntCnt++] = posz1; //z
	    //face 1, vertex 2
	    myVp[pntCnt++] = posx2; //x
	    myVp[pntCnt++] = pairY[k+1]; //y
	    myVp[pntCnt++] = posz2; //z
	    //face 1, vertex 3
	    myVp[pntCnt++] = posx1; //x
	    myVp[pntCnt++] = pairY[k]; //y
	    myVp[pntCnt++] = posz1; //z
		
	    //face 2, vertex 1
	    myVp[pntCnt++] = posx1; //x
	    myVp[pntCnt++] = pairY[k]; //y
	    myVp[pntCnt++] = posz1; //z
	    //face 2, vertex 2
	    myVp[pntCnt++] = posx2; //x
	    myVp[pntCnt++] = pairY[k+1]; //y
	    myVp[pntCnt++] = posz2; //z
	    //face 2, vertex 3
	    myVp[pntCnt++] = posx2; //x
	    myVp[pntCnt++] = pairY[k]; //y
	    myVp[pntCnt++] = posz2; //z
        }
    }
    delete[] pairY;
    delete[] pairR;
    return myVp;
}

GLfloat* getBallNormals(float radius, int ysteps, int thetasteps){
    float *pairY, *pairR;   // save (y,r)pair
    pairY = new float[ysteps+1]; pairR = new float[ysteps+1];
    float deltaY = 2*radius / ysteps;
    //printf("deltaY:%f", deltaY);
    int k, j;
    for (k=0; k<=ysteps; k++){
        pairY[k] = -radius + k * deltaY;
        pairR[k] = sqrt(radius*radius - pairY[k]*pairY[k]);
    }
    // after get (y, r) pair, start drawing revolution with 2 for loops
    float theta1, posx1, posz1;
    float delta = 2 * MY_PI / thetasteps;
    int dim1 = thetasteps + 1, dim2 = ysteps + 1, dim3 = 3;
    //float *allPointsIndices = new float[thetasteps+1][ysteps+1][3];
    float *allPointsIndices = new float[dim1*dim2*dim3];
    for (j = 0; j <= thetasteps; j++){
        theta1 = j * delta;
        //glColor3f (0.0, 0.5*theta1/MY_PI, 0.0);                 
        for (k = 0; k <= ysteps; k++){
            posx1 = pairR[k]*cos(theta1); posz1 = pairR[k]*sin(theta1);
            allPointsIndices[j*dim2*dim3+k*dim3+0] = posx1;
	    allPointsIndices[j*dim2*dim3+k*dim3+1] = pairY[k];
            allPointsIndices[j*dim2*dim3+k*dim3+2] = posz1;
	}
    }
    delete[] pairY;
    delete[] pairR;
    float *allPointsNormals = new float[dim1*dim2*dim3];
    vec3 a(0,0,0), b(0,0,0), c(0,0,0), d(0,0,0), normal(0,0,0), zeroVec3(0,0,0); 
    for (j = 0; j <= thetasteps; j++){
        for (k = 0; k <= ysteps; k++){
	    // vector a point to left, b point to bottom, c point to right, d point to top
	    if (j - 1 < 0){
	        a.v[0] = allPointsIndices[(thetasteps-1)*dim2*dim3+k*dim3+0] - allPointsIndices[j*dim2*dim3+k*dim3+0];
	        a.v[1] = allPointsIndices[(thetasteps-1)*dim2*dim3+k*dim3+1] - allPointsIndices[j*dim2*dim3+k*dim3+1];
	        a.v[2] = allPointsIndices[(thetasteps-1)*dim2*dim3+k*dim3+2] - allPointsIndices[j*dim2*dim3+k*dim3+2];
	    }
	    else{
	        a.v[0] = allPointsIndices[(j-1)*dim2*dim3+k*dim3+0] - allPointsIndices[j*dim2*dim3+k*dim3+0];
	        a.v[1] = allPointsIndices[(j-1)*dim2*dim3+k*dim3+1] - allPointsIndices[j*dim2*dim3+k*dim3+1];
	        a.v[2] = allPointsIndices[(j-1)*dim2*dim3+k*dim3+2] - allPointsIndices[j*dim2*dim3+k*dim3+2];
	    }
	    if (j + 1 > thetasteps){
		c.v[0] = allPointsIndices[1*dim2*dim3+k*dim3+0] - allPointsIndices[j*dim2*dim3+k*dim3+0];
		c.v[1] = allPointsIndices[1*dim2*dim3+k*dim3+1] - allPointsIndices[j*dim2*dim3+k*dim3+1];
		c.v[2] = allPointsIndices[1*dim2*dim3+k*dim3+2] - allPointsIndices[j*dim2*dim3+k*dim3+2];
	    }
	    else{
	    	c.v[0] = allPointsIndices[(j+1)*dim2*dim3+k*dim3+0] - allPointsIndices[j*dim2*dim3+k*dim3+0];
	    	c.v[1] = allPointsIndices[(j+1)*dim2*dim3+k*dim3+1] - allPointsIndices[j*dim2*dim3+k*dim3+1];
	    	c.v[2] = allPointsIndices[(j+1)*dim2*dim3+k*dim3+2] - allPointsIndices[j*dim2*dim3+k*dim3+2];
	    }
	    if (k - 1 < 0)
		d = zeroVec3;
	    else{
		d.v[0] = allPointsIndices[j*dim2*dim3+(k-1)*dim3+0] - allPointsIndices[j*dim2*dim3+k*dim3+0];
		d.v[1] = allPointsIndices[j*dim2*dim3+(k-1)*dim3+1] - allPointsIndices[j*dim2*dim3+k*dim3+1];
		d.v[2] = allPointsIndices[j*dim2*dim3+(k-1)*dim3+2] - allPointsIndices[j*dim2*dim3+k*dim3+2];
	    }
	    if (k + 1 > ysteps)
		b = zeroVec3;
	    else{
		b.v[0] = allPointsIndices[j*dim2*dim3+(k+1)*dim3+0] - allPointsIndices[j*dim2*dim3+k*dim3+0];
		b.v[1] = allPointsIndices[j*dim2*dim3+(k+1)*dim3+1] - allPointsIndices[j*dim2*dim3+k*dim3+1];
		b.v[2] = allPointsIndices[j*dim2*dim3+(k+1)*dim3+2] - allPointsIndices[j*dim2*dim3+k*dim3+2];
	    }
	    normal = normalise(normalise(cross(a, b))+normalise(cross(b, c))+normalise(cross(c, d))+normalise(cross(d, a)));
	    allPointsNormals[j*dim2*dim3+k*dim3+0] = normal.v[0];            
	    allPointsNormals[j*dim2*dim3+k*dim3+1] = normal.v[1];            
	    allPointsNormals[j*dim2*dim3+k*dim3+2] = normal.v[2];
	}
    }
    delete [] allPointsIndices;
    GLfloat * myVp = new GLfloat[6*3*ysteps*thetasteps];
    int pntCnt = 0;
    for (j = 0; j < thetasteps; j++){
        for (k = 0; k < ysteps; k++){
	    //pntCnt += 18 * j * k;
            //face 1, vertex 1
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+(k+1)*dim3+0]; //x
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+(k+1)*dim3+1]; //y
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+(k+1)*dim3+2]; //z
	    //face 1, vertex 2
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+(k+1)*dim3+0]; //x
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+(k+1)*dim3+1]; //y
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+(k+1)*dim3+2]; //z
	    //face 1, vertex 3
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+k*dim3+0]; //x
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+k*dim3+1]; //y
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+k*dim3+2]; //z
		
	    //face 2, vertex 1
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+k*dim3+0]; //x
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+k*dim3+1]; //y
	    myVp[pntCnt++] = allPointsNormals[j*dim2*dim3+k*dim3+2]; //z
	    //face 2, vertex 2
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+(k+1)*dim3+0]; //x
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+(k+1)*dim3+1]; //y
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+(k+1)*dim3+2]; //z
	    //face 2, vertex 3
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+k*dim3+0]; //x
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+k*dim3+1]; //y
	    myVp[pntCnt++] = allPointsNormals[(j+1)*dim2*dim3+k*dim3+2]; //z
        }
    }
    delete [] allPointsNormals;
    return myVp;
}

GLfloat* getBallTexCoords(int ysteps, int thetasteps){
    GLfloat * myVp = new GLfloat[6*2*ysteps*thetasteps];
    int pntCnt = 0, k, j; 
    for (j = 0; j < thetasteps; j++){
        //glColor3f (0.0, 0.5*theta1/MY_PI, 0.0);                 
        for (k = 0; k < ysteps; k++){

            //face 1, vertex 1
	    myVp[pntCnt++] = 1.0 * j / thetasteps; //x
	    myVp[pntCnt++] = 1.0 * (k+1) / ysteps; //y
	    
	    //face 1, vertex 2
	    myVp[pntCnt++] = 1.0 * (j+1) / thetasteps; //x
	    myVp[pntCnt++] = 1.0 * (k+1) / ysteps; //y
	   
	    //face 1, vertex 3
	    myVp[pntCnt++] = 1.0 * j / thetasteps; //x
	    myVp[pntCnt++] = 1.0 * k / ysteps; //y
	   	
	    //face 2, vertex 1
	    myVp[pntCnt++] = 1.0 * j / thetasteps; //x
	    myVp[pntCnt++] = 1.0 * k / ysteps; //y
	
	    //face 2, vertex 2
	    myVp[pntCnt++] = 1.0 * (j+1) / thetasteps; //x
	    myVp[pntCnt++] = 1.0 * (k+1) / ysteps; //y

	    //face 2, vertex 3
	    myVp[pntCnt++] = 1.0 * (j+1) / thetasteps; //x
	    myVp[pntCnt++] = 1.0 * k / ysteps; //y
        }
    }
    return myVp;
}

GLfloat* getWallVertices(float xpos, float ypos, float whl){
	GLfloat * myVp = new GLfloat[18*1];
	if(xpos < 0){
	    myVp[0]=xpos;myVp[1]=-whl;myVp[2]=whl;	//face 1, vertex 1
	    myVp[3]=xpos;myVp[4]=-whl;myVp[5]=-whl;	//face 1, vertex 2
	    myVp[6]=xpos;myVp[7]=whl;myVp[8]=whl;	//face 1, vertex 3
	    myVp[9]=xpos;myVp[10]=whl;myVp[11]=whl;	//face 2, vertex 1
	    myVp[12]=xpos;myVp[13]=-whl;myVp[14]=-whl;	//face 2, vertex 2
	    myVp[15]=xpos;myVp[16]=whl;myVp[17]=-whl;	//face 2, vertex 3
	}
	if (xpos > 0){
	    myVp[0]=xpos;myVp[1]=-whl;myVp[2]=-whl;	//face 1, vertex 1
	    myVp[3]=xpos;myVp[4]=-whl;myVp[5]=whl;	//face 1, vertex 2
	    myVp[6]=xpos;myVp[7]=whl;myVp[8]=-whl;	//face 1, vertex 3
	    myVp[9]=xpos;myVp[10]=whl;myVp[11]=-whl;	//face 2, vertex 1
	    myVp[12]=xpos;myVp[13]=-whl;myVp[14]=whl;	//face 2, vertex 2
	    myVp[15]=xpos;myVp[16]=whl;myVp[17]=whl;	//face 2, vertex 3
	}
	if (ypos < 0){
	    myVp[0]=-whl;myVp[1]=ypos;myVp[2]=whl;	//face 1, vertex 1
	    myVp[3]=whl;myVp[4]=ypos;myVp[5]=whl;	//face 1, vertex 2
	    myVp[6]=-whl;myVp[7]=ypos;myVp[8]=-whl;	//face 1, vertex 3
	    myVp[9]=-whl;myVp[10]=ypos;myVp[11]=-whl;	//face 2, vertex 1
	    myVp[12]=whl;myVp[13]=ypos;myVp[14]=whl;	//face 2, vertex 2
	    myVp[15]=whl;myVp[16]=ypos;myVp[17]=-whl;	//face 2, vertex 3
	}
	if (ypos > 0){
	    myVp[0]=-whl;myVp[1]=ypos;myVp[2]=-whl;	//face 1, vertex 1
	    myVp[3]=whl;myVp[4]=ypos;myVp[5]=-whl;	//face 1, vertex 2
	    myVp[6]=-whl;myVp[7]=ypos;myVp[8]=whl;	//face 1, vertex 3
	    myVp[9]=-whl;myVp[10]=ypos;myVp[11]=whl;	//face 2, vertex 1
	    myVp[12]=whl;myVp[13]=ypos;myVp[14]=-whl;	//face 2, vertex 2
	    myVp[15]=whl;myVp[16]=ypos;myVp[17]=whl;	//face 2, vertex 3
	}
	return myVp;
}
GLfloat* getWallNormals(float xpos, float ypos, float whl){
	GLfloat * myVp = new GLfloat[18*1];
	if(xpos < 0){
	    myVp[0]=1;myVp[1]=0;myVp[2]=0;	//face 1, vertex 1
	    myVp[3]=1;myVp[4]=0;myVp[5]=0;	//face 1, vertex 2
	    myVp[6]=1;myVp[7]=0;myVp[8]=0;	//face 1, vertex 3
	    myVp[9]=1;myVp[10]=0;myVp[11]=0;	//face 2, vertex 1
	    myVp[12]=1;myVp[13]=0;myVp[14]=0;	//face 2, vertex 2
	    myVp[15]=1;myVp[16]=0;myVp[17]=0;	//face 2, vertex 3
	}
	if(xpos > 0){
	    myVp[0]=-1;myVp[1]=0;myVp[2]=0;	//face 1, vertex 1
	    myVp[3]=-1;myVp[4]=0;myVp[5]=0;	//face 1, vertex 2
	    myVp[6]=-1;myVp[7]=0;myVp[8]=0;	//face 1, vertex 3
	    myVp[9]=-1;myVp[10]=0;myVp[11]=0;	//face 2, vertex 1
	    myVp[12]=-1;myVp[13]=0;myVp[14]=0;	//face 2, vertex 2
	    myVp[15]=-1;myVp[16]=0;myVp[17]=0;	//face 2, vertex 3
	}
	if(ypos < 0){
	    myVp[0]=0;myVp[1]=1;myVp[2]=0;	//face 1, vertex 1
	    myVp[3]=0;myVp[4]=1;myVp[5]=0;	//face 1, vertex 2
	    myVp[6]=0;myVp[7]=1;myVp[8]=0;	//face 1, vertex 3
	    myVp[9]=0;myVp[10]=1;myVp[11]=0;	//face 2, vertex 1
	    myVp[12]=0;myVp[13]=1;myVp[14]=0;	//face 2, vertex 2
	    myVp[15]=0;myVp[16]=1;myVp[17]=0;	//face 2, vertex 3
	}
	if(ypos > 0){
	    myVp[0]=0;myVp[1]=-1;myVp[2]=0;	//face 1, vertex 1
	    myVp[3]=0;myVp[4]=-1;myVp[5]=0;	//face 1, vertex 2
	    myVp[6]=0;myVp[7]=-1;myVp[8]=0;	//face 1, vertex 3
	    myVp[9]=0;myVp[10]=-1;myVp[11]=0;	//face 2, vertex 1
	    myVp[12]=0;myVp[13]=-1;myVp[14]=0;	//face 2, vertex 2
	    myVp[15]=0;myVp[16]=-1;myVp[17]=0;	//face 2, vertex 3
	}
	return myVp;
}
GLfloat* getWallTexCoords(){
	GLfloat * myVp = new GLfloat[12*1];
	myVp[0]=0;myVp[1]=0;		//face 1, vertex 1
	myVp[2]=1;myVp[3]=0;		//face 1, vertex 2
        myVp[4]=0;myVp[5]=1;		//face 1, vertex 3
        myVp[6]=0;myVp[7]=1;		//face 2, vertex 1
        myVp[8]=1;myVp[9]=0;		//face 2, vertex 2
        myVp[10]=1;myVp[11]=1;		//face 2, vertex 3
	return myVp;
}


GLfloat* getfloorVertices(float rfhl, float rfhh, float radius){
	GLfloat * myVp = new GLfloat[18*1];
	myVp[0]=-rfhl;myVp[1]=-rfhh;myVp[2]=-radius;	//face 1, vertex 1
	myVp[3]=rfhl;myVp[4]=-rfhh;myVp[5]=-radius;	//face 1, vertex 2
	myVp[6]=-rfhl;myVp[7]=rfhh;myVp[8]=-radius;	//face 1, vertex 3
	myVp[9]=-rfhl;myVp[10]=rfhh;myVp[11]=-radius;	//face 2, vertex 1
	myVp[12]=rfhl;myVp[13]=-rfhh;myVp[14]=-radius;	//face 2, vertex 2
	myVp[15]=rfhl;myVp[16]=rfhh;myVp[17]=-radius;	//face 2, vertex 3
	return myVp;
}

GLfloat* getfloorNormals(float rfhl, float rfhh){
	GLfloat * myVp = new GLfloat[18*1];
	myVp[0]=0;myVp[1]=0;myVp[2]=1;	//face 1, vertex 1
	myVp[3]=0;myVp[4]=0;myVp[5]=1;	//face 1, vertex 2    
	myVp[6]=0;myVp[7]=0;myVp[8]=1;	//face 1, vertex 3
	myVp[9]=0;myVp[10]=0;myVp[11]=1;	//face 2, vertex 1
	myVp[12]=0;myVp[13]=0;myVp[14]=1;	//face 2, vertex 2
	myVp[15]=0;myVp[16]=0;myVp[17]=1;	//face 2, vertex 3
	return myVp;	
}
GLfloat* getfloorTexCoords(){
	GLfloat * myVp = new GLfloat[12*1];
	myVp[0]=0;myVp[1]=0;		//face 1, vertex 1
	myVp[2]=1;myVp[3]=0;		//face 1, vertex 2
        myVp[4]=0;myVp[5]=1;		//face 1, vertex 3
        myVp[6]=0;myVp[7]=1;		//face 2, vertex 1
        myVp[8]=1;myVp[9]=0;		//face 2, vertex 2
        myVp[10]=1;myVp[11]=1;		//face 2, vertex 3
	return myVp;
}
