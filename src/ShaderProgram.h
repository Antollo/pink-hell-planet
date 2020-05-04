#ifndef SHADER_H_
#define SHADER_H_

#include <cassert>
#include <fstream>
#include <map>
#include <list>
#include <iterator>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "error.h"

class ShaderProgram
{
public:
    static void init()
    {
        glGenBuffers(1, &matrices);

        glBindBuffer(GL_UNIFORM_BUFFER, matrices);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrices, 0, 2 * sizeof(glm::mat4));
    }
    ShaderProgram() : loaded(false) {}
    void load(std::string vertexShaderFilename, std::string fragmentShaderFilename)
    {
        assert(matrices != 0);
        assert(loaded == false);
        shaderProgram = glCreateProgram();
        vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFilename);
        fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        link(shaderProgram);
        use();

        GLuint id = glGetUniformBlockIndex(shaderProgram, "matrices");
        glUniformBlockBinding(shaderProgram, id, 0);

        loaded = true;
    }
    ~ShaderProgram()
    {
        if (loaded)
        {
            glDetachShader(shaderProgram, vertexShader);
            glDetachShader(shaderProgram, fragmentShader);

            glDeleteShader(vertexShader);
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
    static void setMatrixP(const glm::mat4 &matrix)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(matrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    static void setMatrixV(const glm::mat4 &matrix)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void use() const
    {
        assert(loaded == true);
        glUseProgram(shaderProgram);
    }

private:
    static inline GLuint matrices = 0;
    bool loaded;
  
    GLuint vertexShader, fragmentShader, shaderProgram;
    static void setMatrixGlobal(GLsizei offset, const float *data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
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
};

#endif /* !SHADER_H_ */
