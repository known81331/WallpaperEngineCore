#include "netgame4mt/RenderEngine.hpp"
#include "netgame4mt/NetGameWorkspace.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

#include "gui/imgui_impl_glfw.h"

GLFWwindow* window = nullptr;

void apphide(GLFWwindow* wnd, void* metalDevice) ;

std::string GetBundleFilePath(const std::string& filename);

int main( int argc, char* argv[] )
{

    NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
  //  ImGui::GetIO().Fonts->AddFontDefault();
    
    if (!glfwInit())
        return 0;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    //glfwWindowHint( GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE );
   // glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
   // glfwWindowHint( GLFW_FOCUS_ON_SHOW, GLFW_FALSE );

    window = glfwCreateWindow(mode->width, mode->height, "wallpaper64", NULL, NULL);
    
    auto* _pDevice = MTL::CreateSystemDefaultDevice();
    apphide(window, (void*)_pDevice);

    
    RenderEngine::initSingleton();
    MTLRenderer::initSingleton();
    MTLRenderer::singleton()->init(_pDevice->retain());
    RenderEngine::singleton()->init();
    
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::GetIO().Fonts->AddFontFromFileTTF(GetBundleFilePath("cn.ttf").data(), 16.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());

    void tahoe_create(MTL::Device*);
    tahoe_create(_pDevice);
    
    CNetGame_Workspace workspace = {};
    workspace.create();

    while (!glfwWindowShouldClose(window)) {
        workspace.update();
        
        void tahoe_update();
        tahoe_update();
        
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();
        RenderEngine::singleton()->camera.update();
        RenderEngine::singleton()->frame();
    }
    
    pAutoreleasePool->release();

    return 0;
}
