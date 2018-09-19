#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1) uniform samplerCube cubeSampler;

layout(location = 0) in vec3 fragTexCoord;
layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = fragColor;
	//outColor = vec4(fragTexCoord, 0.0, 1.0);
	outColor = vec4(/*fragColor **/ texture(cubeSampler, fragTexCoord).rgb, 1.0);
	//outColor = texture(texSampler, fragTexCoord);
}