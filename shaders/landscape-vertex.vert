#version 300 es
precision mediump float;

layout (location = 0) in vec2 v_position;

uniform mat4 projection;

void main(){
    gl_Position = projection * vec4(v_position, 0.0, 1.0);
}
