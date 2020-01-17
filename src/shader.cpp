#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/shader.h"
using mlg::Shader;

Shader::Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    std::ifstream vertexShaderFile, fragmentShaderFile;
    // ensure ifstream objects can throw exceptions:
    vertexShaderFile.exceptions   (std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vertexShaderFile.open(vertexShaderFilename);
        fragmentShaderFile.open(fragmentShaderFilename);
        std::stringstream vertexShaderStream, fragmentShaderStream;
        // read file's buffer contents into streams
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Compile our shaders
        this->compile(std::string(vertexShaderStream.str()).c_str(), std::string(fragmentShaderStream.str()).c_str());
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}
Shader::~Shader()
{}

Shader& Shader::use()
{
    glUseProgram(this->m_shaderProgramID);
    return *this;
}

void Shader::compile(const char* vertexShaderCode
        , const char* fragmentShaderCode
        , const char* geometryShaderCode /* = nullptr */
        )
{
    unsigned vertex = 0, fragment = 0, geometry = 0;
    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // If geometry shader source code is given, also compile geometry shader
    if (geometryShaderCode != nullptr)
    {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometryShaderCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // Shader Program
    this->m_shaderProgramID = glCreateProgram();
    glAttachShader(this->m_shaderProgramID, vertex);
    glAttachShader(this->m_shaderProgramID, fragment);
    if (geometryShaderCode != nullptr)
        glAttachShader(this->m_shaderProgramID, geometry);

    // Link the shaders
    glLinkProgram(this->m_shaderProgramID);
    checkCompileErrors(this->m_shaderProgramID, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryShaderCode != nullptr)
        glDeleteShader(geometry);
}

unsigned& Shader::getID()
{
    return this->m_shaderProgramID;
}
void Shader::setFloat(const char *name, float value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform1f(glGetUniformLocation(this->m_shaderProgramID, name), value);
}
void Shader::setInteger(const char *name, int value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform1i(glGetUniformLocation(this->m_shaderProgramID, name), value);
}
void Shader::setVector2f(const char *name, float x, float y, bool useShader)
{
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->m_shaderProgramID, name), x, y);
}
void Shader::setVector3f(const char *name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->m_shaderProgramID, name), x, y, z);
}
void Shader::setVector4f(const char *name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->m_shaderProgramID, name), x, y, z, w);
}
void Shader::setMat4(const char* name, const glm::mat4& matrix, bool useShader /* = false */)
{
    if (useShader)
        this->use();
    glUniformMatrix4fv(glGetUniformLocation(this->m_shaderProgramID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::checkCompileErrors(unsigned object, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, nullptr, infoLog);
            std::cerr << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, nullptr, infoLog);
            std::cerr << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}
