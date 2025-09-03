#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_Normal;
out vec3 v_FragPos;
out vec2 v_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_MVP * vec4(position, 1.0);
    v_FragPos = vec3(u_Model * vec4(position, 1.0));
    v_TexCoord = texCoord;
    
    // Transform normal to world space (assuming uniform scaling)
    v_Normal = normalize(mat3(u_Model) * normal);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;

uniform vec3 u_Color;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform bool u_UseTexture;
uniform sampler2D u_Texture;

void main()
{
    // Basic Phong lighting
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Choose between texture and solid color
    vec3 baseColor;
    if (u_UseTexture) {
        vec4 texColor = texture(u_Texture, v_TexCoord);
        baseColor = texColor.rgb;
    } else {
        baseColor = u_Color;
    }
    
    vec3 result = (ambient + diffuse + specular) * baseColor;
    color = vec4(result, 1.0);
}