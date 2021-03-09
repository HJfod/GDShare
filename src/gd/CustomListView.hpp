#pragma once

#include "../offsets.hpp"

#pragma runtime_checks("s", off)

class SongObject : public cocos2d::CCNode {
    public:
        virtual cocos2d::CCObject* destructor() {
            return reinterpret_cast<cocos2d::CCObject*(__thiscall*)(
                cocos2d::CCObject*, bool
            )>(
                ModLdr::base + 0x10380
            )(
                this, true
            );
        }

        static SongObject* create(size_t _id) {
            auto obj = new SongObject();

            // what the fuck
            // this is just obj->0x8 = _id
            *reinterpret_cast<size_t*>(reinterpret_cast<std::uintptr_t>(obj) + 0x08) = _id;

            return obj;
        }
};

class StatsObject : public cocos2d::CCNode {
    public:
        virtual cocos2d::CCObject* destructor() {
            return reinterpret_cast<cocos2d::CCObject*(__thiscall*)(
                cocos2d::CCObject*, bool
            )>(
                ModLdr::base + 0x10380
            )(
                this, true
            );
        }

        static StatsObject* create(std::string _text, int _count) {
            return reinterpret_cast<StatsObject*(__fastcall*)(
                std::string, int
            )>(
                ModLdr::base + 0x5daa0
            )(
                _text, _count
            );
        }
};

class CustomListView : public cocos2d::CCLayer {
    public:
        virtual void destructor() {
            return reinterpret_cast<void(__thiscall*)(
                CustomListView*, bool
            )>(
                ModLdr::base + 0x57ec0
            )(
                this, true
            );
        }

        virtual void setupList() {
            reinterpret_cast<void(__fastcall*)(
                cocos2d::CCNode*
            )>(
                ModLdr::base + 0x58870
            )(this);
        };
        virtual cocos2d::CCNode* getListCell(std::string str) {
            return reinterpret_cast<cocos2d::CCNode*(__thiscall*)(
                cocos2d::CCNode*, std::string
            )>(
                ModLdr::base + 0x58050
            )(this, str);
        };
        virtual void loadCell(cocos2d::CCNode* cell, int ix) {
            reinterpret_cast<void(__thiscall*)(
                cocos2d::CCNode*, cocos2d::CCNode*, int
            )>(
                ModLdr::base + 0x585C0
            )(this, cell, ix);
        };

        virtual void another_desctructor_wtf() {
            return reinterpret_cast<void(__thiscall*)(
                CustomListView*, bool
            )>(
                ModLdr::base + 0x658e8
            )(
                this, true
            );
        }

        // TableViewDelegate vftable
        virtual void unknown0() {}
        virtual void unknown1() {}
        virtual void unknown2() {}
        virtual void unknown3() {}
        virtual void unknown4() {}
        virtual void unknown5() {}
        virtual void unknown6() {}

        // TableViewDataSource vftable
        virtual void unknown0_0() {}
        virtual bool unknown1_0() { return true; }
        virtual void unknown2_0() {}
        virtual bool unknown3_0() { return true; }

        static CustomListView* create(cocos2d::CCArray* _arr, float _w, float _h, int _n) {
            
            // size of the listview is passed in xmm1 & xmm2
            
            __asm {
                movss xmm1, [_h]
                movss xmm2, [_w]
            }

            auto ret = reinterpret_cast<CustomListView*(__thiscall*)(cocos2d::CCArray*, intptr_t)>(
                ModLdr::base + 0x57f90
            )(_arr, _n);

            // fix stack
            __asm add esp, 0x4

            return ret;
        }

        static CustomListView* create_(cocos2d::CCArray* _arr, float _w, float _h, int _n) {
            __asm {
                movss xmm1, [_h]
                movss xmm2, [_w]
            }

            auto clv = new CustomListView();

            if (clv && clv->init(_arr, _w, _h, 0x0, _n)) {
                clv->autorelease();
                return clv;
            }

            CC_SAFE_DELETE(clv);
            return nullptr;
        }

        bool init(cocos2d::CCArray* _arr, float _w, float _h, intptr_t _idk, int _n) {

            // width and height are stored in xmm registers
            __asm {
                movss xmm2, [_h]
                movss xmm3, [_w]
            }
            
            std::cout << "dbg0\n";

            auto ret = reinterpret_cast<bool(__thiscall*)(
                cocos2d::CCNode*, cocos2d::CCArray*, intptr_t, int
            )>(
                ModLdr::base + 0x10C20
            )(
                this, _arr, _idk, _n
            );

            // clean stack
            __asm add esp, 0xA
            
            std::cout << "dbg1\n";

            return ret;
        }

        CustomListView() {
            reinterpret_cast<void(__thiscall*)(CustomListView*)>(
				ModLdr::base + 0x57e60
			)(this);

        }

        void constructor() {
            reinterpret_cast<void(__thiscall*)(CustomListView*)>(
				ModLdr::base + 0x57e60
			)(this);
        }

        ~CustomListView() {
            reinterpret_cast<void(__thiscall*)(CustomListView*, bool)>(
				ModLdr::base + 0x57ec0
			)(this, true);
        }
};
#pragma runtime_checks("s", restore)

class InheritedView : public CustomListView {
    public:
        inline virtual void loadCell(cocos2d::CCNode*, int ix) override {
            std::cout << ix << "\n";
        }

        static InheritedView* create(cocos2d::CCArray* _a, float _w, float _h, int _bt) {
            InheritedView* pRet = new InheritedView();
            // InheritedView is inherited from CustomListView

            if (pRet && pRet->init(_a, _w, _h, 0x0, _bt)) {
                std::cout << "yoot\n";
                
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};

