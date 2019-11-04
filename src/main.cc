#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include <tuple>

#include "init.hh"
#include "program.hh"
#include "model.hh"
#include "camera.hh"
#include "fbo.hh"
#include "light.hh"
#include "cubemap.hh"


Camera camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouse_pos.x = xpos;
    camera.mouse_pos.y = ypos;

    if (camera.first_mouse_move)
    {
        camera.last_mouse_x = xpos;
        camera.last_mouse_y = ypos;
        camera.first_mouse_move = false;
    }

    float offset_x = xpos - camera.last_mouse_x;
    float offset_y = camera.last_mouse_y - ypos;

    camera.last_mouse_x = xpos;
    camera.last_mouse_y = ypos;

    offset_x *= camera.sensitivity;
    offset_y *= camera.sensitivity;

    camera.yaw   += offset_x;
    camera.pitch += offset_y;

    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    camera.front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front.y = sin(glm::radians(camera.pitch));
    camera.front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(camera.front);
    camera.right = glm::normalize(glm::cross(camera.front, glm::vec3(0.f, 1.f, 0.f)));
    camera.up    = glm::normalize(glm::cross(camera.right, camera.front));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.fov -= yoffset;

    if(camera.fov <= 1.0f)
        camera.fov = 1.0f;
    if(camera.fov >= 45.0f)
        camera.fov = 45.0f;
}


void process_input(GLFWwindow *window, float delta_time)
{
    float delta_speed = camera.speed * delta_time;

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos += delta_speed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos -= delta_speed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos += delta_speed * camera.right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos -= delta_speed * camera.right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.pos += delta_speed * camera.up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.pos -= delta_speed * camera.up;
}


void set_uniforms(Program& program, int window_w, int window_h, float total_time, float delta_time,
                  std::vector<DirLight>& dir_lights, std::vector<PointLight>& point_lights,
                  glm::vec4 clip_plane)
{
    // set uniforms
    program.set_float("total_time", total_time);
    program.set_float("delta_time", delta_time);
    program.set_vec2("resolution", window_w, window_h);
    // set random
    program.set_int("rand", std::rand() % 100);

    // set lights
    for (int i = 0; i < dir_lights.size(); ++i)
        dir_lights[i].set(program, i);
    for (int i = 0; i < point_lights.size(); ++i)
        point_lights[i].set(program, i);

    program.set_vec3("camera_pos", camera.pos);
    program.set_vec2("mouse_pos", camera.mouse_pos);

    glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
    program.set_mat4("view", view);

    float window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
    program.set_mat4("projection", projection);

    program.set_vec4("clip_plane", clip_plane);
}


int main()
{
    // window variables
    int window_w = 1840;
    int window_h = 1020;

    // water constant
    constexpr float water_h = 7.0f; // FIXME: get this from obj

    // time variables
    float total_time = 0.f;
    float delta_time = 0.f;
    float last_time = 0.f;

    // random
    std::srand(std::time(nullptr));

    GLFWwindow *window = Init::init_all(window_w, window_h);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



    // RENDER PROGRAM --------------------------------------------------------------------------------------------------
    std::vector<const char*> vertex_paths{"../shaders/volcano/vertex.glsl"};
    std::vector<const char*> geometry_paths{"../shaders/volcano/geometry.glsl"};
    std::vector<const char*> frag_paths{"../shaders/utils/simplex.glsl",
                                        "../shaders/volcano/fragment.glsl"};
    Program volcano_program(vertex_paths, geometry_paths, frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // WATER PROGRAM ---------------------------------------------------------------------------------------------------
    std::vector<const char*> water_vertex_paths{"../shaders/water/vertex.glsl"};
    std::vector<const char*> water_geometry_paths{"../shaders/water/geometry.glsl"};
    std::vector<const char*> water_frag_paths{"../shaders/water/fragment.glsl"};
    Program water_program(water_vertex_paths, water_geometry_paths, water_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // CUBEMAP PROGRAM -------------------------------------------------------------------------------------------------
    std::vector<const char*> cubemap_vertex_paths{"../shaders/cubemap/vertex.glsl"};
    std::vector<const char*> cubemap_geometry_paths{};
    std::vector<const char*> cubemap_frag_paths{"../shaders/cubemap/fragment.glsl"};
    Program cubemap_program(cubemap_vertex_paths, cubemap_geometry_paths, cubemap_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // SCREEN PROGRAM --------------------------------------------------------------------------------------------------
    std::vector<const char*> screen_vertex_paths{"../shaders/screen/vertex.glsl"};
    std::vector<const char*> screen_geometry_paths{};
    std::vector<const char*> screen_frag_paths{"../shaders/screen/fragment.glsl"};
    Program screen_program(screen_vertex_paths, screen_geometry_paths, screen_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------





    Model screen("../models/screen/screen.obj");
    Model water("../models/water/water.obj");
    Model volcan_wl("../models/volcan_with_lava/volcan_with_lava.obj");

    Cubemap cubemap = Cubemap();


    FBO screen_fbo = FBO(window_w, window_h);
    FBO reflect_fbo = FBO(window_w, window_h);
    FBO refract_fbo = FBO(window_w, window_h);



    // Create Lights
    std::vector<DirLight> dir_lights;
    std::vector<PointLight> point_lights;
    dir_lights.push_back(DirLight({0.1f, 0.1f, 0.1f}, // ambient
                                  {1.0f, 1.0f, 1.0f}, // diffuse
                                  {1.0f, 1.0f, 1.0f}, // specular
                                  {-1.f, -1.f, -1.f})); // direction
    dir_lights.push_back(DirLight({0.1f, 0.1f, 0.1f}, // ambient
                                  {1.0f, 1.0f, 1.0f}, // diffuse
                                  {1.0f, 1.0f, 1.0f}, // specular
                                  {1.f, -1.f, -1.f})); // direction
    point_lights.push_back(PointLight({0.1f, 0.1f, 0.1f}, // ambient
                                      {1.0f, 1.0f, 1.0f}, // diffuse
                                      {1.0f, 1.0f, 1.0f}, // specular
                                      {-12.0f, 2.f, 2.f})); // position
    point_lights.push_back(PointLight({0.8f, 0.0f, 0.3f}, // ambient
                                      {1.0f, 1.0f, 1.0f}, // diffuse
                                      {1.0f, 1.0f, 1.0f}, // specular
                                      {5.0f, 0.0f, 2.0f})); // position


    // To avoid redeclaration
    std::vector<GLuint> other_textures;
    // Set Model Matrix
    glm::mat4 model_mat = glm::mat4(1.f);
    //model_mat = glm::translate(model_mat, glm::vec3(10.f, -25.f, -30.f));
    //model_mat = glm::rotate(model_mat, glm::radians(40.f), glm::vec3(0.f, 1.f, 0.f));


    // main loop
    while(!glfwWindowShouldClose(window))
    {
        // check window size
        glfwGetWindowSize(window, &window_w, &window_h);

        // update delta_time
        total_time = glfwGetTime();
        delta_time = total_time - last_time;
        last_time = total_time;

        // input
        process_input(window, delta_time);





        // REFLECTION --------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, reflect_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // set camera
        float distance = 2 * (camera.pos.y - water_h);
        camera.pos.y -= distance;
        camera.invert_pitch();

        // VOLCANO WITH LAVA -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, {0, 1, 0, -water_h});
        volcano_program.set_mat4("model", model_mat);
        // Draw
        volcan_wl.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // CUBEMAP -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(cubemap_program.program_id);
        // set uniforms
        glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        cubemap_program.set_mat4("view", view);
        float window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        cubemap_program.set_mat4("projection", projection);
        // Draw
        cubemap.draw(cubemap_program);
        // -------------------------------------------------------------------------------------------------------------

        // reset camera
        camera.pos.y += distance;
        camera.invert_pitch();
        // -------------------------------------------------------------------------------------------------------------




        // REFRACTION --------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, refract_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // VOLCANO WITH LAVA -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, {0, -1, 0, water_h});
        volcano_program.set_mat4("model", model_mat);
        // Draw
        volcan_wl.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------





        // DRAW TO SCREEN FBO ------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // VOLCANO WITH LAVA -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, {0,0,0,0});
        volcano_program.set_mat4("model", model_mat);
        // Draw
        volcan_wl.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // WATER -------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(water_program.program_id);
        // set uniforms
        set_uniforms(water_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, {0,0,0,0});
        float slow_time = total_time * 0.008;
        float wave_speed = (slow_time - static_cast<int>(slow_time));
        wave_speed = std::max(wave_speed, 1.f - wave_speed) * 2.f;
        water_program.set_float("wave_speed", wave_speed);
        water_program.set_mat4("model", model_mat);
        // Draw
        other_textures = {reflect_fbo.tex_buffer, refract_fbo.tex_buffer, refract_fbo.rbo_id};
        water.draw(water_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------

        // CUBEMAP -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(cubemap_program.program_id);
        // set uniforms
        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        cubemap_program.set_mat4("view", view);
        window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        cubemap_program.set_mat4("projection", projection);
        // Draw
        cubemap.draw(cubemap_program);
        // -------------------------------------------------------------------------------------------------------------






        // DRAW TO SCREEN ----------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind back to default framebuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // SCREEN ------------------------------------------------------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(screen_program.program_id);
        // set uniforms
        set_uniforms(screen_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, {0,0,0,0});
        // Draw
        other_textures = {screen_fbo.tex_buffer};
        screen.draw(screen_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------




        // swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // clear all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();


    return 0;
}