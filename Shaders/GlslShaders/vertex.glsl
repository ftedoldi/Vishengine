#version 460 core
#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

struct InstanceData {
    vec4 ViewPositionAndScale;
    vec4 ViewRotation;
};

layout(std430, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
};
  
out vec2 TexCoord;
out vec3 FragViewPosition;
out vec3 FragPos;
out vec3 NormalViewPosition;

uniform mat4 Perspective;

vec3 RotateVectorByQuaternion(vec4 quaternion, vec3 vector);

void main() {
    TexCoord = aTexCoord;

    InstanceData instanceData = instances[gl_InstanceID + gl_BaseInstanceARB];

    NormalViewPosition = RotateVectorByQuaternion(instanceData.ViewRotation, aNormal).xyz;
    const float scale = instanceData.ViewPositionAndScale.w;
    const vec3 viewPosition = instanceData.ViewPositionAndScale.xyz;
    vec3 scaledPosition = vec3(aPos.x * scale, aPos.y * scale, aPos.z * scale);
    FragViewPosition = RotateVectorByQuaternion(instanceData.ViewRotation, scaledPosition).xyz + viewPosition;

    gl_Position = Perspective * vec4(FragViewPosition, 1.0);
}

vec4 QuatMultiply(vec4 first, vec4 second) {
    const float q1x = first.x;
    const float q1y = first.y;
    const float q1z = first.z;
    const float q1w = first.w;

    const float q2x = second.x;
    const float q2y = second.y;
    const float q2z = second.z;
    const float q2w = second.w;

    return vec4(q1w * q2x + q1x * q2w + q1y * q2z - q1z * q2y,
                q1w * q2y + q1y * q2w + q1z * q2x - q1x * q2z,
                q1w * q2z + q1z * q2w + q1x * q2y - q1y * q2x,
                q1w * q2w - q1x * q2x - q1y * q2y - q1z * q2z);
}

vec3 RotateVectorByQuaternion(vec4 quaternion, vec3 vector) {
    vec4 quatConj = vec4(-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w);
    vec4 quatPoint = vec4(vector, 0.0);

    vec4 partialQuatMultiply = QuatMultiply(quaternion, quatPoint);
    vec4 result = QuatMultiply(partialQuatMultiply, quatConj);

    return result.xyz;
}