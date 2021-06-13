#version 430 core

const int MAX_JOINTS = 64;
const int MAX_WEIGHTS = 4;

uniform mat4 u_ProjViewModel;
uniform mat3 u_NormalMatrix;
uniform mat4 u_JointMatrix[MAX_JOINTS];

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in ivec4 a_Joints;
layout(location = 4) in vec4 a_Weights;

out vec3 v_Normal;
out vec2 v_TexCoords;

void main()
{
    mat4 skin_matrix
        = (a_Weights.x * u_JointMatrix[a_Joints.x]
           + a_Weights.y * u_JointMatrix[a_Joints.y]
           + a_Weights.z * u_JointMatrix[a_Joints.z]
           + a_Weights.w * u_JointMatrix[a_Joints.w]);

    vec4 skinned_pos = skin_matrix * vec4(a_Position, 1.0);
    vec3 skinned_normal = mat3(skin_matrix) * a_Normal;

    v_Normal = u_NormalMatrix * skinned_normal;
    v_TexCoords = a_TexCoords;
    gl_Position = u_ProjViewModel * skinned_pos;
}
