#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include <tuple>

#include "../lib/irrKlang/irrKlang.h"

#include "init.hh"
#include "program.hh"
#include "model.hh"
#include "camera.hh"
#include "fbo.hh"
#include "light.hh"
#include "cubemap.hh"
#include "paths.hh"
#include "particle.hh"
#include "sound.hh"
#include "utils.hh"


Camera camera;
bool ice_age = false;
float ice_time = 1.0f;

irrklang::ISoundEngine *sound_engine = irrklang::createIrrKlangDevice();
Sound sound;

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
            sound_engine->play2D("../audio/ice.ogg");
        sound.pause(!ice_age);

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
                  std::vector<DirLight>& dir_lights, std::vector<PointLight>& point_lights, std::vector<LightModel>& light_models,
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
    for (int i = 0; i < light_models.size(); ++i)
        light_models[i].point_light.set(program, i);
    for (int i = 0; i < point_lights.size(); ++i)
        point_lights[i].set(program, i + light_models.size());

    program.set_vec3("camera_pos", camera.pos);
    program.set_vec2("mouse_pos", camera.mouse_pos);

    glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
    program.set_mat4("view", view);

    float window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
    program.set_mat4("projection", projection);

    program.set_vec4("clip_plane", clip_plane);
}

void load_fbo(GLuint fbo_id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void init_lava_lights(std::vector<PointLight>& point_lights, Light& color, std::vector<glm::vec3>& positions)
{
    for (int i = 0; i < positions.size(); i++)
        point_lights.emplace_back(PointLight(color.ambient, color.diffuse, color.specular, positions[i]));
    std::cout << "points lights size " << point_lights.size() << std::endl;
}

int main()
{
    // window variables
    // alex 1840 1020
    // tibo 1000 700
    int window_w = 1840;
    int window_h = 1020;
    int last_window_w = window_w;
    int last_window_h = window_h;

    // water constant
    constexpr float water_h = 7.0f; // FIXME: get this from obj
    constexpr glm::vec3 water_limits = glm::vec3(22.5, water_h, 19);

    // time variables
    float total_time = 0.f;
    float delta_time = 0.f;
    float last_time = 0.f;


    GLFWwindow *window = Init::init_all(window_w, window_h);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    std::srand(std::time(nullptr));

    // LOAD PROGRAMS ---------------------------------------------------------------------------------------------------
    Program volcano_program(PATHS::volcano_vertex, PATHS::volcano_tc, PATHS::volcano_te, PATHS::volcano_geometry, PATHS::volcano_frag);
    Program lava_program(PATHS::lava_vertex, PATHS::lava_tc, PATHS::lava_te, PATHS::lava_geometry, PATHS::lava_frag);
    Program water_program(PATHS::water_vertex, PATHS::water_tc, PATHS::water_te, PATHS::water_geometry, PATHS::water_frag);
    Program cubemap_program(PATHS::cubemap_vertex, PATHS::cubemap_tc, PATHS::cubemap_te, PATHS::cubemap_geometry, PATHS::cubemap_frag);
    Program light_program(PATHS::light_vertex, PATHS::light_tc, PATHS::light_te, PATHS::light_geometry, PATHS::light_frag);
    Program particle_program(PATHS::particle_vertex, PATHS::particle_tc, PATHS::particle_te, PATHS::particle_geometry, PATHS::particle_frag);
    Program bloom_program(PATHS::bloom_vertex, PATHS::bloom_tc, PATHS::bloom_te, PATHS::bloom_geometry, PATHS::bloom_frag);
    Program blur_program(PATHS::blur_vertex, PATHS::blur_tc, PATHS::blur_te, PATHS::blur_geometry, PATHS::blur_frag);
    Program screen_program(PATHS::screen_vertex, PATHS::screen_tc, PATHS::screen_te, PATHS::screen_geometry, PATHS::screen_frag);
    // -----------------------------------------------------------------------------------------------------------------


    std::vector<LightModel> light_models;

    Light light_color1 = Light({0.1f, 0.1f, 0.1f}, {15.0f, 1.0f, 1.0f},  {15.0f, 1.0f, 1.0f});
    Light light_color2 = Light({0.1f, 0.1f, 0.1f}, {15.0f, 1.0f, 1.0f}, {15.0f, 1.0f, 1.0f});

    Model screen("../models/screen/screen.obj", GL_TRIANGLES);
    Model water("../models/water/waterLOD0.obj", GL_PATCHES);
    Model volcano("../models/volcan/volcan.obj", GL_TRIANGLES);
    Model lava("../models/lava/lava.obj", GL_PATCHES);
    Model lava_skeleton("../models/lava/lava_skeleton.obj", GL_TRIANGLES);
    Model cave_lava("../models/cave_lava/cave_lava.obj", GL_PATCHES);
    Model lamp1("../models/lamps/lamp1/lamp.obj", GL_TRIANGLES);
    Model lamp2("../models/lamps/lamp2/lamp.obj", GL_TRIANGLES);
    LightModel light1("../models/lamps/lamp1/light.obj", GL_TRIANGLES, light_color1);
    LightModel light2("../models/lamps/lamp2/light.obj", GL_TRIANGLES, light_color2);

    light_models.push_back(light1);
    light_models.push_back(light2);

    Cubemap cubemap = Cubemap();

    FBO reflect_fbo = FBO(window_w, window_h, 1);
    FBO refract_fbo = FBO(window_w, window_h, 1);

    FBO bloom_fbo = FBO(window_w, window_h, 2);
    FBO ping_pong1 = FBO(window_w, window_h, 1);
    FBO ping_pong2 = FBO(window_w, window_h, 1);

    FBO scene_fbo = FBO(window_w, window_h, 1);



    // Create Lights
    std::vector<DirLight> dir_lights;
    dir_lights.push_back(DirLight({0.1f, 0.1f, 0.1f}, // ambient
                                  {0.5f, 0.5f, 0.5f}, // diffuse
                                  {0.5f, 0.5f, 0.5f}, // specular
                                  {-1.f, -1.f, -1.f})); // direction
    dir_lights.push_back(DirLight({0.1f, 0.1f, 0.1f}, // ambient
                                  {0.5f, 0.5f, 0.5f}, // diffuse
                                  {0.5f, 0.5f, 0.5f}, // specular
                                  {1.f, -1.f, -1.f})); // direction
    std::vector<PointLight> point_lights;
    Light lava_color1 = Light({0.f, 0.f, 0.f}, {10.0f, 1.0f, 0.5f}, {10.0f, 1.0f, 0.5f});
    Light lava_color2 = Light({0.f, 0.f, 0.f}, {1.4f, 0.4f, 0.f}, {1.4f, 0.4f, 0.f});
    init_lava_lights(point_lights, lava_color1, Utils::lava_nexus);
    init_lava_lights(point_lights, lava_color2, Utils::lava_skeleton_light);


    // To avoid redeclaration
    std::vector<GLuint> other_textures;
    glm::mat4 view;
    glm::mat4 projection;
    float window_ratio;
    // Set Model Matrix
    glm::mat4 model_mat = glm::mat4(1.f);
    //model_mat = glm::translate(model_mat, glm::vec3(10.f, -25.f, -30.f));
    //model_mat = glm::rotate(model_mat, glm::radians(40.f), glm::vec3(0.f, 1.f, 0.f));

    LavaParticleGenerator lava_generator = init_lava_particle_generator(lava_skeleton);
    SnowParticleGenerator snow_generator = init_snow_particle_generator();

    // SOUND
    sound.init_lava_sound(sound_engine, 0.9, Utils::lava_nexus);
    sound.init_lava_sound(sound_engine, 0.2, Utils::lava_skeleton_light);
    sound.init_water_sound(sound_engine, 0.5, Utils::water_skeleton);


    // main loop
    while(!glfwWindowShouldClose(window))
    {
        // check window size
        glfwGetWindowSize(window, &window_w, &window_h);
        if (window_w != last_window_w || window_h != last_window_h)
        {
            reflect_fbo.realloc(window_w, window_h);
            refract_fbo.realloc(window_w, window_h);
            bloom_fbo.realloc(window_w, window_h);
            ping_pong1.realloc(window_w, window_h);
            ping_pong2.realloc(window_w, window_h);
            scene_fbo.realloc(window_w, window_h);
        }

        // update delta_time
        total_time = glfwGetTime();
        delta_time = total_time - last_time;
        last_time = total_time;
        ice_time += delta_time;

        // input
        process_input(window, delta_time);

        // Update sounds
        sound_engine->setListenerPosition({camera.pos.x,camera.pos.y,camera.pos.z}, {camera.front.x,camera.front.y,camera.front.z});

        // Update particles
        if (ice_age)
            snow_generator.update(delta_time, total_time);
        else
            lava_generator.update(delta_time, total_time);


        // =============================================================================================================
        // REFLECTION --------------------------------------------------------------------------------------------------
        // =============================================================================================================
        load_fbo(reflect_fbo.fbo_id);

        // set camera
        float distance = 2 * (camera.pos.y - water_h);
        camera.pos.y -= distance;
        camera.invert_pitch();

        // VOLCANO -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0, 1, 0, -water_h+0.5});
        volcano_program.set_mat4("model", model_mat);
        volcano_program.set_vec3("water_limits", water_limits);
        volcano_program.set_float("ice_time", ice_time);
        // Draw
        volcano.draw(volcano_program, nullptr);
        lamp1.draw(volcano_program, nullptr);
        lamp2.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // LAVA --------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(lava_program.program_id);
        // set uniforms
        set_uniforms(lava_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0, 1, 0, -water_h+0.5});
        lava_program.set_mat4("model", model_mat);
        // Draw
        lava.draw(lava_program, nullptr);
        cave_lava.draw(lava_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // LIGHTS -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(light_program.program_id);
        // set uniforms
        set_uniforms(light_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0, 1, 0, -water_h+0.5});
        light_program.set_mat4("model", model_mat);
        // Draw
        light_program.set_vec3("light_color", light1.point_light.diffuse);
        light1.draw(light_program, nullptr);
        light_program.set_vec3("light_color", light2.point_light.diffuse);
        light2.draw(light_program, nullptr);
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
        cubemap_program.set_mat4("view", view);
        cubemap_program.set_mat4("projection", projection);
        // Draw
        cubemap.draw(cubemap_program);
        // -------------------------------------------------------------------------------------------------------------

        // PARTICLES ---------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(particle_program.program_id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        particle_program.set_mat4("view", view);
        particle_program.set_mat4("projection", projection);
        if (ice_age)
            snow_generator.draw(particle_program);
        else
            lava_generator.draw(particle_program);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // -------------------------------------------------------------------------------------------------------------

        // reset camera
        camera.pos.y += distance;
        camera.invert_pitch();
        // -------------------------------------------------------------------------------------------------------------




        // =============================================================================================================
        // REFRACTION --------------------------------------------------------------------------------------------------
        // =============================================================================================================
        load_fbo(refract_fbo.fbo_id);

        // VOLCANO -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0, -1, 0, water_h+1});
        volcano_program.set_mat4("model", model_mat);
        volcano_program.set_vec3("water_limits", water_limits);
        volcano_program.set_float("ice_time", ice_time);
        // Draw
        volcano.draw(volcano_program, nullptr);
        lamp1.draw(volcano_program, nullptr);
        lamp2.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // LAVA --------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(lava_program.program_id);
        // set uniforms
        set_uniforms(lava_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0, -1, 0, water_h});
        lava_program.set_mat4("model", model_mat);
        // Draw
        lava.draw(lava_program, nullptr);
        cave_lava.draw(lava_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------





        // =============================================================================================================
        // DRAW TO SCENE FBO -------------------------------------------------------------------------------------------
        // =============================================================================================================
        load_fbo(scene_fbo.fbo_id);

        // VOLCANO -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(volcano_program.program_id);
        // set uniforms
        set_uniforms(volcano_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0,0,0,0});
        volcano_program.set_mat4("model", model_mat);
        volcano_program.set_vec3("water_limits", water_limits);
        volcano_program.set_float("ice_time", ice_time);
        // Draw
        volcano.draw(volcano_program, nullptr);
        lamp1.draw(volcano_program, nullptr);
        lamp2.draw(volcano_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // LAVA --------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(lava_program.program_id);
        // set uniforms
        set_uniforms(lava_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0,0,0,0});
        lava_program.set_mat4("model", model_mat);
        // Draw
        lava.draw(lava_program, nullptr);
        cave_lava.draw(lava_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // LIGHTS -------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(light_program.program_id);
        // set uniforms
        set_uniforms(light_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0,0,0,0});
        light_program.set_mat4("model", model_mat);
        // Draw
        light_program.set_vec3("light_color", light1.point_light.diffuse);
        light1.draw(light_program, nullptr);
        light_program.set_vec3("light_color", light2.point_light.diffuse);
        light2.draw(light_program, nullptr);
        // -------------------------------------------------------------------------------------------------------------

        // WATER -------------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(water_program.program_id);
        // set uniforms
        set_uniforms(water_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0,0,0,0});
        float slow_time = total_time * 0.007;
        float wave_speed = (slow_time - static_cast<int>(slow_time));
        wave_speed = std::max(wave_speed, 1.f - wave_speed) * 2.f;
        water_program.set_float("wave_speed", wave_speed);
        water_program.set_mat4("model", model_mat);
        // Draw
        other_textures = {reflect_fbo.color_textures[0], refract_fbo.color_textures[0], refract_fbo.depth_texture};
        water.draw(water_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------

        // CUBEMAP -----------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glUseProgram(cubemap_program.program_id);
        // set uniforms
        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        cubemap_program.set_mat4("view", view);
        cubemap_program.set_mat4("projection", projection);
        // Draw
        cubemap.draw(cubemap_program);
        // -------------------------------------------------------------------------------------------------------------

        // PARTICLES ---------------------------------------------------------------------------------------------------
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(particle_program.program_id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        window_ratio = window_w > window_h ? (float)window_w/(float)window_h : (float)window_h/(float)window_w;
        projection = glm::perspective(glm::radians(camera.fov), window_ratio, 0.1f, 1000.0f);
        particle_program.set_mat4("view", view);
        particle_program.set_mat4("projection", projection);
        if (ice_age)
            snow_generator.draw(particle_program);
        else
            lava_generator.draw(particle_program);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // -------------------------------------------------------------------------------------------------------------





        // =============================================================================================================
        // BLOOM -------------------------------------------------------------------------------------------------------
        // =============================================================================================================
        load_fbo(bloom_fbo.fbo_id);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(bloom_program.program_id);
        // Draw
        other_textures = {scene_fbo.color_textures[0]};
        screen.draw(bloom_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------



        // =============================================================================================================
        // BLUR --------------------------------------------------------------------------------------------------------
        // =============================================================================================================
        bool horizontal = true;
        int amount = 5;

        for (unsigned int i = 0; i < amount; i++)
        {
            if (i % 2 == 0)
                load_fbo(ping_pong1.fbo_id);
            else
                load_fbo(ping_pong2.fbo_id);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glUseProgram(blur_program.program_id);

            blur_program.set_int("horizontal", horizontal);

            if (i == 0)
                other_textures = {bloom_fbo.color_textures[1]};
            else if (i % 2 == 0)
                other_textures = {ping_pong2.color_textures[0]};
            else
                other_textures = {ping_pong1.color_textures[0]};

            screen.draw(blur_program, &other_textures);
            horizontal = !horizontal;
        }





        // =============================================================================================================
        // DRAW TO SCREEN ----------------------------------------------------------------------------------------------
        // =============================================================================================================
        load_fbo(0); // bind back to default framebuffer

        // SCREEN ------------------------------------------------------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(screen_program.program_id);
        // set uniforms
        set_uniforms(screen_program, window_w, window_h, total_time, delta_time, dir_lights, point_lights, light_models, {0,0,0,0});
        screen_program.set_bool("ice_age", ice_age);
        screen_program.set_float("ice_time", ice_time);
        // Draw
        if (amount % 2 == 0)
            other_textures = {scene_fbo.color_textures[0], ping_pong2.color_textures[0]};
        else
            other_textures = {scene_fbo.color_textures[0], ping_pong1.color_textures[0]};
        screen.draw(screen_program, &other_textures);
        // -------------------------------------------------------------------------------------------------------------





        // swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // clear sound
    sound_engine->drop();
    sound.clear();

    // clear all previously allocated GLFW resources.
    glfwTerminate();

    return 0;
}