#version 450 core

out vec4 color;

uniform vec3 lightIntensities = vec3(0.7);
uniform vec3 ambient = vec3(0.7);

uniform sampler2D sampler;
uniform sampler1D pallete;

layout (location=3) uniform mat4 pMat;
layout (location=4) uniform mat4 vMat;
layout (location=5) uniform mat4 mMat;

layout (location=6) uniform vec3 camPos;

in VS_OUT
{   
    vec3 position;
    vec3 normal;
    vec2 uvCoords;
} fs_in;

void main(void)
{
	float t = texture(sampler, fs_in.uvCoords).r;
	vec4 col;
	if(t < 0.5)
		col = mix(vec4(0.4, 0.0, 1.0, 1.0), vec4(0.65, 0.65, 0.45, 1.0), 2 * t);
	else
		col = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), 2 * t - 1);
//    vec4 col = texture(pallete, t < 0.5 ? t : 1 - t);

	//Phong shading
    vec3 l = normalize(mat3(vMat*mMat)*(camPos-fs_in.position));
    float brightness = clamp(abs(dot(mat3(vMat*mMat)*fs_in.normal, l)), 0.0, 1.0);
    color = vec4((ambient + brightness * lightIntensities) * col.rgb, col.a); 
}
