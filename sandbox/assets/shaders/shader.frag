#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPosition;
layout(location = 4) flat in uint fragMatID;


layout(location = 0) out vec4 outColor;

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

layout(set = 0, binding = 1) readonly buffer LightBuffer {
    Light lights[];
} light_buffer;

layout(set = 1, binding = 0) uniform sampler2D texSampler;
//layout(set = 1, binding = 1) uniform UniformBufferObject {
//    glm::vec4 albedo;
//    glm::vec4 roughness;
//    glm::vec4 specular;
//    etc...
//} material_constants;

void main() {
    vec3 lighting = vec3(0);
    for (int i = 0; i < light_buffer.lights.length(); i++) {
        Light light = light_buffer.lights[i];
        switch(light.type) {
            case Directional: {
                float ambient = light.ambient;
                float diffuse = light.diffuse * max(dot(normalize(light.direction), normalize(fragNormal)), 0);
                vec3 specular = vec3(0); // TODO: Added specular calculation with view dir
                lighting += light.color * (ambient + diffuse + specular);
                break;
            }
            case Point: {
                float ambient = light.ambient;
                vec3 direction = light.position - fragPosition;
                float diffuse = light.diffuse * max(dot(normalize(direction), normalize(fragNormal)), 0);
                vec3 specular = vec3(0); // TODO: Added specular calculation with view dir
                lighting += light.color * (ambient + diffuse + specular);
                break;
            }
            case Spot:
            case Area:
            default:
                break;
        }
    }
    outColor = vec4(fragColor, 1.0f) * texture(texSampler, fragTexCoord).rgba * vec4(lighting, 1.0f);
    //outColor = vec4(fragNormal, 1); // Debug normals
    //outColor = vec4(vec3(dot(normalize(direction), normalize(fragNormal))), 1); // Debug Lights
}
