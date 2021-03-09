#pragma once

#include "../offsets.hpp"

#pragma runtime_checks("s", off)
class GJListLayer : public cocos2d::CCLayerColor {
    protected:
        CCObject* m_pTarget;

    public:
        virtual ~GJListLayer() {
            reinterpret_cast<void(__thiscall*)(GJListLayer*, bool)>(
                ModLdr::base + 0x12DFD0
            )(this, true);
        }
        static GJListLayer* create(CCObject* target, const char* title, cocos2d::ccColor4B color, float width, float height) {
            __asm movss xmm3, width
            
            auto pRet = reinterpret_cast<GJListLayer * (__fastcall*)(CCObject*, const char*, cocos2d::ccColor4B, float)>(
                ModLdr::base + 0x12E000
            )(target, title, color, height);

            return pRet;
        }
};
#pragma runtime_checks("s", restore)