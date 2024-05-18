#include "MathUtils.h"

namespace MathUtils{
    glm::quat EulerAnglesToQuaternion(const glm::vec3 eulerAngles){
        float yaw{eulerAngles.x};
        float pitch{eulerAngles.y};
        float roll{eulerAngles.z};

        yaw = glm::radians(yaw);
        pitch = glm::radians(pitch);
        roll = glm::radians(roll);

        float rollOver2{roll * 0.5f};
        float sinRollOver2{std::sinf(rollOver2)};
        float cosRollOver2{std::cosf(rollOver2)};
        float pitchOver2{pitch * 0.5f};
        float sinPitchOver2{std::sinf(pitchOver2)};
        float cosPitchOver2{std::cosf(pitchOver2)};
        float yawOver2{yaw * 0.5f};
        float sinYawOver2{std::sinf(yawOver2)};
        float cosYawOver2{std::cosf(yawOver2)};
        glm::quat result;

        result.w = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;

        result.x = sinYawOver2 * cosPitchOver2 * cosRollOver2 + cosYawOver2 * sinPitchOver2 * sinRollOver2;
        result.y = cosYawOver2 * sinPitchOver2 * cosRollOver2 - sinYawOver2* cosPitchOver2 * sinRollOver2;
        result.z = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;

        return result;
    }
}