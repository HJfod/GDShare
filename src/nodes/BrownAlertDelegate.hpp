#pragma once

#include "../offsets.hpp"

class BrownAlertDelegate : public gd::FLAlertLayer {
    protected:
        // layer size is stored here
        cocos2d::CCSize m_pLrSize;

        virtual bool init(
            float width,
            float height,
            const char* bg = "GJ_square01.png",
            const char* title = ""
        );
        virtual void setup() = 0;

        virtual void onClose(cocos2d::CCObject*);
};

