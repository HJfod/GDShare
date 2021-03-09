#pragma once

#include "../offsets.hpp"

// thanks pie <3

class GJListLayer;

class GJDropDownLayer : public cocos2d::CCLayerColor {
	protected:
		cocos2d::CCPoint m_obEndPosition;
		cocos2d::CCPoint m_obStartPosition;
		cocos2d::CCMenu* m_pButtonMenu;
		GJListLayer* m_pListLayer;
		bool m_bControllerEnabled; //?
		cocos2d::CCLayer* m_pLayer;
		bool m_bHidden; //?
		PAD(4);

	public:
		//CCNode vtable
		virtual void registerWithTouchDispatcher() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x16990
				)(this);
		}
		virtual void draw() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x16A80
				)(this);
		}

		//CCTouchDelegate vtable
		virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) { return true; }
		virtual void ccTouchMoved(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {}
		virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {}
		virtual void ccTouchCancelled(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {}

		//CCKeypadDelegate vtable
		virtual void keyBackClicked() {
			return reinterpret_cast<void(__thiscall*)(char*)>(
				ModLdr::base + 0x113960
				)(reinterpret_cast<char*>(this) + 0xF4);
		}

		//vtable
		virtual void customSetup() {}
		virtual void enterLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x16970
				)(this);
		}
		virtual void exitLayer() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113980
				)(this);
		}
		virtual void showLayer(bool noTransition) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::base + 0x1139C0
				)(this, noTransition);
		}
		virtual void hideLayer(bool noTransition) {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::base + 0x113A90
				)(this, noTransition);
		}
		virtual void layerVisible() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x16A40
				)(this);
		}
		virtual void layerHidden() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113B60
				)(this);
		}
		virtual void enterAnimFinished() {}
		virtual void disableUI() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113920
				)(this);
		}
		virtual void enableUI() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x113940
				)(this);
		}
		static GJDropDownLayer* create(const char* title) {
			GJDropDownLayer* pRet = new GJDropDownLayer();
			if (pRet) {
				pRet->constructor();
				if (pRet->init(title))
					pRet->autorelease();
				//no idea if this works correctly, destructor calls vtable function with stack cookie or something
				else pRet->destructor();
			}
			return pRet;
		}
		GJDropDownLayer() {
			reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x38470
			)(this);
		}
		void constructor() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*)>(
				ModLdr::base + 0x038470
			)(this);
		}
		void destructor() {
			return reinterpret_cast<void(__thiscall*)(GJDropDownLayer*, bool)>(
				ModLdr::base + 0x038560
			)(this, true);
		}
		bool init(const char* title) {
			// same size as settingslayer
			float x = 220.0;

			// set height (it's stored in xmm2)
			__asm { movss xmm2, [x] }

			return reinterpret_cast<bool(__thiscall*)(GJDropDownLayer*, const char*)>(
				ModLdr::offsets::DropDownLayerInit
			)(this, title);
		}
		bool init(const char* _t, float _s) {
			_asm { movss xmm2, [_s] }

			return reinterpret_cast<bool(__thiscall*)(GJDropDownLayer*, const char*)>(
				ModLdr::offsets::DropDownLayerInit
			)(this, _t);
		}
		static GJDropDownLayer* createWithSize(const char* _t, float _s) {
			GJDropDownLayer* pRet = new GJDropDownLayer();
			if (pRet) {
				pRet->constructor();
				if (pRet->init(_t, _s))
					pRet->autorelease();
				else pRet->destructor();
			}
			return pRet;
		}
		void showLayerB() {
			return this->showLayer(false);
		}
};

