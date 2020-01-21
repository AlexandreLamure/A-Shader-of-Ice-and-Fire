#include "sound.hh"


void Sound::init_lava_sound(irrklang::ISoundEngine *SoundEngine, float distance, const std::vector<glm::vec3>& positions)
{
    for (int i = 0; i < positions.size(); ++i)
    {
        irrklang::vec3df pos(positions[i].x, positions[i].y, positions[i].z);
        irrklang::ISound *lava_sound = SoundEngine->play3D("../audio/lava.ogg", pos, true, false, true);
        if (lava_sound)
        {
            lava_sound->setMinDistance(distance);
            lava_sounds.emplace_back(lava_sound);
        }
    }

    std::cout << lava_sounds.size() << " lava sounds" << std::endl;
}

void Sound::init_water_sound(irrklang::ISoundEngine *SoundEngine, float distance, const std::vector<glm::vec3>& positions)
{
    for (int i = 0; i < positions.size(); ++i)
    {
        irrklang::vec3df pos(positions[i].x, positions[i].y, positions[i].z);
        irrklang::ISound *water_sound = SoundEngine->play3D("../audio/water.ogg", pos, true, false, true);
        if (water_sound)
        {
            water_sound->setMinDistance(distance);
            water_sounds.emplace_back(water_sound);
        }
    }
    std::cout << water_sounds.size() << " water sounds" << std::endl;
}

void Sound::pause(bool pause)
{
    for (int i = 0; i < lava_sounds.size(); ++i)
        lava_sounds[i]->setIsPaused(pause);
    for (int i = 0; i < water_sounds.size(); ++i)
        water_sounds[i]->setIsPaused(pause);
}

void Sound::clear()
{
    for (int i = 0; i < lava_sounds.size(); ++i)
        lava_sounds[i]->drop();
    for (int i = 0; i < water_sounds.size(); ++i)
        water_sounds[i]->drop();
}
