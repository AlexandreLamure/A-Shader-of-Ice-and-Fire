#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include <tuple>

#include "../lib/irrklang/irrKlang.h"

#include "init.hh"
#include "program.hh"
#include "model.hh"
#include "camera.hh"
#include "fbo.hh"
#include "light.hh"
#include "cubemap.hh"


Camera camera;
bool ice_age = false;
float ice_time = 1.0f;

irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Toggle Ice transition
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
    {
        if (!ice_age)
            SoundEngine->play2D("../audio/zawarudo.mp3");

        ice_age = !ice_age;
        ice_time = 0.0f;
    }
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
                  std::vector<DirLight>& dir_lights, std::vector<LightModel>& model_lights,
                  glm::vec4 clip_plane)
{
    // set uniforms
    program.set_float("total_time", total_time);
    program.set_float("delta_time", delta_time);
    program.set_vec2("resolution", window_w, window_h);

    program.set_bool("ice_age", ice_age);
    program.set_float("ice_time", ice_time);

    // set lights
    for (int i = 0; i < dir_lights.size(); ++i)
        dir_lights[i].set(program, i);
    for (int i = 0; i < model_lights.size(); ++i)
        model_lights[i].pointlight.set(program, i);

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
    // alex 1840 1020
    // tibo 1000 700
    int window_w = 1000;
    int window_h = 700;

    // water constant
    constexpr float water_h = 7.0f; // FIXME: get this from obj

    // time variables
    float total_time = 0.f;
    float delta_time = 0.f;
    float last_time = 0.f;


    GLFWwindow *window = Init::init_all(window_w, window_h);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);



    // VOLCANO PROGRAM -------------------------------------------------------------------------------------------------
    std::vector<const char*> vertex_paths{"../shaders/volcano/vertex.glsl"};
    std::vector<const char*> tc_paths{};
    std::vector<const char*> te_paths{};
    std::vector<const char*> geometry_paths{"../shaders/volcano/geometry.glsl"};
    std::vector<const char*> frag_paths{"../shaders/utils/simplex.glsl",
                                        "../shaders/volcano/fragment.glsl"};
    Program volcano_program(vertex_paths, tc_paths, te_paths, geometry_paths, frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // VOLCANO PROGRAM -------------------------------------------------------------------------------------------------
    std::vector<const char*> lava_vertex_paths{"../shaders/lava/vertex.glsl"};
    std::vector<const char*> lava_tc_paths{"../shaders/lava/tcontrol.glsl"};
    std::vector<const char*> lava_te_paths{"../shaders/lava/teval.glsl"};
    std::vector<const char*> lava_geometry_paths{"../shaders/utils/misc.glsl",
                                                 "../shaders/utils/simplex.glsl",
                                                 "../shaders/utils/lava_texture.glsl",
                                                 "../shaders/utils/ice.glsl",
                                                 "../shaders/lava/geometry.glsl"};
    std::vector<const char*> lava_frag_paths{"../shaders/utils/misc.glsl",
                                             "../shaders/utils/simplex.glsl",
                                             "../shaders/utils/lava_texture.glsl",
                                             "../shaders/utils/ice.glsl",
                                             "../shaders/lava/fragment.glsl"};
    Program lava_program(lava_vertex_paths, lava_tc_paths, lava_te_paths, lava_geometry_paths, lava_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // WATER PROGRAM ---------------------------------------------------------------------------------------------------
    std::vector<const char*> water_vertex_paths{"../shaders/water/vertex.glsl"};
    std::vector<const char*> water_tc_paths{"../shaders/water/tcontrol.glsl"};
    std::vector<const char*> water_te_paths{"../shaders/water/teval.glsl"};
    std::vector<const char*> water_geometry_paths{"../shaders/utils/ice.glsl",
                                                  "../shaders/water/geometry.glsl"};
    std::vector<const char*> water_frag_paths{"../shaders/utils/ice.glsl",
                                              "../shaders/water/fragment.glsl"};
    Program water_program(water_vertex_paths, water_tc_paths, water_te_paths, water_geometry_paths, water_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // CUBEMAP PROGRAM -------------------------------------------------------------------------------------------------
    std::vector<const char*> cubemap_vertex_paths{"../shaders/cubemap/vertex.glsl"};
    std::vector<const char*> cubemap_tc_paths{};
    std::vector<const char*> cubemap_te_paths{};
    std::vector<const char*> cubemap_geometry_paths{};
    std::vector<const char*> cubemap_frag_paths{"../shaders/cubemap/fragment.glsl"};
    Program cubemap_program(cubemap_vertex_paths, cubemap_tc_paths, cubemap_te_paths, cubemap_geometry_paths, cubemap_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // LIGHT PROGRAM --------------------------------------------------------------------------------------------------
    std::vector<const char*> light_vertex_paths{"../shaders/light2/vertex.glsl"};
    std::vector<const char*> light_tc_paths{};
    std::vector<const char*> light_te_paths{};
    std::vector<const char*> light_geometry_paths{"../shaders/light2/geometry.glsl"};
    std::vector<const char*> light_frag_paths{"../shaders/light2/fragment.glsl"};
    Program light_program(light_vertex_paths, light_tc_paths, light_te_paths, light_geometry_paths, light_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    // BLOOM PROGRAM --------------------------------------------------------------------------------------------------
    std::vector<const char*> bloom_vertex_paths{"../shaders/bloom/vertex.glsl"};
    std::vector<const char*> bloom_tc_paths{};
    std::vector<const char*> bloom_te_paths{};
    std::vector<const char*> bloom_geometry_paths{};
    std::vector<const char*> bloom_frag_paths{"../shaders/bloom/fragment.glsl"};
    Program bloom_program(bloom_vertex_paths, bloom_tc_paths, bloom_te_paths, bloom_geometry_paths, bloom_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------

    // BLUR PROGRAM --------------------------------------------------------------------------------------------------
    std::vector<const char*> blur_vertex_paths{"../shaders/blur/vertex.glsl"};
    std::vector<const char*> blur_tc_paths{};
    std::vector<const char*> blur_te_paths{};
    std::vector<const char*> blur_geometry_paths{};
    std::vector<const char*> blur_frag_paths{"../shaders/blur/fragment.glsl"};
    Program blur_program(blur_vertex_paths, blur_tc_paths, blur_te_paths, blur_geometry_paths, blur_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------



    // SCREEN PROGRAM --------------------------------------------------------------------------------------------------
    std::vector<const char*> screen_vertex_paths{"../shaders/screen/vertex.glsl"};
    std::vector<const char*> screen_tc_paths{};
    std::vector<const char*> screen_te_paths{};
    std::vector<const char*> screen_geometry_paths{};
    std::vector<const char*> screen_frag_paths{"../shaders/screen/fragment.glsl"};
    Program screen_program(screen_vertex_paths, screen_tc_paths, screen_te_paths, screen_geometry_paths, screen_frag_paths);
    // -----------------------------------------------------------------------------------------------------------------


    std::vector<LightModel> model_lights;

    Light li = Light({0.1f, 0.1f, 0.1f}, {10.0f, 1.0f, 10.0f}, {10.0f, 1.0f, 10.0f});
    Light li2 = Light({0.1f, 0.1f, 0.1f}, // ambient
                                      {1.0f, 8.0f, 8.0f}, // diffuse
                                      {1.0f, 8.0f, 8.0f});

    Model screen("../models/screen/screen.obj", GL_TRIANGLES);
    Model water("../models/water/waterLOD0.obj", GL_PATCHES);
    Model volcano("../models/volcan/volcan.obj", GL_TRIANGLES);
    //Model lava("../models/lava/lava.obj", GL_PATCHES);
    //Model cave_lava("../models/cave_lava/cave_lava.obj", GL_PATCHES);
    Model lamp1("../models/lamps/lamp1/lamp.obj", GL_TRIANGLES);
    Model lamp2("../models/lamps/lamp2/lamp.obj", GL_TRIANGLES);
    LightModel light1("../models/lamps/lamp1/light.obj", GL_TRIANGLES, li);
    LightModel light2("../models/lamps/lamp2/light.obj", GL_TRIANGLES, li2);

    model_lights.push_back(light1);
    model_lights.push_back(light2);

    Cubemap cubemap = Cubemap();

    FBO reflect_fbo = FBO(window_w, window_h, 1);
    FBO refract_fbo = FBO(window_w, window_h, 1);

    FBO bloom_fbo = FBO(window_w, window_h, 2);
    FBO ping_pong_1 = FBO(window_w, window_h, 1);
    FBO ping_pong_2 = FBO(window_w, window_h, 1);

    FBO screen_fbo = FBO(window_w, window_h, 1);



    // Create Lights
    std::vector<DirLight> dir_lights;
    //std::vector<PointLight> point_lights;
    /*dir_lights.push_back(DirLight({0.f, 0.f, 0.f}, // ambient
                                  {0.f, 0.f, 0.f}, // diffuse
                                  {0.f, 0.f, 0.f}, // specular
                                  {-1.f, -1.f, -1.f})); // direction*/
    dir_lights.push_back(DirLight({0.1f, 0.1f, 0.1f}, // ambient
                                  {1.0f, 1.0f, 1.0f}, // diffuse
                                  {1.0f, 1.0f, 1.0f}, // specular
                                  {-1.f, -1.f, -1.f})); // direction
    dir_lights.push_back(DirLight({0.1f, 0.1f, 0.1f}, // ambient
                                  {1.0f, 1.0f, 1.0f}, // diffuse
                                  {1.0f, 1.0f, 1.0f}, // specular
                                  {1.f, -1.f, -1.f})); // direction


    // To avoid redeclaration
    std::vector<GLuint> other_textures;
    // Set Model Matrix
    glm::mat4 model_mat = glm::mat4(1.f);
    //model_mat = glm::translate(model_mat, glm::vec3(10.f, -25.f, -30.f));
    //model_mat = glm::rotate(model_mat, glm::radians(40.f), glm::vec3(0.f, 1.f, 0.f));


    // SOUND
    //SoundEngine->play2D("../audio/getout.ogg", GL_TRUE);


    // main loop
    while(!glfwWindowShouldClose(window))
    {
        // check window size
        glfwGetWindowSize(window, &window_w, &window_h);

        // update delta_time
        total_time = glfwGetTime();
        delta_time = total_time - last_time;
        last_time = total_time;
        ice_time += delta_time;

        // input
        process_input(window, delta_time);





        // =============================================================================================================
        // REFLECTION --------------------------------------------------------------------------------------------------
        // =============================================================================================================
        glBindFramebuffer(GL_FRAMEBUFFER, reflect_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // set camera
        float distance = 2 * (camera.pos.y - water_h);
        camera.pos.y -= distance;
        camera.invert_pitch();

        // VOLCANO -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0, 1, 0, -water_h});
        volcano_program.set_mat4("model", model_mat);
        // Draw
        volcano.draw(volcano_program, nullptr);
        lamp1.draw(volcano_program, nullptr);
        lamp2.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------
/*
        // LAVA --------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(lava_program.program_id);
        // set uniforms
        set_uniforms(lava_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0, 1, 0, -water_h});
        lava_program.set_mat4("model", model_mat);
        // Draw
        lava.draw(lava_program, nullptr);
        cave_lava.draw(lava_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

*/

        // LIGHTS -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(light_program.program_id);
        // set uniforms
        set_uniforms(light_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0, 1, 0, -water_h});
        light_program.set_mat4("model", model_mat);
        // Draw
        // Draw
        light_program.set_vec3("lightColor", {10.0f, 1.0f, 10.0f});
        light1.draw(light_program, nullptr);
        light_program.set_vec3("lightColor", {1.0f, 8.0f, 8.0f});
        light2.draw(light_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------


        // CUBEMAP -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(cubemap_program.program_id);
        // set uniforms

        glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        float window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        cubemap_program.set_mat4("view", view);
        cubemap_program.set_mat4("projection", projection);
        // Draw
        cubemap.draw(cubemap_program);
        // -------------------------------------------------------------------------------------------------------------

        // reset camera
        camera.pos.y += distance;
        camera.invert_pitch();
        // -------------------------------------------------------------------------------------------------------------






        // =============================================================================================================
        // REFRACTION --------------------------------------------------------------------------------------------------
        // =============================================================================================================
        glBindFramebuffer(GL_FRAMEBUFFER, refract_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // VOLCANO -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0, -1, 0, water_h});
        volcano_program.set_mat4("model", model_mat);
        // Draw
        volcano.draw(volcano_program, nullptr);
        lamp1.draw(volcano_program, nullptr);
        lamp2.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------
/*
        // LAVA --------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(lava_program.program_id);
        // set uniforms
        set_uniforms(lava_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0, -1, 0, water_h});
        lava_program.set_mat4("model", model_mat);
        // Draw
        lava.draw(lava_program, nullptr);
        cave_lava.draw(lava_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

*/





        // =============================================================================================================
        // DRAW TO SCREEN FBO ------------------------------------------------------------------------------------------
        // =============================================================================================================
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // VOLCANO -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0,0,0,0});
        volcano_program.set_mat4("model", model_mat);
        // Draw
        volcano.draw(volcano_program, nullptr);
        lamp1.draw(volcano_program, nullptr);
        lamp2.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------
/*
        // LAVA --------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(lava_program.program_id);
        // set uniforms
        set_uniforms(lava_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0,0,0,0});
        lava_program.set_mat4("model", model_mat);
        // Draw
        lava.draw(lava_program, nullptr);
        cave_lava.draw(lava_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------
*/

        // LIGHTS -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(light_program.program_id);
        // set uniforms
        set_uniforms(light_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0, 1, 0, -water_h});
        light_program.set_mat4("model", model_mat);
        // Draw
        // Draw
        light_program.set_vec3("lightColor", {10.0f, 1.0f, 10.0f});
        light1.draw(light_program, nullptr);
        light_program.set_vec3("lightColor", {1.0f, 8.0f, 8.0f});
        light2.draw(light_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------


        // WATER -------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(water_program.program_id);
        // set uniforms
        set_uniforms(water_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0,0,0,0});
        float slow_time = total_time * 0.007;
        float wave_speed = (slow_time - static_cast<int>(slow_time));
        wave_speed = std::max(wave_speed, 1.f - wave_speed) * 2.f;
        water_program.set_float("wave_speed", wave_speed);
        water_program.set_mat4("model", model_mat);
        // Draw
        other_textures = {reflect_fbo.color_textures[0], refract_fbo.color_textures[0], refract_fbo.depth_texture};
        //water.draw(water_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------

        // CUBEMAP -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(cubemap_program.program_id);
        // set uniforms



        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);



        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        cubemap_program.set_mat4("view", view);
        window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        cubemap_program.set_mat4("projection", projection);
        // Draw
        cubemap.draw(cubemap_program);
        // -------------------------------------------------------------------------------------------------------------



        // BLOOOOOOOMMMMM
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo.fbo_id);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(bloom_program.program_id);

        // Draw

        other_textures = {screen_fbo.color_textures[0]};
        screen.draw(bloom_program, &other_textures);

        // =============================================================================================================
        // BLUR ----------------------------------------------------------------------------------------------
        // =============================================================================================================

        bool horizontal = true;
        int amount = 10;

        /*
        glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_1.fbo_id);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);


        blur_program.set_int("horizontal", horizontal);
        other_textures = {bloom_fbo.color_textures[1]};
        screen.draw(blur_program, &other_textures);
*/

        for (unsigned int i = 0; i < amount; i++)
        {
            if (i % 2 == 0)
                glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_1.fbo_id);
            else
                glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_2.fbo_id);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            glUseProgram(blur_program.program_id);

            blur_program.set_int("horizontal", horizontal);

            if (i == 0)
                other_textures = {bloom_fbo.color_textures[1]};

            else if (i % 2 == 0)
                other_textures = {ping_pong_2.color_textures[0]};

            else
                other_textures = {ping_pong_1.color_textures[0]};

            screen.draw(blur_program, &other_textures);

            // ???
            //RenderQuad();

            horizontal = !horizontal;
        }





        // =============================================================================================================
        // DRAW TO SCREEN ----------------------------------------------------------------------------------------------
        // =============================================================================================================
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind back to default framebuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // SCREEN ------------------------------------------------------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(screen_program.program_id);
        // set uniforms
        set_uniforms(screen_program, window_w, window_h, total_time, delta_time, dir_lights, model_lights, {0,0,0,0});
        // Draw
        other_textures = {bloom_fbo.color_textures[0], ping_pong_2.color_textures[0]};
        screen.draw(screen_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------









        // swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // SOUND CLEAR

    SoundEngine->drop();


    // clear all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();


    return 0;
}