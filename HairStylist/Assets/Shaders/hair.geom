#version 330
precision mediump float;

uniform float u_hairLength;
uniform vec3 u_hairColor;
uniform mat4 u_proj;

// In view space
in vec3 v_viewPosition[];
in vec3 v_viewNormal[];
in vec3 v_viewTangent[];
in vec3 v_viewBitangent[];

// r = hairLength, g = hair curl (tangent), b = hair twist (bitangent)
in vec3 v_hairParams[];

const float PI = 3.14159265359;
const vec3 bary[7] = vec3[](vec3(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0),
                            vec3(4.0 / 6.0, 1.0 / 6.0, 1.0 / 6.0),
                            vec3(1.0 / 6.0, 4.0 / 6.0, 1.0 / 6.0),
                            vec3(1.0 / 6.0, 1.0 / 6.0, 4.0 / 6.0),
                            vec3(5.0 / 12.0, 5.0 / 12.0, 1.0 / 6.0),
                            vec3(5.0 / 12.0, 1.0 / 6.0, 5.0 / 12.0),
                            vec3(1.0 / 6.0, 5.0 / 12.0, 5.0 / 12.0));
const int numSegments = 5;

out vec3 v_direction;
out vec3 v_viewPos;
out vec3 v_color;

layout(triangles) in;
layout(line_strip, max_vertices = 42) out;
void main()
{
    float averageHairLen = (v_hairParams[0].r + v_hairParams[1].r + v_hairParams[2].r) / 3.0;
    
    if (averageHairLen > 0.01)
    {
        // Generate 7 hair
        for (int i = 0; i < bary.length(); ++i)
        {
            vec3 viewP = v_viewPosition[0] * bary[i].x +
                         v_viewPosition[1] * bary[i].y +
                         v_viewPosition[2] * bary[i].z;
            
            vec3 viewN = normalize(v_viewNormal[0] * bary[i].x +
                                   v_viewNormal[1] * bary[i].y +
                                   v_viewNormal[2] * bary[i].z);
                        
            vec3 viewT = normalize(v_viewTangent[0] * bary[i].x +          
                                   v_viewTangent[1] * bary[i].y +
                                   v_viewTangent[2] * bary[i].z);
            
            vec3 viewB = normalize(v_viewBitangent[0] * bary[i].x +
                                   v_viewBitangent[1] * bary[i].y +
                                   v_viewBitangent[2] * bary[i].z);

            vec3 hairParams = v_hairParams[0] * bary[i].x +
                              v_hairParams[1] * bary[i].y +
                              v_hairParams[2] * bary[i].z;

            float hairLength = hairParams.r * u_hairLength / numSegments;
            float curl  = (hairParams.g * PI * 2.0 - PI) / numSegments;
            float twist = (hairParams.b * PI * 2.0 - PI) / numSegments;
            
            float cc = cos(curl);
            float cs = sin(curl);
            mat3 curlM = mat3(cc,  0.0, -cs,
                              0.0, 1.0, 0.0,
                              cs,  0.0, cc);
            
            float tc = cos(twist);
            float ts = sin(twist);
            mat3 twistM = mat3(1.0, 0.0, 0.0,
                               0.0, tc,  -ts,
                               0.0, ts,  tc);                 
            mat3 rot = twistM * curlM;
            
            // Matrix from tangent space back to the previous space (view space in this case)
            mat3 MTS = mat3(viewT, viewB, viewN);

            vec3 direction = vec3(0.0, 0.0, 1.0);
            vec3 pos = vec3(0.0, 0.0, 0.0);

            for (int j = 0; j <= numSegments; ++j)
            {
                v_direction = MTS * direction;
                v_viewPos = viewP + pos;    
                gl_Position = u_proj * vec4(v_viewPos, 1.0);    
                v_color = u_hairColor * (j / float(numSegments));
                EmitVertex();

                // Prepare next vertex
                direction = rot * direction;
                pos += hairLength * MTS * direction;
            }

            EndPrimitive();
        }
    }
}
