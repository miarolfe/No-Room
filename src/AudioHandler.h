#pragma once

#include <string>
#include <map>
#include <vector>
#include "SDL_mixer.h"

using string = std::string;

class AudioHandler {
public:
    std::map<string, Mix_Chunk*> effectMap;
    std::map<string, Mix_Music*> trackMap;

    AudioHandler(const std::vector<string>& effectPaths, const std::vector<string>& trackPaths) {
        for (const string& effectPath : effectPaths) {
            std::string name = effectPath.substr(0, effectPath.size()-4);
            name = name.substr(name.find_last_of('/') + 1);
            Mix_Chunk* effect = Mix_LoadWAV(effectPath.c_str());
            effectMap[name] = effect;
        }

        for (const string& trackPath : trackPaths) {
            std::string name = trackPath.substr(0, trackPath.size()-4);
            name = name.substr(name.find_last_of('/') + 1);
            Mix_Music* track = Mix_LoadMUS(trackPath.c_str());
            trackMap[name] = track;
        }
    }

    bool PlayEffect(const string& name, int loops=1) {
        if (effectMap.count(name) > 0) {
            const int returnCode = Mix_PlayChannel(-1, effectMap[name], loops);
            if (returnCode != -1) {
                return true;
            }
        }

        return false;
    }

    bool PlayTrack(const string& name, int loops=1) {
        if (trackMap.count(name) > 0) {
            const int returnCode = Mix_PlayMusic(trackMap[name], loops);
            if (returnCode != -1) {
                return true;
            }
        }

        return false;
    }
};