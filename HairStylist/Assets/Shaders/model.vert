#version 330
precision mediump float;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv; 
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 v_texCoords;

out vec3 v_viewPosition;
out vec3 v_viewNormal;
out vec3 v_viewTangent;
out vec3 v_viewBitangent;

void main()
{
    mat4 MV = u_view * u_model;
    v_viewNormal    = (MV * vec4(in_normal, 0.0)).xyz;
    v_viewTangent   = (MV * vec4(in_tangent, 0.0)).xyz;
    v_viewBitangent = (MV * vec4(in_bitangent, 0.0)).xyz;
    v_viewPosition  = (MV * vec4(in_pos, 1.0)).xyz;
    
    gl_Position = u_proj * MV * vec4(in_pos, 1.0);
    v_texCoords = in_uv;
}
