#pragma once

#include "../offsets.hpp"

class CCMenuItemToggler : public cocos2d::CCMenuItem {
    public:
        static CCMenuItemToggler* create(
            cocos2d::CCSprite* _spr_off,
            cocos2d::CCSprite* _spr_on,
            cocos2d::CCMenu* _parent,
            cocos2d::SEL_MenuHandler _click
        ) {
            auto ret = reinterpret_cast<CCMenuItemToggler*(__fastcall*)(
                cocos2d::CCSprite*,
                cocos2d::CCSprite*,
                cocos2d::CCMenu*,
                cocos2d::SEL_MenuHandler
            )>(
                (uintptr_t)GetModuleHandleA(0) + 0x19600
            )(
                _spr_off,
                _spr_on,
                _parent,
                _click
            );

            __asm { add esp, 0x8 }

            return ret;
        }

        static CCMenuItemToggler* create(
            cocos2d::CCMenu* _parent,
            cocos2d::SEL_MenuHandler _click
        ) {
            return CCMenuItemToggler::create(
                cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
                cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
                _parent,
                _click
            );
        }

        static CCMenuItemToggler* createWithScale(
            cocos2d::CCMenu* _parent,
            cocos2d::SEL_MenuHandler _click,
            float _scale
        ) {
            auto sprOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
            auto sprOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

            sprOn->setScale(_scale);
            sprOff->setScale(_scale);

            return CCMenuItemToggler::create(
                sprOn,
                sprOff,
                _parent,
                _click
            );
        }

        void toggle(bool _toggle) {
            reinterpret_cast<void(__thiscall*)(
                CCMenuItemToggler*, bool
            )>(
                (uintptr_t)GetModuleHandleA(0) + 0x199b0
            )(
                // for some god knows reason true = not toggled
                this, !_toggle
            );
        }

        bool isToggled() {
            return *reinterpret_cast<bool*>(
                reinterpret_cast<uintptr_t>(this) + 0x114
            );
        }

        static CCMenuItemToggler* createWithText(
            const char* _text,
            cocos2d::CCMenu* _parent,
            bool _is_toggled,
            cocos2d::SEL_MenuHandler _click,
            cocos2d::CCPoint _pos
        ) {
            auto toggle = CCMenuItemToggler::create(
                cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
                cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
                _parent,
                _click
            );

            toggle->setPosition(_pos);

            toggle->toggle(_is_toggled);

            auto text = cocos2d::CCLabelBMFont::create(_text, "bigFont.fnt");

            text->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft);

            text->setPositionY(_pos.y); 

            text->setScale(.5);

            auto twidth = text->getScaledContentSize().width;

            text->setPositionX(_pos.x + twidth / 2 + 20);

            _parent->addChild(text);

            return toggle;
        }
};