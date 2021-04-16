#pragma once

#include "../offsets.hpp"

class HorizontalSelectMenu : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_pBGLayer;
        std::vector<gd::CCMenuItemToggler*> m_vToggles;
        unsigned int m_nSelIndex = 0u;

        bool init(std::vector<const char*> const&);

        void onSelect(cocos2d::CCObject*);
    
    public:
        static HorizontalSelectMenu* create(std::vector<const char*> const&);

        void select(unsigned int);
        unsigned int getSelected();
};
