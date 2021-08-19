#version 300 es
#extension GL_OVR_multiview : enable
layout(num_views = 2) in;
in vec3 vertexPosition;
uniform mat4 modelViewProjection[2];
uniform mat4 model;
in vec4 a_Color;
out vec4 v_Color;
void main()
{
gl_Position = modelViewProjection[gl_ViewID_OVR] * vec4(vertexPosition, 1.0);
v_Color = a_Color * 0.5;
}