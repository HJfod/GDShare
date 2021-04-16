#pragma once

#include "../offsets.hpp"

class GameLevelManager : public cocos2d::CCNode {
    protected:

    public:
        inline static GameLevelManager* sharedState() {
            return as<GameLevelManager*(__stdcall*)()>( gd::base + 0x9f860 )();
        }

        inline static gd::GJGameLevel* createNewLevel() {
            return as<gd::GJGameLevel*(__stdcall*)()>( gd::base + 0xa0db0 )();
        }
};
