#ifndef GLOBALCONFIG_HPP_
#define GLOBALCONFIG_HPP_

#include <string>

#include "Config.hpp"

class GlobalConfig : private Config
{
public:
    GlobalConfig(std::string fileName) : Config(fileName) {}

    
    int getResX()
    {
        return Config::getInt("res_x");
    }
    int getResY()
    {
        return Config::getInt("rex_y");
    }
    void setResX(int resX)
    {
        Config::setInt(resX, "res_x");
    }
    void setResY(int resY)
    {
        Config::setInt(resY, "res_y");
    }

    int getFullscreen()
    {
        return Config::getBool("fullscreen");
    }
    void setFullscreen(bool fullscreen)
    {
        Config::setBool(fullscreen, "fullscreen");
    }

    enum GraphicSetting
    {
        low,
        normal
    };

    GraphicSetting getGraphicSetting() const
    {
        return stringToGraphicSetting[Config::getString("graphics")];
    }
    void setGraphicSetting(GraphicSetting setting)
    {
        Config::setString(graphicSettingToString[setting], "graphics");
    }

    using Config::flush;

private:
    static inline std::unordered_map<std::string, GraphicSetting> stringToGraphicSetting = {
        { "low", GraphicSetting::low },
        { "normal", GraphicSetting::normal }
    };
    static inline std::array<std::string, 2> graphicSettingToString = {
        "low", "normal"
    };
};


#endif
