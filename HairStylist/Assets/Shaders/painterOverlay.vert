#version 330
precision mediump float;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv; 
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

void main()
{
    gl_Position = vec4(2.0 * in_uv.x - 1.0, 2.0 * in_uv.y - 1.0, 0.0, 1.0);
}
