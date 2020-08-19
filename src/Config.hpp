#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <nlohmann/json.hpp>

#include "error.h"

class Config
{
public:
    Config();
    Config(std::string fileName) : file(fileName)
    {
        std::ifstream input(file);
        if (!input.good())
            errorAndExit("Could not open config", file);
        input >> json;
        input.close();
    }

    virtual ~Config()
    {
        std::ofstream output(file);
        output << json.dump(4);
        output.close();
    }

    //TODO error handling
    bool getBool(std::string paramName) const
    {
        return json[paramName];
    }
    int getInt(std::string paramName) const
    {
        return json[paramName];
    }
    std::string getString(std::string paramName) const
    {
        return json[paramName];
    }
    nlohmann::json getJson(std::string paramName) const
    {
        return json[paramName];
    }
    
    void setBool(bool value, std::string paramName)
    {
        json[paramName] = value;
    }
    void setInt(int value, std::string paramName)
    {
        json[paramName] = value;
    }
    void setString(std::string value, std::string paramName)
    {
        json[paramName] = value;
    }
    void setJson(nlohmann::json value, std::string paramName)
    {
        json[paramName] = value;
    }
    
private:
    nlohmann::json json;
    std::string file;
};

#endif
