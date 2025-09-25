/*
 *
 * Copyright 2022 Apple Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cassert>
#include <vector>

#define UI_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <UIKit/UIKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_METAL_CPP
#include "../gui/imgui.h"
#include "../gui/imgui_impl_metal.h"

#include "../netgame4mt/RenderEngine.hpp"
#include "../netgame4mt/NetGameWorkspace.hpp"


#include <unordered_map>
#include <set>

struct touch_t {
    float x, y;
    int phase;
};
touch_t touches[12], otouches[12];
void TouchHandler_handleTouch(int idx, float x, float y, int phase) {
    touch_t t;
    t.x = x;
    t.y = y;
    t.phase = phase;
    
    touches[0] = t;
}

void ReloadFrameBuffers() {
    RenderEngine::singleton()->destroyFramebuffers();
    RenderEngine::singleton()->createFramebuffers();
    
}
void* touchableMTKView(void* pframe, void* pdevice, void* pdelegate);
void* touchableViewController();
int metalLayerWidth();
int metalLayerHeight();


#pragma region Declarations {


class MyMTKViewDelegate : public MTK::ViewDelegate
{
    public:
        MyMTKViewDelegate( MTL::Device* pDevice );
        virtual ~MyMTKViewDelegate() override;
        virtual void drawInMTKView( MTK::View* pView ) override;
};

class MyAppDelegate : public UI::ApplicationDelegate
{
    public:
        ~MyAppDelegate();

        bool applicationDidFinishLaunching( UI::Application *pApp, NS::Value *options ) override;
        void applicationWillTerminate( UI::Application *pApp ) override;

    private:
        UI::Window* _pWindow;
        UI::ViewController* _pViewController;
        MTK::View* _pMtkView;
        MTL::Device* _pDevice;
        MyMTKViewDelegate* _pViewDelegate = nullptr;
};

#pragma endregion Declarations }

int main( int argc, char* argv[] )
{
    NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    MyAppDelegate del;
    UI::ApplicationMain(argc, argv, &del);

    pAutoreleasePool->release();

    return 0;
}

#pragma mark - AppDelegate
#pragma region AppDelegate {

MyAppDelegate::~MyAppDelegate()
{
    _pMtkView->release();
    _pWindow->release();
    _pViewController->release();
    _pDevice->release();
    delete _pViewDelegate;
}



std::string GetBundleFilePath(const std::string& filename);

MTK::View* __GLOBAL_pView;
void* currentDrawable() {
    return __GLOBAL_pView->currentDrawable();
}
bool MyAppDelegate::applicationDidFinishLaunching( UI::Application *pApp, NS::Value *options )
{
   // SDL_SetMainReady();
   // SDL_Init(SDL_INIT_EVENTS);
    CGRect frame = UI::Screen::mainScreen()->bounds();
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
   // io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;  // Enable Keyboard Controls
    io.DisplaySize = ImVec2(frame.size.width, frame.size.height);
    io.DisplayFramebufferScale = ImVec2(1.f, 1.f);
    ImGui::GetIO().Fonts->AddFontFromFileTTF(GetBundleFilePath("cn.ttf").data(), 16.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
    

    _pWindow = UI::Window::alloc()->init(frame);
    

    _pViewController = (UI::ViewController*)touchableViewController(); //UI::ViewController::alloc()->init( nil, nil );

    _pDevice = MTL::CreateSystemDefaultDevice();
    _pViewDelegate = new MyMTKViewDelegate( _pDevice );
    
    _pMtkView = (MTK::View*)touchableMTKView( &frame, _pDevice, _pViewDelegate);
    _pMtkView->setDelegate( _pViewDelegate );
    
    
    io.DisplaySize = ImVec2(_pMtkView->drawableSize().width, _pMtkView->drawableSize().height);

    UI::View *mtkView = (UI::View *)_pMtkView;
    mtkView->setAutoresizingMask( UI::ViewAutoresizingFlexibleWidth | UI::ViewAutoresizingFlexibleHeight );
    _pViewController->view()->addSubview( mtkView );
    _pWindow->setRootViewController( _pViewController );

    __GLOBAL_pView = _pMtkView;
    
    
    _pWindow->makeKeyAndVisible();
    
    // Setup style
    ImGui::StyleColorsDark();
    
    io.FontGlobalScale = 2.f;
    ImGui::GetStyle().ScaleAllSizes(2.f);
    
  //  ImGui_ImplMetal_Init(_pDevice);
    
    
    ImGui::GetIO().MouseDrawCursor = true;
    return true;
}

void MyAppDelegate::applicationWillTerminate( UI::Application *pApp )
{
}

#pragma endregion AppDelegate }


#pragma mark - ViewDelegate
#pragma region ViewDelegate {


CNetGame_Workspace workspace = {};

MyMTKViewDelegate::MyMTKViewDelegate( MTL::Device* pDevice )
: MTK::ViewDelegate()
{
    RenderEngine::initSingleton();
    MTLRenderer::initSingleton();
    MTLRenderer::singleton()->init(pDevice->retain());
    RenderEngine::singleton()->init();
    
    void tahoe_create(MTL::Device*);
    tahoe_create(pDevice);
    
    workspace.create();

    
}

MyMTKViewDelegate::~MyMTKViewDelegate()
{
}


int touch_idx = 0;

void MyMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
    ImVec2 anchor = {};
   // for (int i = 0; i < 12; i++)
    {
        auto& touch = touches[0];
        if (touch.phase != 2) {
            ImGui::GetIO().AddMousePosEvent(touch.x, touch.y);

            if (touch.phase == 0) {
                ImGui::GetIO().AddMouseButtonEvent(0, true);
                anchor = {touch.x, touch.y};
            }
        }
        else {
            ImGui::GetIO().AddMouseButtonEvent(0, false);
        }
    }
    
    
    void tahoe_update();
    tahoe_update();
    
    
    workspace.update();
    RenderEngine::singleton()->camera.update();
    RenderEngine::singleton()->frame();
    
}

void touch_controls() {
    
}

#pragma endregion ViewDelegate }

