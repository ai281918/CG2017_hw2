#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderProgram.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Axis.h"
#include <vector>

std::vector<Program> progs;

static void error_callback(int error, const char* description)
{
    std::cerr<<description<<"\n";
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
        else if (key == GLFW_KEY_1 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{0, 0};
            }
            puts("Flat-Phong");
        }
        else if (key == GLFW_KEY_2 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{0, 1};
            }
            puts("Flat-Blinn_Phong");
        }
        else if (key == GLFW_KEY_3 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{0, 2};
            }
            puts("Flat-Lambert");
        }
        else if (key == GLFW_KEY_4 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{1, 0};
            }
            puts("Gouraud-Phong");
        }
        else if (key == GLFW_KEY_5 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{1, 1};
            }
            puts("Gouraud-Blinn_Phong");
        }
        else if (key == GLFW_KEY_6 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{1, 2};
            }
            puts("Gouraud-Lambert");
        }
        else if (key == GLFW_KEY_7 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{2, 0};
            }
            puts("Phong-Phong");
        }
        else if (key == GLFW_KEY_8 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{2, 1};
            }
            puts("Phong-Blinn_Phong");
        }
        else if (key == GLFW_KEY_9 && action == GLFW_PRESS){
            for(int i=0;i<progs.size();++i){
                progs[i]["opt"] = glm::vec2{2, 2};
            }
            puts("Phong-Lambert");
        }
}

glm::mat4 translate(glm::mat4 model, glm::vec3 t)
{
    glm::mat4 t_mat = glm::mat4(1.0);
    t_mat[3][0] = t.x;
    t_mat[3][1] = t.y;
    t_mat[3][2] = t.z;

    return model*t_mat;
}

glm::mat4 scale(glm::mat4 model, glm::vec3 t)
{
    glm::mat4 t_mat = glm::mat4(1.0);
    t_mat[0][0] = t.x;
    t_mat[1][1] = t.y;
    t_mat[2][2] = t.z;

    return model*t_mat;
}

glm::mat4 rotateX(glm::vec3 v, int n)
{
    glm::mat4 r = glm::mat4(1.0);
    float d = glm::sqrt(v.y*v.y + v.z*v.z);
    if(d == 0) return r;

    r[1][1] = v.z/d;
    r[2][1] = -v.y/d * n;
    r[1][2] = v.y/d * n;
    r[2][2] = v.z/d;

    return r;
}

glm::mat4 rotateY(glm::vec3 v, int n)
{
    glm::mat4 r = glm::mat4(1.0);
    float d = glm::sqrt(v.y*v.y + v.z*v.z);
    
    r[0][0] = d;
    r[0][2] = v.x * n;
    r[2][0] = -v.x * n;
    r[2][2] = d;

    return r;
}

glm::mat4 rotateZ(float degree)
{
    glm::mat4 r = glm::mat4(1.0);
    r[0][0] = glm::cos(degree);
    r[1][0] = -glm::sin(degree);
    r[0][1] = glm::sin(degree);
    r[1][1] = glm::cos(degree);

    return r;
}

glm::mat4 rotate(glm::mat4 model, float degree, glm::vec3 v)
{
    v = glm::normalize(v);
    return model * rotateX(v, -1) * rotateY(v, -1) * rotateZ(degree) * rotateY(v, 1) * rotateX(v, 1);
}

void print_mat4(glm::mat4 m4)
{
    for(int i=0;i<4;++i){
        for(int j=0;j<4;++j){
            printf("%10f", m4[i][j]);
        }
        puts("");
    }
    puts("==================================================");
}

int main(void)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Mac OS X is not allow to use >= 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for Mac OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    auto mesh1 = StaticMesh::LoadMesh("../resource/sun.obj");
    auto mesh2 = StaticMesh::LoadMesh("../resource/earth.obj");
    auto mesh3 = StaticMesh::LoadMesh("../resource/earth.obj");
    auto text = Texture2D::LoadFromFile("../resource/QQ.png");
    auto prog = Program::LoadFromFile("../resource/vs.txt", "../resource/fs.txt");
    auto text2 = Texture2D::LoadFromFile("../resource/earth.png");
    auto prog2 = Program::LoadFromFile("../resource/vs.txt", "../resource/fs.txt");
    auto text3 = Texture2D::LoadFromFile("../resource/sun.png");
    auto prog3 = Program::LoadFromFile("../resource/vs.txt", "../resource/fs.txt");
    
    progs.push_back(prog);
    progs.push_back(prog2);
    progs.push_back(prog3);
	// Remove this line and see the difference
	text.setFilter(FilterMode::eNearestMipmapLinear, FilterMode::eLinear);
    text2.setFilter(FilterMode::eNearestMipmapLinear, FilterMode::eLinear);
    text3.setFilter(FilterMode::eNearestMipmapLinear, FilterMode::eLinear);

	if (!mesh1.hasUV()) {
		std::cerr<<"WARNING: The mesh has no UV data\n";
	}
    if (!mesh2.hasUV()) {
		std::cerr<<"WARNING: The mesh has no UV data\n";
    }

    auto view = glm::lookAt(glm::vec3{20.0f, 20.0f, 20.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    auto proj = glm::perspective(glm::pi<float>()/4, 800.0f/600.0f, 0.1f, 200.f);
    auto vp = proj*view;
    prog["view"] = view;
    prog["proj"] = proj;
    prog["text"] = 0;
    prog["opt"] = glm::vec2{0, 0};
    prog["lightPos"] = glm::vec3{10.0f, 10.0f, 0.0f};
    prog["lightColor"] = glm::vec3{1.0f, 1.0f, 1.0f};
    auto uniform_model_sun = prog["model"];
    prog2["view"] = view;
    prog2["proj"] = proj;
    prog2["text"] = 0;
    prog2["opt"] = glm::vec2{0, 0};
    prog2["lightPos"] = glm::vec3{10.0f, 10.0f, 0.0f};
    prog2["lightColor"] = glm::vec3{1.0f, 1.0f, 1.0f};
    auto uniform_model_earth = prog2["model"];
    prog3["view"] = view;
    prog3["proj"] = proj;
    prog3["text"] = 0;
    prog3["opt"] = glm::vec2{0, 0};
    prog3["lightPos"] = glm::vec3{10.0f, 10.0f, 0.0f};
    prog3["lightColor"] = glm::vec3{1.0f, 1.0f, 1.0f};
    auto uniform_model_moon = prog3["model"];

    prog.use();
    auto test = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    print_mat4(test);
    test = rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    print_mat4(test);

    Axis axis(7.0f);
    glEnable(GL_DEPTH_TEST);

    Line l;
    l.set(glm::vec3(1, 5, 0), glm::vec3(5, 5, 5));
    while (!glfwWindowShouldClose(window))
    {
        // draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        axis.draw(vp);
        l.draw(vp);

        prog.use();
        auto model = rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 0.0f));
        uniform_model_sun = model;
        text.bindToChannel(0);
        mesh1.draw();

        prog2.use();
        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::translate(model, glm::vec3(15, 0, 0));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        uniform_model_earth = model;
        text2.bindToChannel(0);
        mesh2.draw();
        
        prog3.use();
        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, -1.0f, 1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, -1.0f, -1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, -1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3+3.14f, glm::vec3(0.0f, -1.0f, 1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3+3.14f, glm::vec3(0.0f, -1.0f, -1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3+3.14f, glm::vec3(0.0f, 1.0f, 1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();

        model = rotate(glm::mat4(1.0), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, glm::vec3(15, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3+3.14f, glm::vec3(0.0f, 1.0f, -1.0f));
        model = translate(model, glm::vec3(2, 0, 0));
        model = rotate(model, static_cast<float>(glfwGetTime())*3, glm::vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        uniform_model_moon = model;
        text3.bindToChannel(0);
        mesh3.draw();
        ////////////////
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    mesh1.release();
    prog.release();
    text.release();
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
