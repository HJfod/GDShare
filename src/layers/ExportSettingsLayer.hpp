#pragma once

#include "../offsets.hpp"
#include "../nodes/BrownAlertDelegate.hpp"
#include "../nodes/HorizontalSelectMenu.hpp"
#include "../nodes/InputNode.hpp"

class ExportResultHandler : public gd::FLAlertLayerProtocol {
    protected:
        std::string m_sPath;

        virtual void FLAlert_Clicked(gd::FLAlertLayer*, bool) override;

    public:
        ExportResultHandler(std::string const&);
};

class ExportSettingsLayer : public BrownAlertDelegate {
    protected:
        gd::GJGameLevel* m_pLevel;
        HorizontalSelectMenu* m_pHorizontalMenu;
        InputNode* m_pPathInput;

        void setup() override;

        void onClose(cocos2d::CCObject*);
        void onSelectPath(cocos2d::CCObject*);
        void onInfo(cocos2d::CCObject*);
        void onExport(cocos2d::CCObject*);

    public:
        static ExportSettingsLayer* create(gd::GJGameLevel*);
};

