#pragma once

#include "../offsets.hpp"

namespace gd {

enum SearchType {
    Search          = 0x0,
    MostDownloaded  = 0x1,
    MostLiked       = 0x2,
    Trending        = 0x3,
    Recent          = 0x4,
    UsersLevels     = 0x5,
    Featured        = 0x6,
    Magic           = 0x7,
    MapPacks        = 0x9,
    Awarded         = 0xB,
    Followed        = 0xC,
    Friends         = 0xD,
    FindUsers       = 0xE,
    HallOfFame      = 0x10,
    MyLevels        = 0x62,
    SavedLevels     = 0x63
};

class GJSearchObject : public cocos2d::CCNode {
    protected:
        SearchType m_nScreenID;

    public:
        static GJSearchObject* create(SearchType nID) {
            return reinterpret_cast<GJSearchObject*(__fastcall*)(
                SearchType
            )>(
                gd::base + 0xc2b90
            )( nID );
        }

        SearchType getType() {
            return this->m_nScreenID;
        }
};

}

