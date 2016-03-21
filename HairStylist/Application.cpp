#include "Application.h"
#include "Input.h"
#include <GL/glew.h>
#include "Mesh.h"
#include "Shader.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "Texture.h"
#include "math.h"

Application::Application(const std::string& title, int width, int height)
    :m_title(title)
{
    m_window = std::make_unique<Window>(width, height);
    m_window->setTitle(title);

    m_painterFBO = std::make_unique<Framebuffer>(1024, 1024, true);
    resize(width, height);

    m_saveHairstyleManager = std::make_unique<HairstyleManager>("hairstyle", "Save", "save.info");
    m_presetHairstyleManager = std::make_unique<HairstyleManager>("preset", "Presets", "preset.info");

    m_dirLight.ambient = glm::vec3(0.f);
    m_dirLight.diffuse = glm::vec3(1.f);
    m_dirLight.specular = glm::vec3(1.f);
    m_dirLight.direction = glm::normalize(glm::vec3(0.f, -0.5f, -1.f));

    m_modelMaterial.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_modelMaterial.specular = glm::vec4(0.15f, 0.15f, 0.15f, 64.0f);

    m_hairMaterial.diffuse = glm::vec3(0.8f);
    m_hairMaterial.specular = glm::vec4(0.5f, 0.5f, 0.5f, 64.0f);
}

Application::Application()
    :Application("Hello World", 1000, 500)
{
}

Application::~Application()
{
}

void Application::run()
{
    Input::subscribe(this);
 
    m_modelShader.load("Assets/Shaders/model.vert", "Assets/Shaders/model.frag");
    m_painterOverlayShader.load("Assets/Shaders/painterOverlay.vert", "Assets/Shaders/painterOverlay.frag");
    m_quadShader.load("Assets/Shaders/quad.vert", "Assets/Shaders/quad.frag");
    m_hairShader.load("Assets/Shaders/hair.vert", "Assets/Shaders/hair.frag", "Assets/Shaders/hair.geom");

    m_quadMesh.loadQuad();
    m_modelTexture.load("Assets/Textures/AngelinaFaceDiffuse.png");
    m_brushTexture.load("Assets/Textures/Brush.png");
    m_modelMesh.load("Assets/Mesh/AngelinaHeadVB.raw", "Assets/Mesh/AngelinaHeadIB.raw");

    m_painterCamera.setPosition(0.f, 0.f, 1.0f);

    m_modelCamera.setPosition(0.f, 0.0f, 10.f);
    m_modelCamera.lookAt(glm::vec3(0.f, 0.f, 0.f));

    glEnable(GL_SCISSOR_TEST);

    clear();

    while (m_running)
    {
        m_paintingAllowed = false;
        Input::update(m_window->getHeight(), true);
        Time::update();

        if (m_paused)
            continue;

        m_painterCamera.updateViewMatrix();
        m_modelCamera.updateViewMatrix();

#ifdef DEVELOP
        showFPS();
#endif

        updatePainterView();
        updateModelView();

        m_window->flip();
    }
}
    
void Application::onQuit()
{
    m_running = false;
}

void Application::onMousewheel(float delta)
{
    onViewFocus();

    if (m_painterFocus)
        m_brushIntensity = math::clamp(m_brushIntensity + delta * m_brushIntensityInc, 0.0f, 1.0f);
    else
    {
        if (Input::isKeyDown(SDL_SCANCODE_LSHIFT) || Input::isKeyDown(SDL_SCANCODE_RSHIFT))
            m_activeHairstyle.width = math::clamp(m_activeHairstyle.width + delta * m_hairWidthInc, 1.f, 5.f);
        else
            m_modelCamera.zoom(-delta * m_zoomInc);
    }
}

void Application::onKeyDown(SDL_Keycode keyCode)
{
    switch (keyCode)
    {
    case SDLK_0:
        m_activeHairstyle.color = glm::vec3(1.f, 1.f, 1.f);
        break;
    case SDLK_1:
        m_activeHairstyle.color = glm::vec3(0.f, 0.f, 0.f);
        break;
    case SDLK_2:
        m_activeHairstyle.color = glm::vec3(1.f, 0.f, 0.f);
        break;
    case SDLK_3:
        m_activeHairstyle.color = glm::vec3(0.f, 1.f, 0.f);
        break;
    case SDLK_4:
        m_activeHairstyle.color = glm::vec3(0.f, 0.f, 1.f);
        break;
    case SDLK_5:
        m_activeHairstyle.color = glm::vec3(1.f, 1.f, 0.f);
        break;
    case SDLK_6:
        m_activeHairstyle.color = glm::vec3(1.f, 0.f, 1.f);
        break;
    case SDLK_7:
        m_activeHairstyle.color = glm::vec3(0.f, 1.f, 1.f);
        break;
    case SDLK_8:
        m_activeHairstyle.color = glm::vec3(0.6f, 0.3f, 0.f);
        break;
    case SDLK_9:
        m_activeHairstyle.color = glm::vec3(0.2f, 0.0f, 0.4f);
        break;
    case SDLK_l:
        m_activeColor = 0;
        break;
    case SDLK_t:
        m_activeColor = 1;
        break;
    case SDLK_b:
        m_activeColor = 2;
        break;
    case SDLK_F1:
#ifdef DEVELOP
        m_hairShader.load("Assets/Shaders/hair.vert", "Assets/Shaders/hair.frag", "Assets/Shaders/hair.geom");
        m_modelShader.load("Assets/Shaders/model.vert", "Assets/Shaders/model.frag");
#endif
        break;
    case SDLK_F5:
    case SDLK_s:
        m_saveHairstyleManager->save(m_activeHairstyle, *m_painterFBO.get());
        break;
    case SDLK_F9:
    case SDLK_x:
        m_saveHairstyleManager->loadRecent(*m_painterFBO.get(), m_activeHairstyle);
        break;
    case SDLK_KP_PLUS:
    case SDLK_PLUS:
        if (m_painterFocus)
            m_brushScale = std::min(1.0f, m_brushScale + m_brushScaleInc);
        else
            m_activeHairstyle.length += m_hairLengthInc;
        break;
    case SDLK_KP_MINUS:
    case SDLK_MINUS:
        if (m_painterFocus)
            m_brushScale = std::max(m_minBrushScale, m_brushScale - m_brushScaleInc);
        else
            m_activeHairstyle.length = std::max(0.0f, m_activeHairstyle.length - m_hairLengthInc);
        break;
    case SDLK_r:
        if (m_painterFocus)
            clear();
        break;
    case SDLK_c:
        if (m_painterFocus)
            clear(m_activeColor == 0, m_activeColor == 1, m_activeColor == 2);
        break;
    case SDLK_d:
        if (m_painterFocus)
            m_showOverlay = !m_showOverlay;
        break;
    case SDLK_LEFT:
        m_presetHairstyleManager->loadPrev(*m_painterFBO.get(), m_activeHairstyle);
        break;
    case SDLK_n:
    case SDLK_RIGHT:
        m_presetHairstyleManager->loadNext(*m_painterFBO.get(), m_activeHairstyle);
        break;
    case SDLK_UP:
        m_saveHairstyleManager->loadNext(*m_painterFBO.get(), m_activeHairstyle);
        break;
    case SDLK_DOWN:
        m_saveHairstyleManager->loadPrev(*m_painterFBO.get(), m_activeHairstyle);
        break;
    default:
        break;
    }
}

void Application::onWindowEvent(const SDL_WindowEvent& windowEvent)
{
    switch (windowEvent.event) 
    {
    case SDL_WINDOWEVENT_RESIZED:
        resize(windowEvent.data1, windowEvent.data2);
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
        m_paused = true;
        break;
    case SDL_WINDOWEVENT_RESTORED:
        m_paused = false;
        break;
    }
}

void Application::onMouseDown(const SDL_MouseButtonEvent& e)
{
    onViewFocus();

    if (e.button == SDL_BUTTON_LEFT || e.button == SDL_BUTTON_RIGHT)
        m_paintingAllowed = true;
}

void Application::onMouseMotion(const SDL_MouseMotionEvent& e)
{
    m_paintingAllowed = Input::isDragging();
}

void Application::resize(int width, int height)
{
    m_painterCamera.setOrthographic(float(width), float(height), 0.1f, 100.f);
    m_painterCamera.setViewport(0.f, 0.f, width * 0.5f, float(height));

    m_modelCamera.setPerspective(45.0f, float(width), float(height), 0.1f, 100.f);
    m_modelCamera.setViewport(width * 0.5f, 0.f, width * 0.5f, float(height));

    m_window->resize(width, height);
}

void Application::showFPS()
{
    static float left = 0.f;
    static uint32_t fps = 0;
    left -= Time::deltaTime;

    if (left <= 0.f)
    {
        float frameTime = 1000.f / fps;
        m_window->setTitle(m_title + " FPS: " + std::to_string(fps) + " Frame time: " + std::to_string(frameTime) + "ms/frame");

        left = 1.f;
        fps = 0;
    }

    fps++;
}

void Application::updatePainterView()
{
    glDisable(GL_DEPTH_TEST);

    if (m_paintingAllowed)
        paint();

    setViewport(m_painterCamera.getViewport());

    renderColorLayers();

    // Do not render the brush if the user is rotating the model 
    // and ends up in the painter view with the mouse
    if (m_painterFocus || !Input::isDragging())
        renderBrush();

    if (m_showOverlay)
        renderPainterOverlay();
}

void Application::updateModelView()
{
    updateModelRotation();

    setViewport(m_modelCamera.getViewport());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render model
    m_modelShader.bind();
    glm::mat4 view = m_modelCamera.view();
    glm::mat4 proj = m_modelCamera.proj();
    m_modelShader.bind();
    m_modelShader.setCamera(view, proj);
    m_modelShader.setModel(glm::toMat4(m_modelRotation));
    m_modelShader.setDirLight(m_dirLight, view);
    m_modelShader.setMaterial(m_modelMaterial);
    m_modelShader.bindTexture2D(m_modelTexture, "u_hairTexture");
    m_modelShader.bindTexture2D(m_painterFBO->getRenderTexture(), "u_hairTexture", 1);
    m_modelMesh.render();

    // Hair pass
    glLineWidth(m_activeHairstyle.width);
    m_hairShader.bind();
    m_hairShader.setFloat("u_hairLength", m_activeHairstyle.length);
    m_hairShader.setVec3("u_hairColor", m_activeHairstyle.color);
    m_hairShader.setCamera(view, proj);
    m_hairShader.setDirLight(m_dirLight, view);
    m_hairShader.setMaterial(m_hairMaterial);
    m_hairShader.setModel(glm::toMat4(m_modelRotation));
    m_hairShader.bindTexture2D(m_painterFBO->getRenderTexture(), "u_hairTexture");
    m_modelMesh.render();
    glLineWidth(1.0f);
}

void Application::updateModelRotation()
{
    if (m_painterFocus || !Input::leftDrag().isDragging())
    {
        m_modelRotationBeforeDrag = m_modelRotation;
        return;
    }

    if (glm::length2(Input::leftDrag().getDragDeltaToStart()) == 0.0f)
        return;

    // Simple arcball model rotation based on: https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
    glm::vec3 startPos = glm::vec3(Input::leftDrag().getStartDragPos(), 0.0f);
    glm::vec3 ndcStart = m_modelCamera.viewportToNDC(m_modelCamera.screenToViewportPoint(startPos));
    glm::vec3 curPos = glm::vec3(Input::leftDrag().getCurDragPos(), 0.0f);
    glm::vec3 ndcEnd = m_modelCamera.viewportToNDC(m_modelCamera.screenToViewportPoint(curPos));

    glm::vec3 toStart = computeArcballVector(ndcStart);
    glm::vec3 toEnd = computeArcballVector(ndcEnd);

    float angle = 2.0f * std::acosf(std::min(1.0f, glm::dot(toStart, toEnd)));
    glm::vec3 rotationAxis = glm::normalize(glm::cross(toStart, toEnd));
    m_modelRotation = glm::angleAxis(angle, rotationAxis) * m_modelRotationBeforeDrag;
}

glm::vec3 Application::computeArcballVector(glm::vec3 ndcPos)
{
    // Radius of the ball
    float r = 1.5f;

    // Use pythagorean theorem to compute z
    glm::vec3 v = ndcPos;
    v.z = 0.0f;
    float sq = v.x * v.x + v.y * v.y;
    if (sq <= r * r)
        v.z = std::sqrtf(r * r - sq);

    return glm::normalize(v);
}

void Application::renderPainterOverlay()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_painterOverlayShader.bind();
    m_modelMesh.bindAndRender();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
}

void Application::renderBrush()
{
    glEnable(GL_BLEND);
    // Invert intensity if holding left shift
    float intensity = (Input::isKeyDown(SDL_SCANCODE_LSHIFT) || Input::isKeyDown(SDL_SCANCODE_RSHIFT)) ? 1.0f - m_brushIntensity : m_brushIntensity;

    // Erase if using right mouse button
    if (Input::rightDrag().isDragging())
        intensity = m_activeColor == 0 ? 0.0f : 0.5f;

    glBlendColor(0.f, 0.f, 0.0f, intensity);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_SRC_COLOR);

    m_quadShader.bind();
    glm::vec3 brushPos = m_painterCamera.viewportToWorldPoint(m_painterCamera.screenToViewportPoint(Input::mousePosition));
    glm::mat4 model = glm::translate(brushPos) * glm::scale(glm::vec3(m_brushScale));
    m_quadShader.setMVP(m_painterCamera.viewProj() * model);

    m_quadShader.bindTexture2D(m_brushTexture, "u_textureDiffuse");
    m_quadMesh.bindAndRender();
    glDisable(GL_BLEND);
}

void Application::renderColorLayers()
{
    m_quadShader.bind();
    glm::mat4 model = glm::translate(glm::vec3(0.5f, 0.5f, 0.0f));
    m_quadShader.setMVP(m_painterCamera.viewProj() * model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_painterFBO->getRenderTexture());
    m_quadMesh.bindAndRender();
}

void Application::paint()
{
    if (!m_painterFocus)
        return;

    m_painterFBO->begin();
    glColorMask(m_activeColor == 0, m_activeColor == 1, m_activeColor == 2, true);
    renderBrush();
    glColorMask(true, true, true, true);
    m_painterFBO->end();
}

void Application::clear(bool red, bool green, bool blue, bool alpha)
{
    m_painterFBO->begin();
    glColorMask(red, green, blue, alpha);
    glClearColor(0.f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(true, true, true, true);
    m_painterFBO->end();
}

void Application::setViewport(const Rect& rect, bool scissor)
{
    glViewport(GLint(rect.minX()), GLint(rect.minY()), GLsizei(rect.width()), GLsizei(rect.height()));
    if (scissor)
        glScissor(GLint(rect.minX()), GLint(rect.minY()), GLsizei(rect.width()), GLsizei(rect.height()));
}

void Application::onViewFocus()
{
    auto& painterViewport = m_painterCamera.getViewport();
    auto& modelViewport = m_modelCamera.getViewport();

    if (painterViewport.inside(glm::vec2(Input::mousePosition)))
        m_painterFocus = true;
    else if (modelViewport.inside(glm::vec2(Input::mousePosition)))
        m_painterFocus = false;
}
