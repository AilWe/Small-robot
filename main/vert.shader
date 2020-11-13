#version 410

layout (location = 0) in vec3 vtxPosition;
layout (location = 1) in vec3 normalPosition;
layout (location = 2) in vec2 texturePosition;

uniform mat4 proj_mat, view_mat, ball_model_mat, wall_model_mat, floor_model_mat, head_model_mat;
uniform int draw_mode;

out vec3 ex_position, ex_normal;
out vec2 ex_texture;

void main () {
	mat4 model_mat;
	if (draw_mode == 0)
   	    model_mat = ball_model_mat;
	else if (draw_mode == 5)
	    model_mat = floor_model_mat;
	else if (draw_mode == 6)
	    model_mat = head_model_mat;
	else
   	    model_mat = wall_model_mat;
	gl_Position = vec4(vtxPosition, 1.0);
	gl_Position = proj_mat * view_mat * model_mat * gl_Position;
	ex_position = vtxPosition;
	ex_normal = normalPosition;
	ex_texture = texturePosition;
}
