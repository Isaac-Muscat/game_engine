#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPosition;
layout(location = 4) flat in uint fragMatID;
layout(location = 5) in vec3 cameraPosition;

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
#define PI 3.14159265359

layout(set = 0, binding = 1) readonly buffer LightBuffer {
    Light lights[];
} light_buffer;

layout(set = 1, binding = 0) uniform sampler2D texSampler;


struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;
};

#define MAX_MATERIALS 10
layout(set = 1, binding = 1) uniform MaterialBufferObject {
    Material materials[MAX_MATERIALS];
} material_buffer;


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 lerp(vec3 a, vec3 b, float t) {
    return b * t + a * (1 - t);
}

void main() {
    Material material = material_buffer.materials[fragMatID];
    vec3 albedo = material.albedo * texture(texSampler, fragTexCoord).rgb;
    float roughness = material.roughness;
    float metallic = material.metallic;
    float ao = material.ao;

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPosition - fragPosition);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0);
    for (int i = 0; i < light_buffer.lights.length(); i++) {
        Light light = light_buffer.lights[i];
        switch(light.type) {
            case Directional: {
                /*
                float diffuse = light.diffuse * max(dot(normalize(light.direction), normalize(fragNormal)), 0);
                vec3 specular = vec3(0); // TODO: Added specular calculation with view dir
                lighting += light.color * (ambient + diffuse + specular);
                break;
                */
                // calculate per-light radiance
                vec3 L = normalize(light.direction);
                vec3 H = normalize(V + L);
                vec3 radiance     = light.color;
                
                // cook-torrance brdf
                float NDF = DistributionGGX(N, H, roughness);        
                float G   = GeometrySmith(N, V, L, roughness);      
                vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
                
                vec3 kS = F;
                vec3 kD = vec3(1.0) - kS;
                kD *= 1.0 - metallic;	  
                
                vec3 numerator    = NDF * G * F;
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
                vec3 specular     = numerator / denominator;  
                    
                // add to outgoing radiance Lo
                float NdotL = max(dot(N, L), 0.0);                
                Lo += (kD * albedo / PI + specular) * radiance * NdotL;
                break;
            }
            case Point: {
                // calculate per-light radiance
                vec3 L = normalize(light.position - fragPosition);
                vec3 H = normalize(V + L);
                float distance    = length(light.position - fragPosition);
                float attenuation = 1.0 / (0.01f * distance * distance);
                vec3 radiance     = light.color * attenuation;        
                
                // cook-torrance brdf
                float NDF = DistributionGGX(N, H, roughness);        
                float G   = GeometrySmith(N, V, L, roughness);      
                vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
                
                vec3 kS = F;
                vec3 kD = vec3(1.0) - kS;
                kD *= 1.0 - metallic;	  
                
                vec3 numerator    = NDF * G * F;
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
                vec3 specular     = numerator / denominator;  
                    
                // add to outgoing radiance Lo
                float NdotL = max(dot(N, L), 0.0);                
                Lo += (kD * albedo / PI + specular) * radiance * NdotL;
                break;
            }
            case Spot:
            case Area:
            default:
                break;
        }
    }
    vec3 fog_col = vec3(1);
    float len = length(cameraPosition - fragPosition);
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = lerp(ambient + Lo, fog_col, clamp(exp(len/50.0f)/10.0f - 1, 0, 1));
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    outColor = vec4(color, 1.0);
    //outColor = vec4(fragColor, 1.0f) * texture(texSampler, fragTexCoord).rgba * vec4(lighting, 1.0f);
    //outColor = vec4(fragNormal, 1); // Debug normals
    //outColor = vec4(vec3(dot(normalize(direction), normalize(fragNormal))), 1); // Debug Lights
}
