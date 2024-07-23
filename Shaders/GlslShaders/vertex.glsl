#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
  
out vec2 TexCoord;

uniform vec3 Translation;
uniform vec4 Rotation;

uniform mat4 Perspective;

vec3 rotateVectorByQuaternion(vec4 rotationQuat, vec3 position);

void main() {
    TexCoord = aTexCoord;

    vec3 rotatedPoint = rotateVectorByQuaternion(Rotation, aPos).xyz;

    gl_Position = Perspective * vec4(rotatedPoint + Translation, 1.0);
}

vec4 quatMultiply(vec4 first, vec4 second) {
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

vec3 rotateVectorByQuaternion(vec4 rotationQuat, vec3 position) {
    vec4 quatConj = vec4(-rotationQuat.x, -rotationQuat.y, -rotationQuat.z, rotationQuat.w);
    vec4 quatPoint = vec4(position, 0.0);

    vec4 partialQuatMultiply = quatMultiply(rotationQuat, quatPoint);
    vec4 result = quatMultiply(partialQuatMultiply, quatConj);

    return result.xyz;
}