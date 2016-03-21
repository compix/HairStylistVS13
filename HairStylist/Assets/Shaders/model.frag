#version 330
precision mediump float;

struct BlinnPhongOut
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 viewDirection; // light direction in view space
};

struct Material
{
    vec3 diffuse;
    vec4 specular; // w = specular power
};

in vec2 v_texCoords;
in vec3 v_viewPosition;
in vec3 v_viewNormal;
in vec3 v_viewTangent;
in vec3 v_viewBitangent;

uniform sampler2D u_textureDiffuse;
uniform sampler2D u_hairTexture;
uniform DirectionalLight u_dirLight;
uniform Material u_material;

layout(location = 0) out vec4 out_color;

BlinnPhongOut blinnPhongShading(DirectionalLight dirLight,
                                vec3 normal,
                                vec3 viewVec)
{
    vec3 lightVec = -dirLight.viewDirection;
    vec3 halfway = normalize(viewVec + lightVec);
    float specFactor = pow(max(dot(halfway, normal), 0.0f), u_material.specular.w);
    float diffuseFactor = dot(lightVec, normal);
    
    BlinnPhongOut result;
    result.ambient = dirLight.ambient;
    result.diffuse = diffuseFactor * dirLight.diffuse * u_material.diffuse;
    result.specular = specFactor * dirLight.specular * u_material.specular.xyz;
    
    return result;
}

void main() 
{
    BlinnPhongOut result = blinnPhongShading(u_dirLight, normalize(v_viewNormal), -normalize(v_viewPosition));
    
	out_color = vec4(result.ambient, 0.0) + 
                     texture2D(u_textureDiffuse, v_texCoords) * vec4(result.diffuse, 1.0) + 
                     vec4(result.specular, 0.0);

    // Make the area where hair grows dark
    vec4 hairSample = texture2D(u_hairTexture, v_texCoords);
    out_color.rgb *= clamp(1.0 - hairSample.r, 0.0, 1.0);
}
