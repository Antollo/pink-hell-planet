#ifndef SHADER_H_
#define SHADER_H_

#include <cassert>
#include <fstream>
#include <map>
#include <list>
#include <iterator>
#include <regex>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "error.h"
#include "ParticleGroupLight.h"

class ShaderProgram
{
private:
    static constexpr size_t pOffset = 0;
    static constexpr size_t vOffset = pOffset + sizeof(glm::mat4);
    static constexpr size_t invVOffset = vOffset + sizeof(glm::mat4);
    static constexpr size_t lightsOffset = invVOffset + sizeof(glm::mat4);
    static constexpr size_t colorOffset = lightsOffset + ParticleGroupLight::count * sizeof(ParticleGroupLight);
    static constexpr size_t timeOffset = colorOffset + sizeof(glm::vec4);
    static constexpr size_t alphaOffset = timeOffset + sizeof(float);
    static constexpr size_t dataSize = alphaOffset + sizeof(float);

public:
    static void init()
    {
        glGenBuffers(1, &data);

        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, data, 0, dataSize);
    }
    ShaderProgram() : loaded(false), validated(false), vertexShader(0), geometryShader(0), fragmentShader(0), shaderProgram(0) {}
    void load(std::string vertexShaderFilename, std::string fragmentShaderFilename)
    {
        assert(data != 0);
        assert(loaded == false);
        shaderProgram = glCreateProgram();
        vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFilename);
        fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        link(shaderProgram);
        glUseProgram(shaderProgram);

        GLuint id = glGetUniformBlockIndex(shaderProgram, "data");
        glUniformBlockBinding(shaderProgram, id, 0);

        loaded = true;
    }
    void load(std::string vertexShaderFilename, std::string geometryShaderFilename, std::string fragmentShaderFilename)
    {
        assert(data != 0);
        assert(loaded == false);
        shaderProgram = glCreateProgram();
        vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFilename);
        geometryShader = loadShader(GL_GEOMETRY_SHADER, geometryShaderFilename);
        fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, geometryShader);
        glAttachShader(shaderProgram, fragmentShader);

        link(shaderProgram);
        glUseProgram(shaderProgram);

        GLuint id = glGetUniformBlockIndex(shaderProgram, "data");
        glUniformBlockBinding(shaderProgram, id, 0);

        loaded = true;
    }
    ~ShaderProgram()
    {
        if (loaded)
        {
            glDetachShader(shaderProgram, vertexShader);
            if (geometryShader)
                glDetachShader(shaderProgram, geometryShader);
            glDetachShader(shaderProgram, fragmentShader);

            glDeleteShader(vertexShader);
            if (geometryShader)
                glDeleteShader(geometryShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(shaderProgram);
        }
    }
    void setUniformMatrix4fv(const char *name, const glm::mat4 &matrix) const
    {
        GLuint location = glGetUniformLocation(shaderProgram, name);
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
    }
    void setUniform1i(const char *name, int i) const
    {
        GLuint location = glGetUniformLocation(shaderProgram, name);
        glUniform1i(location, i);
    }
    void setUniform1f(const char *name, float f) const
    {
        GLuint location = glGetUniformLocation(shaderProgram, name);
        glUniform1f(location, f);
    }
    static void setMatrixP(const glm::mat4 &matrix)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, pOffset, sizeof(glm::mat4), glm::value_ptr(matrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    static void setMatrixV(const glm::mat4 &matrix)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, vOffset, sizeof(glm::mat4), glm::value_ptr(matrix));
        glBufferSubData(GL_UNIFORM_BUFFER, invVOffset, sizeof(glm::mat4), glm::value_ptr(glm::inverse(matrix)));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    static void setColor(const glm::vec4 &color)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, colorOffset, sizeof(glm::vec4), glm::value_ptr(color));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    static void setTime(const float &number)
    {
        time = number;
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, timeOffset, sizeof(float), &number);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    static float getTime()
    {
        return time;
    }
    static void setParticleGroupLight(const ParticleGroupLight &light, int index)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, lightsOffset + index * sizeof(ParticleGroupLight), sizeof(ParticleGroupLight), &light);
    }
    static void setAlpha(const float &alpha)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, alphaOffset, sizeof(float), &alpha);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void use() const
    {
        assert(loaded == true);
        glUseProgram(shaderProgram);
    }
    void validate() const
    {
        assert(loaded == true);
        if (!validated)
        {
            validated = true;
            _validate(shaderProgram);
        }
    }

private:
    static inline GLuint data = 0;
    static inline float time = 0.f;
    bool loaded;
    mutable bool validated;

    GLuint vertexShader, geometryShader, fragmentShader, shaderProgram;
    static void setMatrixGlobal(GLsizei offset, const float *matrix)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, data);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), matrix);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    GLuint loadShader(const GLenum type, std::string fileName)
    {
        std::string source(readAll(fileName));

        GLuint shader = glCreateShader(type);
        const GLchar *sourcePtr = source.c_str();
        glShaderSource(shader, 1, &sourcePtr, nullptr);
        glCompileShader(shader);
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            error("Shader compile error");
            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(logLength, ' ');
            glGetShaderInfoLog(shader, logLength, NULL, log.data());
            errorAndExit(log);
        }
        return shader;
    }
    void link(GLuint program)
    {
        glLinkProgram(program);
        GLint status;

        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (!status)
        {
            error("Link program error");
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(logLength, ' ');
            glGetProgramInfoLog(program, logLength, nullptr, log.data());
            errorAndExit(log);
        }
    }
    void _validate(GLuint program) const
    {
        GLint status;

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

        if (!status)
        {
            error("Program validation error");
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(logLength, ' ');
            glGetProgramInfoLog(program, logLength, nullptr, log.data());
            errorAndExit(log);
        }
    }
    void include(std::string &source)
    {
        std::regex regex("#include\\s+\"(\\S+)\"");
        std::smatch results;

        while (std::regex_search(source, results, regex))
        {
            source.replace(results[0].first, results[0].second, readAll(std::string(results[1].first, results[1].second)));
        }
    }
    std::string readAll(const std::string &fileName)
    {
        std::ifstream file(fileName);
        if (!file.good())
            errorAndExit("File ", fileName, " not good");
        std::string source((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        if (source.empty())
            errorAndExit("File ", fileName, " empty");
        
        include(source);
        return source;
    }
};

#endif /* !SHADER_H_ */
