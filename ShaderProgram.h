#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include <cstdint>
#include <GL/glew.h>
#include <map>
#include <functional>
#include <glm/glm.hpp>

class UniformVariable
{
public:
    UniformVariable();
    explicit UniformVariable(GLint id);
    UniformVariable(const UniformVariable &rhs);
    float operator=(float);
    uint32_t operator=(uint32_t);
    int32_t operator=(int32_t);
    glm::uvec2 operator=(const glm::uvec2 &);
    glm::vec2 operator=(const glm::vec2 &);
    glm::vec3 operator=(const glm::vec3 &);
    glm::vec4 operator=(const glm::vec4 &);
    glm::mat4 operator=(const glm::mat4 &);
    UniformVariable &operator=(const UniformVariable &rhs)=default;
    bool valid() const;
private:
    GLint m_id;
};

class Program
{
public:
    Program()=default;
    Program(const Program &)=default;
    static Program LoadFromFile(const std::string &filename);
    static Program LoadFromFile(const std::string &vs, const std::string &fs);
    UniformVariable &operator[](const std::string &);

    bool valid() const;
    void use() const;

    void dispatchCompute(uint32_t x, uint32_t y, uint32_t z) const;

    void release();
private:
    Program(GLuint prog);
    std::map<std::string, UniformVariable> m_uniformVariables;
    GLuint m_program;
};

#endif