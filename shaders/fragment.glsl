#version 330 core

in vec2 st;             // Interpolated texture coords, sent from vertex shader
in vec3 interpolatedNormal;
out vec4 finalcolor;

uniform sampler2D tex;  // A uniform variable to identify the texture
uniform mat4 T;

void main() {
	vec3 L = normalize(mat3(T) * vec3(0.0f, 0.1f, 1.0f));
	vec3 V = normalize(vec3(-0.4f,-0.4f,-1.0f));
	vec3 N = interpolatedNormal;

	vec3 colorRGB = vec3(texture(tex,st));
	vec3 colorGreyscale = vec3(1.0f, 1.0f, 1.0f);

	vec3 ka = 0.9f * colorRGB;
	vec3 Ia = 0.5f * colorGreyscale;

	vec3 ks = 0.1f * colorGreyscale;
	vec3 Is = 0.9f * colorGreyscale;

	vec3 kd = 1.0f * colorRGB;
	vec3 Id = 0.8f * colorGreyscale;

	float n = 100.0f;

	vec3 R = 2.0 * dot(N, L) * N - L;   // Could also have used the function reflect()
	float dotNL = max(dot(N, L), 0.0);  // If negative, set to zero
	float dotRV = max(dot(R, V), 0.0);
	if (dotNL == 0.0) {
		dotRV = 0.0;  // Do not show highlight on the dark side
	}
	vec3 shadedcolor = Ia * ka + Id * kd * dotNL + Is * ks * pow(dotRV, n);

	finalcolor = vec4(shadedcolor, 1.0)*texture(tex, st);  // Use the texture to set the surface color
}
