#ifndef CUBEDATA_H_
#define CUBEDATA_H_

#include <array>

#include "glm/glm.hpp"

// From:
// https://github.com/LunarG/VulkanSamples/blob/master/API-Samples/data/cube_data.h
constexpr std::array<glm::fvec3, 36> kCubePosition = {
    // left face
    glm::fvec3(-1, -1, -1), // lft-top-front
    glm::fvec3(-1, 1, 1),   // lft-btm-back
    glm::fvec3(-1, -1, 1),  // lft-top-back
    glm::fvec3(-1, 1, 1),   // lft-btm-back
    glm::fvec3(-1, -1, -1), // lft-top-front
    glm::fvec3(-1, 1, -1),  // lft-btm-front

    // front face
    glm::fvec3(-1, -1, -1), // lft-top-front
    glm::fvec3(1, -1, -1),  // rgt-top-front
    glm::fvec3(1, 1, -1),   // rgt-btm-front
    glm::fvec3(-1, -1, -1), // lft-top-front
    glm::fvec3(1, 1, -1),   // rgt-btm-front
    glm::fvec3(-1, 1, -1),  // lft-btm-front

    // top face
    glm::fvec3(-1, -1, -1), // lft-top-front
    glm::fvec3(1, -1, 1),   // rgt-top-back
    glm::fvec3(1, -1, -1),  // rgt-top-front
    glm::fvec3(-1, -1, -1), // lft-top-front
    glm::fvec3(-1, -1, 1),  // lft-top-back
    glm::fvec3(1, -1, 1),   // rgt-top-back

    // bottom face
    glm::fvec3(-1, 1, -1), // lft-btm-front
    glm::fvec3(1, 1, 1),   // rgt-btm-back
    glm::fvec3(-1, 1, 1),  // lft-btm-back
    glm::fvec3(-1, 1, -1), // lft-btm-front
    glm::fvec3(1, 1, -1),  // rgt-btm-front
    glm::fvec3(1, 1, 1),   // rgt-btm-back

    // right face
    glm::fvec3(1, 1, -1),  // rgt-btm-front
    glm::fvec3(1, -1, 1),  // rgt-top-back
    glm::fvec3(1, 1, 1),   // rgt-btm-back
    glm::fvec3(1, -1, 1),  // rgt-top-back
    glm::fvec3(1, 1, -1),  // rgt-btm-front
    glm::fvec3(1, -1, -1), // rgt-top-front

    // back face
    glm::fvec3(-1, 1, 1),  // lft-btm-back
    glm::fvec3(1, 1, 1),   // rgt-btm-back
    glm::fvec3(-1, -1, 1), // lft-top-back
    glm::fvec3(-1, -1, 1), // lft-top-back
    glm::fvec3(1, 1, 1),   // rgt-btm-back
    glm::fvec3(1, -1, 1),  // rgt-top-back
};

constexpr std::array<glm::fvec2, 36> kCubeTexCoord = {
    // left face
    glm::fvec2(1.f, 0.f), // lft-top-front
    glm::fvec2(0.f, 1.f), // lft-btm-back
    glm::fvec2(0.f, 0.f), // lft-top-back
    glm::fvec2(0.f, 1.f), // lft-btm-back
    glm::fvec2(1.f, 0.f), // lft-top-front
    glm::fvec2(1.f, 1.f), // lft-btm-front

    // front face
    glm::fvec2(0.f, 0.f), // lft-top-front
    glm::fvec2(1.f, 0.f), // rgt-top-front
    glm::fvec2(1.f, 1.f), // rgt-btm-front
    glm::fvec2(0.f, 0.f), // lft-top-front
    glm::fvec2(1.f, 1.f), // rgt-btm-front
    glm::fvec2(0.f, 1.f), // lft-btm-front

    // top face
    glm::fvec2(0.f, 1.f), // lft-top-front
    glm::fvec2(1.f, 0.f), // rgt-top-back
    glm::fvec2(1.f, 1.f), // rgt-top-front
    glm::fvec2(0.f, 1.f), // lft-top-front
    glm::fvec2(0.f, 0.f), // lft-top-back
    glm::fvec2(1.f, 0.f), // rgt-top-back

    // bottom face
    glm::fvec2(0.f, 0.f), // lft-btm-front
    glm::fvec2(1.f, 1.f), // rgt-btm-back
    glm::fvec2(0.f, 1.f), // lft-btm-back
    glm::fvec2(0.f, 0.f), // lft-btm-front
    glm::fvec2(1.f, 0.f), // rgt-btm-front
    glm::fvec2(1.f, 1.f), // rgt-btm-back

    // right face
    glm::fvec2(0.f, 1.f), // rgt-btm-front
    glm::fvec2(1.f, 0.f), // rgt-top-back
    glm::fvec2(1.f, 1.f), // rgt-btm-back
    glm::fvec2(1.f, 0.f), // rgt-top-back
    glm::fvec2(0.f, 1.f), // rgt-btm-front
    glm::fvec2(0.f, 0.f), // rgt-top-front

    // back face
    glm::fvec2(1.f, 1.f), // lft-btm-back
    glm::fvec2(0.f, 1.f), // rgt-btm-back
    glm::fvec2(1.f, 0.f), // lft-top-back
    glm::fvec2(1.f, 0.f), // lft-top-back
    glm::fvec2(0.f, 1.f), // rgt-btm-back
    glm::fvec2(0.f, 0.f), // rgt-top-back
};

#endif // CUBEDATA_H_
