#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mlg
{

class Shader
{
public:
    Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
   ~Shader();

    Shader&   use();
    unsigned& getID();

    // Utility functions
    void    setFloat    (const char* name, float value, bool useShader = false);
    void    setInteger  (const char* name, int value, bool useShader = false);
    void    setVector2f (const char* name, float x, float y, bool useShader = false);
    void    setVector3f (const char* name, float x, float y, float z, bool useShader = false);
    void    setVector4f (const char* name, float x, float y, float z, float w, bool useShader = false);
    void    setMat4     (const char* name, const glm::mat4& matrix, bool useShader = false);
protected:
    void    compile(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode = nullptr);
    void    checkCompileErrors(unsigned object, const std::string& type);
private:
    unsigned m_shaderProgramID;
};

} // namespace mlg

#endif // SHADER_H
