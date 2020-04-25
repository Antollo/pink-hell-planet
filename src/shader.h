#ifndef SHADER_H_
#define SHADER_H_

#include <fstream>
#include <glad/glad.h>
#include "error.h"

class ShaderProgram
{
public:
    void load(std::string vertexShaderFilename, std::string fragmentShaderFilename)
    {
        shaderProgram = glCreateProgram();
        vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFilename);
        fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        link(shaderProgram);
        glUseProgram(shaderProgram);
    }
    ~ShaderProgram()
    {
        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
    }
    void uniformMatrix4fv(const char *name, const float *matrix)
    {
        GLuint location = glGetUniformLocation(shaderProgram, name);
        glUniformMatrix4fv(location, 1, false, matrix);
    }

private:
    GLuint vertexShader, fragmentShader, shaderProgram;
    GLuint loadShader(const GLenum type, std::string fileName)
    {
        std::ifstream file(fileName);
        if (!file.good())
            errorAndExit("File ", fileName, " not good");
        std::string source((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        if (source.empty())
            errorAndExit("File ", fileName, " empty");

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
            glGetShaderInfoLog(shader, logLength, NULL, &log[0]);
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
            glGetProgramInfoLog(program, logLength, nullptr, &log[0]);
            errorAndExit(log);
        }

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

        if (!status)
        {
            error("Program validation error");
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(logLength, ' ');
            glGetProgramInfoLog(program, logLength, nullptr, &log[0]);
            errorAndExit(log);
        }
    }
};

#endif /* !SHADER_H_ */
