#version 300 es
precision mediump float;

in vec2 t_position;

out vec4 f_color;

uniform sampler2D ground_texture;

void main(){
	f_color = texture(ground_texture, t_position);
}
