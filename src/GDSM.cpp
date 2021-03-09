#include "GDSM.hpp"
#include "gd/GJGameLevel.hpp"
#include "gd/ButtonSprite.hpp"
#include "gd/FLAlertLayer.hpp"
#include "gd/pugixml.hpp"
#include "gdshare.hpp"
#include <fstream>
#include <filesystem>

// for SHOpenFolderAndSelectItems
#include <Shlobj.h>

static bool saveFileText(std::string _path, std::string _cont) {
    std::ofstream file;
    file.open(_path);
    if (file.is_open()) {
        file << _cont;
        file.close();

        return true;
    }
    file.close();
    return false;
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
        nfdchar_t* path = nullptr;
        const nfdchar_t* filter = "gmd"; // "gmd2,gmd,lvl;gmd"
        nfdresult_t res = NFD_SaveDialog(filter, nullptr, &path);

        if (res == NFD_OKAY) {
            std::string p (path);
            if (!p.ends_with(".gmd"))
                p += ".gmd";

            auto lvl = reinterpret_cast<GJGameLevel*>(
                reinterpret_cast<cocos2d::CCNode*>(pSender)->getUserData()
            );
            
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
                    << "<k>k13</k>"
                    << "<t/>"
                    << "<k>k21</k>"
                    << "<i>2</i>"
                    << "<k>k50</k>"
                    << "<i>35</i>"
                << "</d>";
            }

            if (saveFileText(p, data.str()))
                FLAlertLayer::create(
                    new ExportSuccessDialog(p.c_str()),
                    "Exported",
                    "OK", "Show file",
                    280.0,
                    "Succesfully <cl>exported</c> to <cy>" + p + "</c>!"
                )->show();
            
            else
                FLAlertLayer::create(
                    nullptr,
                    "<cx>Error exporting</c>",
                    "OK", nullptr,
                    "An unknown <cr>error</c> occurred while exporting :("
                )->show();

            free(path);
        }
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

class LevelBrowserLayer {
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

                auto speed = std::chrono::high_resolution_clock::now();

                auto d = new dumbass_xml(&lvl);

                d->try_to_find_this_fucker_pls("k2", &name);
                d->try_to_find_this_fucker_pls("k3", &desc);
                d->try_to_find_this_fucker_pls("k4", &data);
                d->try_to_find_this_fucker_pls("k8", &song);
                d->try_to_find_this_fucker_pls("k45", &songCustom);

                d->well_bitch_go_the_fuck_ahead();

                d->ok_now_gtfo();

                delete d;   // haha get fucked

                std::cout << "done in " << std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - speed).count() << "ms\n";

                if (data.starts_with("H4sIAAAAAAA"))
                    data = gdshare::decoder::GZip(
                        gdshare::decoder::Base64(
                            data
                        )
                    );

                int songID = 0;

                // todo: add songs to export
                
                if (song.length())
                    songID = std::stoi(song);
                else if (songCustom.length())
                    songID = std::stoi(songCustom);

                desc = gdshare::decoder::Base64(desc);

                addedLevel->levelName = name;
                addedLevel->levelDesc = desc;
                addedLevel->songID = songID;
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


