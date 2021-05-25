#pragma once

#include "../offsets.hpp"

class HorizontalSelectMenu : public cocos2d::CCNode {
    public:
        using Callback = std::function<void(unsigned int, bool)>;

    protected:
        cocos2d::extension::CCScale9Sprite* m_pBGLayer;
        std::vector<gd::CCMenuItemToggler*> m_vToggles;
        unsigned int m_nSelIndex = 0u;

        bool m_bIsMulti = false;
        std::vector<bool> m_vMultiStates;

        Callback m_pCallback;

        bool init(std::vector<const char*> const&);

        void onSelect(cocos2d::CCObject*);
        void onMultiSelect(cocos2d::CCObject*);
    
    public:
        static HorizontalSelectMenu* create(std::vector<const char*> const&);
        static HorizontalSelectMenu* createMulti(std::vector<const char*> const&);

        void setCallback(Callback const&);

        std::vector<bool> const& getMultiStates();

        void select(unsigned int);
        unsigned int getSelected();
};
