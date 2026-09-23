#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D texture1;

void main(){
    vec3 lightColor=vec3(1);

    float ambientStrength=0.2;
    vec3 ambient=ambientStrength*lightColor;

    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(lightPos-FragPos);
    float diff=max(dot(norm,lightDir),0);
    vec3 diffuse=diff*lightColor;

    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0),32);
    vec3 specular=0.5*spec*lightColor;

    vec4 texColor = texture(texture1, TexCoord);
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting * texColor.rgb, texColor.a);
};