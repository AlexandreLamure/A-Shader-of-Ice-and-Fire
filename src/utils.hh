#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Utils
{
    std::vector<glm::vec3> lava_skeleton_light = {
            {  5, 17, -20},
            {-10, 10, -19},
            {-21, 10, -15},
            {  7, 20,  -7},
            {  0, 10,  -6},
            {-15, 10,  -1},
            { 16, 20,   4},
            { 16,  8,   8},
            { 16,  8,  22}};
    std::vector<glm::vec3> lava_nexus = {{ 22, 20, -20}};

    std::vector<glm::vec3> water_skeleton = {
            {28, 7, 28},
            {-1, 7, 30},
            {-27, 7, 4},
            {-33, 7, -27}
    };
}