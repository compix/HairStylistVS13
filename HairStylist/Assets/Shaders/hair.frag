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

in vec3 v_direction;
in vec3 v_viewPos;
in vec3 v_color;

uniform DirectionalLight u_dirLight;
uniform Material u_material;

BlinnPhongOut blinnPhongShading(DirectionalLight dirLight,
                                vec3 viewVec)
{
    vec3 d = normalize(v_direction);
    vec3 lightVec = -dirLight.viewDirection;
    vec3 halfway = normalize(viewVec + lightVec);
    float dh = dot(d, halfway);
    float specFactor = pow(sqrt(1.0 - dh*dh), u_material.specular.w);
    float dl = dot(d, lightVec);
    float diffuseFactor = sqrt(1.0 - dl*dl);
    
    BlinnPhongOut result;
    result.ambient = dirLight.ambient;
    result.diffuse = diffuseFactor * dirLight.diffuse * u_material.diffuse;
    result.specular = specFactor * dirLight.specular * u_material.specular.xyz;
    
    return result;
}

layout(location = 0) out vec4 out_color;
void main() 
{
    BlinnPhongOut result = blinnPhongShading(u_dirLight, -normalize(v_viewPos));

	out_color = vec4(result.ambient, 0.0) + 
                vec4(v_color, 1.0) * vec4(result.diffuse, 1.0) + 
                vec4(result.specular, 0.0);
}
