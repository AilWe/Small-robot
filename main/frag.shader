#version 410


// Define INPUTS from fragment shader
uniform mat4 view_mat;
uniform int ball_pass_wall_no;
uniform int draw_mode;

// And from the uniform outputs for the textures setup in main.cpp.
uniform sampler2D texture00;
uniform sampler2D texture01;
uniform sampler2D texture02;
uniform sampler2D texture03;
uniform sampler2D texture04;
uniform sampler2D texture05;
uniform sampler2D texture06;

in vec3 ex_position, ex_normal;
in vec2 ex_texture;

out vec4 fragment_color; //RGBA color

void main () {
vec3 P, N, L, V, H;
vec4 tcolor;
vec3 eye_position, light_position;
vec4 diffuse_color, specular_color, ambient_color;
float shininess;
eye_position = vec3(0.0f, 0.0f, 5.0f);
light_position = vec3(0.0f, 0.0f, 5.0f);

if (draw_mode == 0){
    diffuse_color = vec4(0.1,0.9,0.0,1.0); 
    specular_color = vec4(0.9,0.5,0.6,1.0);
    ambient_color = vec4(0.4,0.4,0.4,1.0); 
    shininess = 2.0;
    //light_position = vec3(ex_position.xy, 5.0f);
    if (ball_pass_wall_no == 0) tcolor = texture2D(texture00, ex_texture);
    if (ball_pass_wall_no == 1) tcolor = texture2D(texture01, ex_texture);
    if (ball_pass_wall_no == 2) tcolor = texture2D(texture02, ex_texture);
    if (ball_pass_wall_no == 3) tcolor = texture2D(texture03, ex_texture);
    if (ball_pass_wall_no == 4) tcolor = texture2D(texture04, ex_texture);
}
else if(draw_mode == 1){
    tcolor = texture2D(texture01, ex_texture);
    diffuse_color = vec4(0.7,0.3,0.2,1.0); 
    specular_color = vec4(0.3,0.7,0.0,1.0);
    ambient_color = vec4(0.4,0.4,0.4,1.0); 
    shininess = 2.0;
    if (ball_pass_wall_no == 1){     
	specular_color = vec4(0.95,0.95,0.95,1.0);
	light_position = vec3(0.0f, 0.0f, 0.0f);
    }
}
else if(draw_mode == 2){
    tcolor = texture2D(texture02, ex_texture);
    diffuse_color = vec4(0.5,0.2,0.8,1.0); 
    specular_color = vec4(0.3,0.6,0.1,1.0);
    ambient_color = vec4(0.4,0.4,0.4,1.0); 
    shininess = 2.0;
    if (ball_pass_wall_no == 2){     
	specular_color = vec4(0.95,0.95,0.95,1.0);
	light_position = vec3(0.0f, 0.0f, 0.0f);
    }
}
else if(draw_mode == 3){
    tcolor = texture2D(texture03, ex_texture);
    diffuse_color = vec4(0.7,0.7,0.8,1.0); 
    specular_color = vec4(0.0,0.2,0.9,1.0);
    ambient_color = vec4(0.4,0.4,0.4,1.0); 
    shininess = 2.0;
    if (ball_pass_wall_no == 3){     
	specular_color = vec4(0.95,0.95,0.95,1.0);
	light_position = vec3(0.0f, 0.0f, 0.0f);
    }
}
else if(draw_mode == 4){
    tcolor = texture2D(texture04, ex_texture);
    diffuse_color = vec4(0.5,0.2,0.8,1.0); 
    specular_color = vec4(0.7,0.7,0.7,1.0);
    ambient_color = vec4(0.4,0.4,0.4,1.0); 
    shininess = 2.0;
    if (ball_pass_wall_no == 4){     
	specular_color = vec4(0.95,0.95,0.95,1.0);
	light_position = vec3(0.0f, 0.0f, 0.0f);
    }
}
else if(draw_mode == 5){
    tcolor = texture2D(texture05, ex_texture);
    diffuse_color = vec4(0.2,0.2,0.3,1.0); 
    specular_color = vec4(0.7,0.4,0.9,1.0);
    ambient_color = vec4(0.1,0.1,0.1,1.0); 
    shininess = 2.0;
}
else{
    tcolor = texture2D(texture06, ex_texture);
    diffuse_color = vec4(0.1,0.9,0.0,1.0); 
    specular_color = vec4(0.9,0.5,0.6,1.0);
    ambient_color = vec4(0.4,0.4,0.4,1.0); 
    shininess = 2.0;
}

P = ex_position;
N = normalize(ex_normal);
L = normalize(light_position - P);
V = normalize(eye_position - P);
H = normalize(L+V);

diffuse_color = 0.1*diffuse_color + 0.9*tcolor;
diffuse_color *= max(dot(N,L),0.0);
specular_color *= pow(max(dot(H,N),0.0),shininess);
ambient_color *= diffuse_color;
fragment_color = ambient_color + diffuse_color + specular_color;


// See the normals:
//fragment_color = vec4(N,1.0); 
//fragment_color = vec4(1.0,0.5,0.0,1.0);
}
