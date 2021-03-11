#include "GDSM.hpp"
#include "gd/GJGameLevel.hpp"
#include "gd/ButtonSprite.hpp"
#include "gd/FLAlertLayer.hpp"
#include "gd/CCMenuItemToggler.hpp"
#include "gd/pugixml.hpp"
#include "gd/CCTextInputNode.hpp"
#include "gdshare.hpp"
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include <fstream>
#include <filesystem>
#include <direct.h>

// for SHOpenFolderAndSelectItems
#include <Shlobj.h>

static constexpr const char* allowedPathChars =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/\\.:-_,;'=%&";

static std::string workdir() {
    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

namespace GameLevelManager {
    void* (__stdcall* sharedState)(void);
    GJGameLevel* (__stdcall* createNewLevel)(void);

    bool load() {
        sharedState = reinterpret_cast<decltype(sharedState)>(
            base + 0x9f860
        );

        createNewLevel = reinterpret_cast<decltype(createNewLevel)>(
            base + 0xa0db0
        );

        return true;
    }
}

struct OptionInfo {
    cocos2d::CCMenu* menu;
    unsigned int* ref;
    unsigned int index;
    int ID;
};

class HorizontalOptionMenu : public cocos2d::CCMenu {
    public:
        static const int oTag = 99;

        void onSelect(cocos2d::CCObject* pSender) {
            auto oi = reinterpret_cast<OptionInfo*>(
                reinterpret_cast<cocos2d::CCNode*>(pSender)->getUserData()
            );

            *oi->ref = oi->index;

            for (unsigned int i = 0; i < oi->menu->getChildrenCount(); i++) {
                auto t = getChild<cocos2d::CCNode*>(oi->menu, i);

                if (t->getTag() == oi->ID)
                    dynamic_cast<CCMenuItemToggler*>(t)->toggle(false);
            }
        }

        static bool populate(
            cocos2d::CCMenu* _menu,
            std::vector<const char*> _opts,
            unsigned int* _ref,
            cocos2d::CCPoint _pos = { 0, 0 },
            float _scale = 1.0f
        ) {
            float padding = 4.0f;
            float height = 0.0f;
            float width = 0.0f;
            float bwidth = 0.0f;

            auto pathBG = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
            pathBG->setScale(.5f);
            pathBG->setColor({ 0, 0, 0 });
            pathBG->setOpacity(75);

            unsigned int off = _menu->getChildrenCount();

            unsigned int ix = 0;
            for (auto opt : _opts) {
                auto optSelect = CCMenuItemToggler::createWithScale(
                    _menu,
                    (cocos2d::SEL_MenuHandler)&HorizontalOptionMenu::onSelect,
                    _scale
                );

                auto label = cocos2d::CCLabelBMFont::create(opt, "bigFont.fnt");
                label->setScale(.55f);

                bwidth = optSelect->getScaledContentSize().width;

                label->setPosition(
                    _pos.x +
                    width +
                    label->getScaledContentSize().width / 2 +
                    bwidth / 2 +
                    padding,
                    _pos.y
                );

                optSelect->setUserData(new OptionInfo {
                    _menu,
                    _ref,
                    ix,
                    oTag
                });
                optSelect->setTag(oTag);

                optSelect->setPosition(
                    _pos.x + width,
                    _pos.y
                );

                width += bwidth * 1.5f +
                         label->getScaledContentSize().width + 
                         padding;

                optSelect->toggle(*_ref == ix);

                _menu->addChild(optSelect, 50);
                _menu->addChild(label, 50);
                
                height = optSelect->getScaledContentSize().height;

                ix++;
            }

            for (unsigned int i = 0; i < _opts.size() * 2; i++) {
                auto c = getChild<cocos2d::CCNode*>(_menu, off + i);
                c->setPositionX(
                    c->getPositionX() - (width - padding - (bwidth / 2) * _opts.size()) / 2
                );
            }

            //_menu->alignItemsHorizontallyWithPadding(padding);

            pathBG->setPosition(_pos.x, _pos.y);
            pathBG->setContentSize({ width * 2 + 20.0f, height * 4 });
            _menu->addChild(pathBG);

            return true;
        }

        static bool populate(
            cocos2d::CCMenu* _menu,
            std::vector<const char*> _opts,
            unsigned int* _ref,
            float _scale = 1.0f
        ) {
            return populate(_menu, _opts, _ref, { 0, 0 }, _scale);
        }

        bool init(std::vector<const char*> _opts, float _scale) {
            if (!cocos2d::CCMenu::init())
                return false;
            
            return populate(this, _opts, nullptr, _scale);
        }

        static HorizontalOptionMenu* create(std::vector<const char*> _opts, float _scale = 1.0f) {
            HorizontalOptionMenu* menu = new HorizontalOptionMenu();

            if (menu && menu->init(_opts, _scale)) {
                menu->setAnchorPoint({ 0, 0 });
                menu->autorelease();
                return menu;
            }

            CC_SAFE_DELETE(menu);
            return nullptr;
        }
};

class ExportSuccessDialog : public FLAlertLayerProtocol {
    const char* m_path;

    public:
        virtual void FLAlert_Clicked(FLAlertLayer*, bool btn2) override {
            if (btn2) {
                ITEMIDLIST *pidl = ILCreateFromPathA(this->m_path);

                if (pidl) {
                    SHOpenFolderAndSelectItems(pidl,0,0,0);
                    ILFree(pidl);
                }
            }
        }

        ExportSuccessDialog(const char* _path) {
            this->m_path = _path;
        }
};

class ExportSettingsLayer : public FLAlertLayer {
    protected:
        static const int inpTag = 198;

        void onExport(cocos2d::CCObject* _obj) {
            auto tuple = reinterpret_cast<std::tuple<GJGameLevel*, ExportSettingsLayer*>*>(
                reinterpret_cast<cocos2d::CCNode*>(_obj)->getUserData()
            );
            
            auto inp = std::get<1>(*tuple)->m_pButtonMenu->getChildByTag(inpTag);
            if (inp)
                outputPath = reinterpret_cast<gd::CCTextInputNode*>(
                    inp
                )->m_pTextField->getString();

            auto lvl = std::get<0>(*tuple);

            // making a copy cuz we don't want to add
            // the filename to outputPath
            std::string path = outputPath;

            if (std::filesystem::exists(path))
                if (std::filesystem::is_directory(path)) {
                    path += "\\" + lvl->levelName + "." +
                        gdshare::filetypes::typemap[selectedFormat];
                }
            
            std::string song;

            if (lvl->songID)
                song = "<k>k45</k><i>" + std::to_string(lvl->songID) + "</i>";
            else
                if (lvl->audioTrack)
                    song = "<k>k8</k><i>" + std::to_string(lvl->audioTrack) + "</i>";

            // jesus fucking christ

            std::stringstream data;

            {
                data
                << "<d>"
                    << "<k>kCEK</k>"
                    << "<i>4</i>"
                    << "<k>k2</k>"
                    << "<s>" << lvl->levelName << "</s>"
                    << "<k>k3</k>"
                    << "<s>" <<
                        gdshare::decoder::Convert(
                            gdshare::encoder::Base64(
                                gdshare::decoder::Convert(
                                    lvl->levelDesc
                                )
                            )
                        )
                    << "</s>"
                    << "<k>k4</k>"
                    << "<s>" <<
                        gdshare::decoder::Convert(
                            gdshare::encoder::Base64(
                                gdshare::encoder::GZip(
                                    gdshare::decoder::Convert(
                                        lvl->levelString
                                    )
                                )
                            )
                        )
                    << "</s>"
                    << song
                    << "<k>k13</k>"
                    << "<t/>"
                    << "<k>k21</k>"
                    << "<i>2</i>"
                    << "<k>k50</k>"
                    << "<i>35</i>"
                << "</d>";
            }

            if (gdshare::saveFile(path, data.str(), selectedFormat))
                FLAlertLayer::create(
                    new ExportSuccessDialog(path.c_str()),
                    "Exported",
                    "OK", "Show file",
                    280.0,
                    "Succesfully <cl>exported</c> to <cy>" + path + "</c>!"
                )->show();
            
            else
                FLAlertLayer::create(
                    nullptr,
                    "<cx>Error exporting</c>",
                    "OK", nullptr,
                    "An unknown <cr>error</c> occurred while exporting :("
                )->show();
            
            std::get<1>(*tuple)->close();
        }

        void onFormatInfo(cocos2d::CCObject*) {
            FLAlertLayer::create(
                nullptr,
                "Info", "OK", nullptr,
                300.0f,
                "You can export levels for multiple different level sharing programs.\n\n" \
                "<cy>.gmd2</c> is <cl>recommended</c>, unless your receiver is using <co>GDShare</c>.\n\n" \
                "<cy>.gmd2</c> is for <cg>GDShare 2</c>\n" \
                "<cy>.gmd</c> is for <co>GDShare</c>\n"    \
                "<cy>.lvl</c> is for <cr>LvlShare</c>\n"   \
            )->show();
        }

        void onSelectPath(cocos2d::CCObject* pSender) {
            auto input = reinterpret_cast<gd::CCTextInputNode*>(
                reinterpret_cast<cocos2d::CCNode*>(pSender)->getUserData()
            );
                
            nfdchar_t* path = nullptr;
            nfdresult_t res = NFD_PickFolder(nullptr, &path);

            if (res == NFD_OKAY) {
                std::string p (path);

                input->m_pTextField->setString(p.c_str());
                input->refreshLabel();

                free(path);
            }
        }

        virtual bool init(GJGameLevel* _lvl) {
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            auto lrSize  = cocos2d::CCSize { 320.0f, 220.0f };

            if (!this->initWithColor({ 0, 0, 0, 75 })) return false;
            this->m_pLayer = cocos2d::CCLayer::create();
            this->addChild(this->m_pLayer);

            auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
            bg->setContentSize(lrSize);
            bg->setPosition(winSize.width / 2, winSize.height / 2);
            this->m_pLayer->addChild(bg);

            this->m_pButtonMenu = cocos2d::CCMenu::create();
            this->m_pLayer->addChild(this->m_pButtonMenu);



            auto title = cocos2d::CCLabelBMFont::create(
                ("Export " + _lvl->levelName).c_str(), "goldFont.fnt"
            );
            title->setScale(.8f);
            title->setPosition(lrSize.width / 2, lrSize.height - 20);
            bg->addChild(title);

            auto targetText = cocos2d::CCLabelBMFont::create("Format:", "bigFont.fnt");
            targetText->setScale(.5f);
            targetText->setPosition(0, 60);
            this->m_pButtonMenu->addChild(targetText);

            auto formatInfoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            formatInfoSpr->setScale(.65f);

            auto formatInfo = CCMenuItemSpriteExtraGD::create(
                formatInfoSpr,
                this->m_pButtonMenu,
                (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onFormatInfo
            );

            formatInfo->setPosition(
                targetText->getScaledContentSize().width / 2 + 15.0f,
                targetText->getPosition().y
            );

            this->m_pButtonMenu->addChild(formatInfo);

            HorizontalOptionMenu::populate(
                this->m_pButtonMenu,
                { ".gmd2", ".gmd", ".lvl" },
                &this->selectedFormat,
                { 0, 28 },
                .65f
            );

            auto exportSpr = ButtonSprite::create("Export", 0, 0, "bigFont.fnt", "GJ_button_01.png", 0.0f, .8f);
            exportSpr->setScale(.75f);

            auto exportButton = CCMenuItemSpriteExtraGD::create(
                exportSpr,
                this->m_pButtonMenu,
                (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onExport
            );

            exportButton->setPositionY(-80);

            this->m_pButtonMenu->addChild(exportButton);


            auto pathBG = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });

            pathBG->setScale(.5f);
            pathBG->setColor({ 0, 0, 0 });
            pathBG->setOpacity(75);
            pathBG->setPosition(0, -40);
            pathBG->setContentSize({ lrSize.width * 2 - 80.0f, 60.0f });

            this->m_pButtonMenu->addChild(pathBG);

            auto pathInput = gd::CCTextInputNode::create(
                "Path to export the level to",
                nullptr,
                "chatFont.fnt",
                lrSize.width - 60.0f,
                30.0f
            );

            pathInput->setTag(inpTag);

            pathInput->m_pTextField->setString(outputPath.c_str());

            pathInput->setLabelPlaceholderColor({ 120, 180, 255 });
            pathInput->setLabelPlaceholerScale(.8f);
            pathInput->setAllowedChars(allowedPathChars);
            pathInput->setPositionY(-40);

            this->m_pButtonMenu->addChild(pathInput);


            auto pathText = cocos2d::CCLabelBMFont::create("Output folder:", "bigFont.fnt");
            pathText->setScale(.5f);
            pathText->setPosition(0, -10);
            this->m_pButtonMenu->addChild(pathText);

            auto pathFolderSpr = cocos2d::CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
            pathFolderSpr->setScale(.65f);

            auto pathFolder = CCMenuItemSpriteExtraGD::create(
                pathFolderSpr,
                this->m_pButtonMenu,
                (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onSelectPath
            );

            pathFolder->setUserData(pathInput);

            pathFolder->setPosition(
                pathText->getScaledContentSize().width / 2 + 15.0f,
                pathText->getPosition().y
            );

            this->m_pButtonMenu->addChild(pathFolder);

            exportButton->setUserData(new std::tuple<GJGameLevel*, ExportSettingsLayer*>(
                _lvl, this
            ));



            auto closeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
            closeSpr->setScale(.8f);

            auto closeBtn = CCMenuItemSpriteExtraGD::create(
                closeSpr,
                this->m_pButtonMenu,
                (cocos2d::SEL_MenuHandler)&ExportSettingsLayer::onClose
            );
            closeBtn->setUserData(reinterpret_cast<void*>(this));

            this->m_pButtonMenu->addChild(closeBtn);

            closeBtn->setPosition(
                - lrSize.width / 2,
                lrSize.height / 2
            );

            this->setKeypadEnabled(true);
            this->setTouchEnabled(true);

            return true;
        };

        void onClose(cocos2d::CCObject* pSender) {
            auto layer = reinterpret_cast<ExportSettingsLayer*>(
                reinterpret_cast<cocos2d::CCNode*>(pSender)->getUserData()
            );

            auto inp = layer->m_pButtonMenu->getChildByTag(inpTag);
            if (inp)
                outputPath = reinterpret_cast<gd::CCTextInputNode*>(
                    inp
                )->m_pTextField->getString();

            layer->close();
        };

        void close() {
            if (this->callback_ != nullptr)
                this->callback_();
            
            this->setKeyboardEnabled(false);
            this->removeFromParentAndCleanup(true);
        }

        std::function<bool()> callback_;

    public:
        static std::string outputPath;
        static unsigned int selectedFormat;

        static ExportSettingsLayer* create(
            GJGameLevel* _name,
            std::function<bool()> _cb = nullptr
        ) {
            auto pRet = new ExportSettingsLayer();

            if (pRet && pRet->init(_name)) {
                pRet->callback_ = _cb;
                pRet->autorelease();
                return pRet;
            }

            CC_SAFE_DELETE(pRet);
            return nullptr;
        };
};

unsigned int ExportSettingsLayer::selectedFormat = 1;
std::string ExportSettingsLayer::outputPath = workdir();

class dumbass_xml {
    std::string* the_fucking_data;
    std::vector<std::tuple<const char*, std::string*>> stupid_keys_to_get;
    bool aight_fuck_it_goodbye_im_leaving;

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void blah_blah_stfu_lets_move_on(const size_t _size) {
        *this->the_fucking_data = this->the_fucking_data->substr(_size);
    };

    void skippity_skip_your_whitespace_is_dumb() {
        auto it_first_nonspace = find_if(
            this->the_fucking_data->begin(),
            this->the_fucking_data->end(),
            [](unsigned char ch) {
                return !std::isspace(ch);
            }
        );

        // e.g. number of blank characters to skip
        const size_t chars_to_skip = it_first_nonspace - this->the_fucking_data->begin();

        // e.g. trim leading blanks
        blah_blah_stfu_lets_move_on(chars_to_skip);
    }

    void well_get_on_to_the_next_key_already() {
        if (this->the_fucking_data->find("<") == std::string::npos) {
            this->aight_fuck_it_goodbye_im_leaving = true;
            return;
        }

        this->blah_blah_stfu_lets_move_on(this->the_fucking_data->find_first_of("<") + 1);

        this->skippity_skip_your_whitespace_is_dumb();

        if (this->the_fucking_data->starts_with("k")) {
            this->skippity_skip_your_whitespace_is_dumb();
            this->blah_blah_stfu_lets_move_on(this->the_fucking_data->find_first_of(">") + 1);

            std::string key = this->the_fucking_data->substr(
                0,
                this->the_fucking_data->find_first_of("<")
            );

            rtrim(key);

            unsigned int ix = 0;
            for (auto s : this->stupid_keys_to_get)
                if (std::string(std::get<0>(s)) == key) {
                    this->blah_blah_stfu_lets_move_on(this->the_fucking_data->find_first_of(">") + 1);
                    this->blah_blah_stfu_lets_move_on(this->the_fucking_data->find_first_of(">") + 1);

                    // fuck you we're not cleaning up whitespace
                    // go do it yourself

                    auto end_of_this_dumb_data = this->the_fucking_data->find_first_of("<");

                    *std::get<1>(s) = this->the_fucking_data->substr(0, end_of_this_dumb_data);

                    // + 1 to skip the <
                    this->blah_blah_stfu_lets_move_on(end_of_this_dumb_data + 1);

                    this->stupid_keys_to_get.erase(
                        this->stupid_keys_to_get.begin() + ix
                    );
                } else ix++;
        }
    }

    public:
        dumbass_xml(std::string* const& _str) {
            this->the_fucking_data = _str;
            this->aight_fuck_it_goodbye_im_leaving = false;
        }

        void try_to_find_this_fucker_pls(const char* _key, std::string* const& _ref) {
            this->stupid_keys_to_get.push_back({ _key, _ref });
        }

        void well_bitch_go_the_fuck_ahead() {
            while (!aight_fuck_it_goodbye_im_leaving)
                this->well_get_on_to_the_next_key_already();
        }

        void ok_now_gtfo() {
            delete this->the_fucking_data;
        }
};

class EditLevelLayer : public cocos2d::CCLayer {
    static inline bool (__thiscall* init)(EditLevelLayer*, GJGameLevel*);
    static bool __fastcall initHook(EditLevelLayer* _self, void*, GJGameLevel* _lvl) {
        auto res = init(_self, _lvl);

        cocos2d::CCArray* children = _self->getChildren();

        cocos2d::CCMenu* m = ((cocos2d::CCMenu*)children->objectAtIndex(14));

        auto exportButton = CCMenuItemSpriteExtraGD::create(
            cocos2d::CCSprite::create("BE_Export_File.png"),
            m,
            (cocos2d::SEL_MenuHandler)&EditLevelLayer::exportLevel
        );
        exportButton->setUserData(_lvl);

        auto folderButton = getChild<CCMenuItemSpriteExtraGD*>(
            m, m->getChildrenCount() - 1
        );
        auto moveUpButton = getChild<cocos2d::CCNode*>(
            m, m->getChildrenCount() - 2
        );

        auto xOffset = getChild<cocos2d::CCNode*>(
            m, 0
        )->getPositionX();
        auto yOffset =  moveUpButton->getPositionY()
                        - getChild<cocos2d::CCNode*>(
                            m, m->getChildrenCount() - 3
                        )->getPositionY();
        
        exportButton->setPosition(
            xOffset,
            moveUpButton->getPositionY() + yOffset
        );

        m->removeChild(folderButton, false);
        m->addChild(exportButton);
        m->addChild(folderButton);

        _self->removeChild((cocos2d::CCNode*)children->objectAtIndex(2), true);

        return res;
    }
    
    void exportLevel(cocos2d::CCObject* pSender) {
        auto lvl = reinterpret_cast<GJGameLevel*>(
            reinterpret_cast<cocos2d::CCNode*>(pSender)->getUserData()
        );
        
        ExportSettingsLayer::create(
            lvl
        )->show();
    }

    public:
        static EditLevelLayer* __fastcall create(GJGameLevel* _lvl) {
            return reinterpret_cast<EditLevelLayer* (__fastcall*)(GJGameLevel*)>(
                base + 0x6f530
            )(
                _lvl
            );
        }

        static MH_STATUS load() {
            return MH_CreateHook(
                (PVOID)(base + 0x6f5d0),
                (LPVOID)EditLevelLayer::initHook,
                (LPVOID*)&EditLevelLayer::init
            );
        }
};

class LevelBrowserLayer : public cocos2d::CCLayer {
    static inline bool (__thiscall* init)(cocos2d::CCLayer*, uintptr_t);
    static bool __fastcall initHook(cocos2d::CCLayer* _self, void*, uintptr_t _array_probs) {
        auto res = init(_self, _array_probs);

        unsigned int screen_id = *reinterpret_cast<uintptr_t*>(_array_probs + 0xEC);

        if (screen_id == 0x62) {
            cocos2d::CCMenu* btnMenu = getChild<cocos2d::CCMenu*>(
                _self, 8
            );

            auto importButton = CCMenuItemSpriteExtraGD::create(
                cocos2d::CCSprite::create("BE_Import_File.png"),
                btnMenu,
                (cocos2d::SEL_MenuHandler)&LevelBrowserLayer::importLevel
            );

            cocos2d::CCNode* newBtn = getChild<cocos2d::CCNode*>(btnMenu, 0);

            btnMenu->addChild(importButton);

            importButton->setPosition(newBtn->getPositionX(), newBtn->getPositionY() + 60.0f);
        }

        return res;
    }

    void importLevel(cocos2d::CCObject* pSender) {
        nfdchar_t* path = nullptr;
        const nfdchar_t* filter = "gmd,gmd2,lvl";
        nfdresult_t res = NFD_OpenDialog(filter, nullptr, &path);

        if (res == NFD_OKAY) {
            auto lvl = gdshare::decodeFile(path);

            if (lvl.length() > 0) {
                auto addedLevel = GameLevelManager::createNewLevel();

                std::string name = "Unnamed",
                            desc = "",
                            data = "",
                            song = "",
                            songCustom = "";

                auto d = new dumbass_xml(&lvl);

                d->try_to_find_this_fucker_pls("k2", &name);
                d->try_to_find_this_fucker_pls("k3", &desc);
                d->try_to_find_this_fucker_pls("k4", &data);
                d->try_to_find_this_fucker_pls("k8", &song);
                d->try_to_find_this_fucker_pls("k45", &songCustom);

                d->well_bitch_go_the_fuck_ahead();

                d->ok_now_gtfo();

                delete d;   // haha get fucked

                if (data.starts_with("H4sIAAAAAAA"))
                    data = gdshare::decoder::GZip(
                        gdshare::decoder::Base64(
                            data
                        )
                    );

                if (song.length())
                    addedLevel->audioTrack = std::stoi(song);
                else if (songCustom.length())
                    addedLevel->songID = std::stoi(songCustom);

                if (desc.length())
                    desc = gdshare::decoder::Base64(desc);

                addedLevel->levelName = name;
                addedLevel->levelDesc = desc;
                addedLevel->levelString = data;

                auto scene = cocos2d::CCScene::create();

                auto layer = EditLevelLayer::create(addedLevel);

                scene->addChild(layer);

                cocos2d::CCDirector::sharedDirector()->replaceScene(
                    cocos2d::CCTransitionFade::create(0.5f, scene)
                );
            } else
                FLAlertLayer::create(
                    nullptr,
                    "<cx>Error importing</c>",
                    "OK", nullptr,
                    "An <cr>error</c> occurred while importing" \
                    "(<cl>lvl</c> is <cy>nullptr</c>, likely <cg>XML</c> is incorrect)"
                )->show();

            free(path);
        }
    }

    public:
        static MH_STATUS load() {
            return MH_CreateHook(
                (PVOID)(base + 0x15a040),
                (LPVOID)LevelBrowserLayer::initHook,
                (LPVOID*)&LevelBrowserLayer::init
            );
        }
};

bool GDShare::live::init() {
    if (MH_Initialize() != MH_OK)
        return false;

    #ifdef GDCONSOLE
    ModLdr::console::load();
    #endif

    GameLevelManager::load();

    MH_STATUS s;
    if ((s = EditLevelLayer::load()) != MH_OK)
        return false;

    if ((s = LevelBrowserLayer::load()) != MH_OK)
        return false;

    return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
}

bool GDShare::live::unload() {
    #ifdef GDCONSOLE
    ModLdr::console::unload();
    #endif

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    return true;
}

#ifdef GDCONSOLE
void GDShare::live::awaitUnload() {
    std::string inp;
    getline(std::cin, inp);

    if (inp != "e")
        GDShare::live::awaitUnload();
}
#endif


