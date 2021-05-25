#pragma once

#include "../offsets.hpp"

class BGCheckbox : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_pBGLayer;
        gd::CCMenuItemToggler* m_pToggler;
        cocos2d::CCLabelBMFont* m_pLabel;

        bool init(const char*);
    
    public:
        static BGCheckbox* create(const char*);

        void setEnabled(bool);

        inline gd::CCMenuItemToggler* getToggle() { return m_pToggler; }
};
