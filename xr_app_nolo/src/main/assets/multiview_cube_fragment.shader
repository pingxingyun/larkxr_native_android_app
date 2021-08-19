#version 300 es
precision highp float;
in vec4 v_Color;
out vec4 f_color;
void main()
{
f_color.rgb = vec3(0.0);
f_color.r = v_Color.x;
f_color.g = v_Color.y;
f_color.b = v_Color.z;
f_color.a = 1.0;
}