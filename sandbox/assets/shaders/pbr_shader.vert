#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragPosition;
layout(location = 4) out vec3 cameraPosition;


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConsts {
	mat4 model;
} pushConsts;

void main() {
    gl_Position = ubo.proj * ubo.view * pushConsts.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = mat3(transpose(inverse(pushConsts.model))) * inNormal; // TODO: Send normal matrix to push consts
    fragPosition = (pushConsts.model * vec4(inPosition, 1.0)).xyz;
    mat4 inview = inverse(ubo.view);
    cameraPosition = vec3(inview[3][0], inview[3][1], inview[3][2]); // IS THIS CORRECT?
}
