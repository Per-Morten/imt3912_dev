#pragma once

#include <nox/app/graphics/2d/Camera.h>
#include <nox/app/graphics/2d/TransformationNode.h>
#include <nox/event/Event.h>
#include <nox/log/Logger.h>
#include <nox/window/RenderSdlWindowView.h>

/*
 * This window view inherits from the nox::window::RenderSdlWindowView.
 * The nox::window::RenderSdlWindowView manages a window with SDL2 and runs a nox::app::graphics::OpenGlRenderer
 * to render things to the screen.
 */
class WindowView final
    : public nox::window::RenderSdlWindowView
{
public:
    /**
     * Create an example window view.
     * @param applicationContext The context that it is created in. This is needed to create a logger and pass it down
     * the inheritance tree.
     * @param windowTitle The tilte that will appear on top of the window.
     */
    WindowView(nox::app::IContext* applicationContext, const std::string& windowTitle);

private:
    void onRendererCreated(nox::app::graphics::IRenderer* renderer) override;
    void onEvent(const std::shared_ptr<nox::event::Event>& event) override;

    bool initialize(nox::logic::IContext* context) override;
    void onWindowSizeChanged(const glm::uvec2& size) override;
    void onKeyPress(const SDL_KeyboardEvent& event) override;

    nox::log::Logger log;
    nox::event::ListenerManager listener;

    nox::app::graphics::IRenderer* renderer;

    // The camera decides how to project the content on the screen. This is an orthographic projection.
    std::shared_ptr<nox::app::graphics::Camera> camera;

    // The root scene node is where all other scene nodes are attached for rendering.
    std::shared_ptr<nox::app::graphics::TransformationNode> rootSceneNode;
};

