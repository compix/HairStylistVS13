#pragma once

//#define DEVELOP

#include "Window.h"
#include <memory>
#include "Timer.h"
#include "Input.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include <glm/ext.hpp>
#include "Rect.h"
#include "Framebuffer.h"
#include "HairstyleManager.h"
#include "Hairstyle.h"

class Application : public InputHandler
{
    friend HairstyleManager;
public:
    Application();
    Application(const std::string& title, int width, int height);
    ~Application();

    void run();

protected:
    void onQuit() override;
    void onMousewheel(float delta) override;
    void onKeyDown(SDL_Keycode keyCode) override;
    void onWindowEvent(const SDL_WindowEvent& windowEvent) override;

    void onMouseDown(const SDL_MouseButtonEvent& e) override;
    void onMouseMotion(const SDL_MouseMotionEvent& e) override;
private:
    void resize(int width, int height);

    void showFPS();

    void updatePainterView();
    void updateModelView();

    void updateModelRotation();

    void renderPainterOverlay();
    void renderBrush();
    void renderColorLayers();
    void paint();
    void clear(bool red = true, bool green = true, bool blue = true, bool alpha = true);

    void setViewport(const Rect& rect, bool scissor = true);

    void onViewFocus();

    glm::vec3 computeArcballVector(glm::vec3 ndcPos);
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<HairstyleManager> m_saveHairstyleManager;
    std::unique_ptr<HairstyleManager> m_presetHairstyleManager;
    bool m_running{ true };
    bool m_paused{ false };
    bool m_painterFocus{ true };
    bool m_paintingAllowed{ false };
    bool m_showOverlay{ true };
    DirectionalLight m_dirLight;
    Material m_hairMaterial;
    Material m_modelMaterial;

    Timer m_timer;
    std::string m_title;

    Camera m_painterCamera;
    Camera m_modelCamera;

    Shader m_hairShader;
    Shader m_modelShader;
    Texture m_modelTexture;
    Texture m_brushTexture;
    Mesh m_modelMesh;
    glm::quat m_modelRotationBeforeDrag;
    glm::quat m_modelRotation;

    Shader m_painterOverlayShader;
    Shader m_quadShader;

    Mesh m_quadMesh;

    // 0 = Red, 1 = Green, 2 = Blue
    uint8_t m_activeColor{0};
    std::unique_ptr<Framebuffer> m_painterFBO;
    float m_hairLengthInc{ 0.1f };
    float m_hairWidthInc{ 1.0f };
    Hairstyle m_activeHairstyle;

    float m_zoomInc{ 0.1f };

    float m_brushScale{ 0.1f };
    float m_brushScaleInc{ 0.01f };
    float m_minBrushScale{ 0.01f };
    float m_brushIntensity{ 1.0f };
    float m_brushIntensityInc{ 0.1f };
};
