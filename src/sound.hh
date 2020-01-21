#pragma once

#include "model.hh"
#include "../lib/irrKlang/ik_ISoundEngine.h"

class Sound
{
public:
    std::vector<irrklang::ISound*> lava_sounds;
    std::vector<irrklang::ISound*> water_sounds;

    void init_lava_sound(irrklang::ISoundEngine *SoundEngine, float distance, const std::vector<glm::vec3>& positions);
    void init_water_sound(irrklang::ISoundEngine *SoundEngine, float distance, const std::vector<glm::vec3>& positions);
    void pause(bool pause);
    void clear();
};
