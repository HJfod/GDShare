#include "gdshare.hpp"
#include "ext/Base64.hpp"
#include "ext/ZlibHelper.hpp"
#include "ext/zip.h"
#include "ext/json.hpp"
#include <filesystem>
#include <fstream>
#include <regex>

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

static constexpr unsigned int h$(const char* str, int h = 0) {
    return !str[h] ? 5381 : (h$(str, h+1) * 33) ^ str[h];
}

static std::string sanitizeString(std::string _text, bool _actually_do_it = true) {
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

// Decoder

std::vector<uint8_t> gdshare::decoder::Convert(const std::string & _data) {
    return std::vector<uint8_t> (_data.begin(), _data.end());
}

std::string gdshare::decoder::Convert(const std::vector<uint8_t> & _data) {
    return std::string(_data.begin(), _data.end());
}


std::vector<uint8_t> gdshare::decoder::XORX(const std::vector<uint8_t> & _data, int _key) {
    std::vector<uint8_t> copy(_data);

    for (unsigned int ix = 0; ix < copy.size(); ix++)
        copy[ix] ^= _key;

    return copy;
}

std::vector<uint8_t> gdshare::decoder::Base64X(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);
    return b64.decode(_data);
}

std::vector<uint8_t> gdshare::decoder::GZipX(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::inflateBuffer(_data);
}


std::string gdshare::decoder::GZip(const std::string & _data) {
    std::vector<uint8_t> data = gdshare::decoder::Convert(_data);

    return gdshare::decoder::Convert(gdshare::decoder::GZipX(data));
}

std::string gdshare::decoder::Base64(const std::string & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::vector<uint8_t> data = b64.decode(_data);

    return gdshare::decoder::Convert(data);
}

std::string gdshare::decoder::XOR(const std::string & _data, int _key) {
    std::vector<uint8_t> data = gdshare::decoder::Convert(_data);

    for (uint8_t b : data)
        b ^= _key;

    return gdshare::decoder::Convert(data);
}


std::vector<uint8_t> gdshare::encoder::GZip(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::deflateBuffer(_data);
}

std::vector<uint8_t> gdshare::encoder::Base64(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::string data = b64.encode(_data);

    return gdshare::decoder::Convert(data);
}

std::vector<uint8_t> gdshare::encoder::XOR(const std::vector<uint8_t> & _data, int _key) {
    // xor is two-way so i can literally
    // just call the decoder XOR function

    return gdshare::decoder::XORX(_data, _key);
}



std::string gdshare::decodeFile(const std::string & _path) {
    if (!std::filesystem::exists(_path))
        return nullptr;

    std::string type = std::filesystem::path(_path).extension().string();
    // remove .
    type = type.substr(1);

    switch (h$(type.c_str())) {
        case h$(gdshare::filetypes::GDShare2): {
            void* metaBuffer = nullptr;
            void* dataBuffer = nullptr;
            size_t metaBufSize, dataBufSize;

            struct zip_t *zip = zip_open(_path.c_str(), 0, 'r');
            {
                zip_entry_open(zip, "level.meta");
                {
                    zip_entry_read(zip, &metaBuffer, &metaBufSize);
                }
                zip_entry_close(zip);

                zip_entry_open(zip, "level.data");
                {
                    zip_entry_read(zip, &dataBuffer, &dataBufSize);
                }
                zip_entry_close(zip);
            }
            zip_close(zip);

            nlohmann::json metaj = nlohmann::json::parse((const char*)metaBuffer);

            free(metaBuffer);

            std::string compr = metaj["compression"];

            std::string data = (const char*)dataBuffer;

            free(dataBuffer);

            compr = sanitizeString(compr);

            data = decodeCompression(data, compr);

            if (data == "")
                return nullptr;

            return data;
        } break;

        case h$(gdshare::filetypes::GDShare): {
            std::string data = readFileString(_path);

            return data;
        } break;

        case h$(gdshare::filetypes::LvlShare): {
            std::string datac = readFileString(_path);

            std::string data = gdshare::decoder::GZip(datac);

            data = "<d>" + data + "</d>";

            return data;
        } break;

        default:
            return nullptr;
    }

    return nullptr;
}

DS_Dictionary* gdshare::parseFile(const std::string & _path) {
    auto data = decodeFile(_path);
    if (data.length()) {
        data = fixPlist(data);

        auto doc = new DS_Dictionary();

        if (!doc->loadRootSubDictFromString(data))
            return nullptr;

        return doc;
    }

    return nullptr;
}

