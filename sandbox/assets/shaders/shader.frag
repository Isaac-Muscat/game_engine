#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

struct Light {
    int type;
    float ambient, diffuse, specular;
    vec3 position;
    vec3 direction;
    vec3 color;
};

#define Directional 0
#define Point 1
#define Spot 2
#define Area 3

layout(set = 1, binding = 0) uniform LightBuffer {
    Light lights[1];
} light_buffer;

void main() {
    vec3 lighting = vec3(0);
    for (int i = 0; i < 1; i++) {
        Light light = light_buffer.lights[i];
        switch(light.type) {
            case Directional:
                float ambient = light.ambient;
                float diffuse = light.diffuse * max(dot(normalize(light.direction), normalize(fragNormal)), 0);
                vec3 specular = vec3(0); // TODO: Added specular calculation with view dir
                lighting += light.color * (ambient + diffuse + specular);
                break;
            case Point:
            case Spot:
            case Area:
            default:
                break;
        }
    }
    outColor = texture(texSampler, fragTexCoord).rgba * vec4(lighting, 1.0f);
    //outColor = vec4(fragNormal, 1); // Debug normals
}
