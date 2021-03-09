#pragma once

#include "../offsets.hpp"

class CCScrollLayerExt : public cocos2d::CCLayer {
    public:
        virtual void destructor() {
            
        }

        static CCScrollLayerExt* create(
            cocos2d::CCLayer* layer,
            cocos2d::CCPoint pos,
            int unknown,
            float width,
            float height
        ) {
            auto ret = reinterpret_cast<CCScrollLayerExt*(__thiscall*)(
                cocos2d::CCLayer*,
                cocos2d::CCPoint,
                int, float, float
            )>(
                ModLdr::base + 0x1b020
            )(layer, pos, unknown, width, height);

            __asm add esp, 0x10

            return ret;
        }
};

