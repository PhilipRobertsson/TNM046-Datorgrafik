#version 330 core

layout(location = 0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 TexCoord;

out vec3 interpolatedNormal;
out vec2 st;
uniform mat4 MV;
uniform mat4 P;

void main() {
vec3 transformedNormal = vec3(MV) * Normal;
gl_Position = P*MV*vec4(Position, 1.0); // Special requiered output
interpolatedNormal = normalize(transformedNormal); // Will be interpolated across the triangle
st = TexCoord; // Will also be interpolated across the triangle
}