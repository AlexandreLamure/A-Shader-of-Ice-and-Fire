#include "camera.hh"

Camera::Camera()
{
    pos = glm::vec3(0.0f, 0.0f,  25.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::vec3(1.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f,  0.0f);
    fov = 45.f;

    speed = 15.f;
    sensitivity = 0.03f;

    yaw = -90.f;
    pitch = 0.f;

    first_mouse_move = true;
}

void Camera::invert_pitch()
{
    pitch = -pitch;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, glm::vec3(0.f, 1.f, 0.f)));
    up    = glm::normalize(glm::cross(right, front));
}
