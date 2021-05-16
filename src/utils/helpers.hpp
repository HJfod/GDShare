#pragma once

#include <filesystem>
#include <fstream>
#include <regex>
#include "gdshare.hpp"

[[nodiscard]]
static std::string readFileString(const std::string & _path) {
    std::ifstream in(_path, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize((const size_t)in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    return "";
}

[[nodiscard]]
static std::vector<uint8_t> readFileBinary(const std::string & _path) {
    std::ifstream in(_path, std::ios::in | std::ios::binary);
    if (in)
        return std::vector<uint8_t> ( std::istreambuf_iterator<char>(in), {});
    return {};
}

static bool saveFileText(const std::string & _path, const std::string & _cont) {
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

static bool saveFileBinary(const std::string & _path, std::vector<uint8_t> const& _bytes) {
    std::ofstream file;
    file.open(_path, std::ios::out | std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(_bytes.data()), _bytes.size());
        file.close();

        return true;
    }
    file.close();
    return false;
}

static constexpr unsigned int h$(const char* str, int h = 0) {
    return !str[h] ? 5381 : (h$(str, h+1) * 33) ^ str[h];
}

static std::string sanitizeString(const std::string & _text, bool _actually_do_it = true) {
    if (_actually_do_it) {
        std::string s = _text;
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
        s = std::regex_replace(s, std::regex("^ +| +$|( ) +"), "$1");
        return s;
    }
    return _text;
}

static std::string decodeCompression(std::string _text, std::string _comp_args) {
    // decode compression if such has been applied
    if (!(_comp_args == "" || _comp_args == "none")) {
        std::vector<uint8_t> decr = gdshare::decoder::Convert(_text);
        std::string comps = _comp_args;

        while (comps.size() > 0) {
            std::string com;
            if (comps.find(";") == std::string::npos) {
                com = comps;
                comps = "";
            } else {
                com = comps.substr(0, comps.find_first_of(";"));
                comps = comps.substr(comps.find_first_of(";") + 1);
            }

            if (com == "") continue;

            int key = 0;
            // check if key has been defined
            if (com.find("-") != std::string::npos) {
                key = std::stoi(com.substr(com.find_last_of("-") + 1));
                com = com.substr(0, com.find_first_of("-"));
            }

            switch (h$(com.c_str())) {
                case h$("gzip"): {
                    decr = gdshare::decoder::GZipX(decr);
                } break;

                case h$("base64"): {
                    decr = gdshare::decoder::Base64X(decr);
                } break;

                case h$("xor"): {
                    decr = gdshare::decoder::XORX(decr, key);
                } break;
            }
        }

        return gdshare::decoder::Convert(decr);
    }

    return _text;
}

static std::string applyCompression(std::string _text, std::string _comp_args) {
    if (!(_comp_args == "" || _comp_args == "none")) {
        std::vector<uint8_t> decr = gdshare::decoder::Convert(_text);
        std::string comps = _comp_args;

        while (comps.size() > 0) {
            std::string com;
            if (comps.find(";") == std::string::npos) {
                com = comps;
                comps = "";
            } else {
                com = comps.substr(comps.find_last_of(";") + 1);
                comps = comps.substr(0, comps.find_last_of(";"));
            }

            if (com == "") continue;

            int key = 0;
            // check if key has been defined
            if (com.find("-") != std::string::npos) {
                key = std::stoi(com.substr(com.find_last_of("-") + 1));
                com = com.substr(0, com.find_first_of("-"));
            }

            switch (h$(com.c_str())) {
                case h$("gzip"): {
                    decr = gdshare::encoder::GZip(decr);
                } break;

                case h$("base64"): {
                    decr = gdshare::encoder::Base64(decr);
                } break;

                case h$("xor"): {
                    decr = gdshare::encoder::XOR(decr, key);
                } break;
            }
        }

        return gdshare::decoder::Convert(decr);
    }

    return _text;
}

static std::string fixPlist(const std::string & _text) {
    return "<plist version=\"1.0\" gjver=\"2.0\"><dict><k>lvl</k>" + _text + "</dict></plist>";
}

