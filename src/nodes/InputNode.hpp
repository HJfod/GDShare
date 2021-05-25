#pragma once

#include "../offsets.hpp"

class InputNode : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_pBG;
        gd::CCTextInputNode* m_pInput;

        bool init(float, float, const char*, const char*, const std::string &, int);
        bool init(float, const char*, const char*, const std::string &, int);
        bool initMulti(float, float, const char*, const char*, const std::string &, int);

    public:
        static InputNode* create(
            float width,
            const char* placeholder,
            const char* fontFile,
            const std::string & filter,
            int limit
        );
        static InputNode* create(
            float width,
            const char* placeholder,
            const std::string & filter,
            int limit
        );
        static InputNode* create(
            float width,
            const char* placeholder,
            const std::string & filter
        );
        static InputNode* create(
            float width,
            const char* placeholder,
            const char* fontFile
        );
        static InputNode* create(
            float width,
            const char* placeholder
        );

        // don't remember if this even works lol
        // the second float is height, rest are
        // the same as normal
        static InputNode* createMulti(float, float, const char*, const char*, const std::string &, int);
        static InputNode* createMulti(float, float, const char*, const char*, int);

        gd::CCTextInputNode* getInputNode();

        void setString(const char*);
        const char* getString();
};