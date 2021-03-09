#pragma once

#include "../GDSM.hpp"

// thanks pie

class CCMenuItemSpriteExtraGD : public cocos2d::CCMenuItem {
    public:
        inline static CCMenuItemSpriteExtraGD* create(
            cocos2d::CCSprite* spr,
            cocos2d::CCMenu* targ,
            cocos2d::SEL_MenuHandler click
        ) {
            auto pRet = reinterpret_cast<CCMenuItemSpriteExtraGD*(__thiscall*)(
                cocos2d::CCSprite*,
                cocos2d::CCMenu*,
                cocos2d::SEL_MenuHandler
            )>(
                base + 0x18EE0
            )(
                spr, targ, click
            );

            __asm add esp, 0x8
            return pRet;
        }

        void setSizeMult(float mult) {
			__asm movss xmm1, mult
			return reinterpret_cast<void(__thiscall*)(CCMenuItemSpriteExtraGD*)>(
				base + 0x19080
			)(this);
		}
};

class ButtonSprite : public cocos2d::CCSprite {
	public:
		/*PARAMS:
		* caption - the button's caption.
		* width - the width of the button. only used if absolute is TRUE.
		* absolute - whether or not to use provided width. if FALSE, game will auto-adjust.
		* font - font file for caption.
		* texture - texture file for button background.
		* height - height of button. put 0 for auto.
		* scale - scale of the caption.
		*/
		inline static ButtonSprite* create(
            const char* caption,
            int width,
            bool absolute,
            const char* font,
            const char* texture,
            float height,
            float scale
        ) {
			//scale is passed in lower 4 bytes of xmm3
			__asm movss xmm3, scale

			//arg 3 is always 0. dunno why it's not optimized out as a param
			auto pRet = reinterpret_cast<ButtonSprite* (__fastcall*)(
                const char*,
                int,
                int,
                bool,
                const char*,
                const char*,
                float
            )>(
                base + 0x137D0
            )(
                caption, width, 0, absolute, font, texture, height
            );
            
			//clean stack before returning
			__asm add esp, 0x14
			return pRet;
		}

        inline static ButtonSprite* create(
            const char* caption,
            int width,
            bool absolute,
            const char* font,
            const char* texture,
            float height,
            float scale,
            float bscale
        ) {
            auto spr = ButtonSprite::create(
                caption, width, absolute, font, texture, height, scale
            );

            spr->setScale(bscale);

			return spr;
		}
};


