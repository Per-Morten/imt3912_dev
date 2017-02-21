#include "window_view.h"

#include <nox/app/graphics/2d/IRenderer.h>
#include <nox/app/graphics/2d/BackgroundGradient.h>
#include <nox/app/IContext.h>
#include <nox/app/resource/Descriptor.h>
#include <nox/logic/IContext.h>
#include <nox/logic/graphics/event/DebugRenderingEnabled.h>
#include <nox/logic/graphics/event/SceneNodeEdited.h>
#include <nox/logic/physics/actor/ActorPhysics.h>
#include <nox/logic/actor/Actor.h>
#include <nox/logic/event/IBroadcaster.h>
#include <nox/logic/physics/Simulation.h>

#include <cmd/parser.h>


WindowView::WindowView(nox::app::IContext* applicationContext, const std::string& windowTitle):
    nox::window::RenderSdlWindowView(applicationContext, windowTitle),
    renderer(nullptr),
    camera(std::make_shared<nox::app::graphics::Camera>(getWindowSize())),
    listener("WindowView")
{
    log = applicationContext->createLogger();
    log.setName("WindowView");

    int width = cmd::g_cmdParser.getIntArgument(cmd::constants::window_width_cmd,
                                                cmd::constants::window_width_default);
    int height = cmd::g_cmdParser.getIntArgument(cmd::constants::window_height_cmd,
                                                 cmd::constants::window_height_default);
    
    setWindowSize({ width, height });

    // We need to listen to SceneNodeEdited events so that we can add graphics to the screen.
    listener.addEventTypeToListenFor(nox::logic::graphics::SceneNodeEdited::ID);

    // One meter should cover 60 pixels.
    camera->setScale({60.0f, 60.0f});

    // This is the root for our scene graph used for rendering.
    rootSceneNode = std::make_shared<nox::app::graphics::TransformationNode>();
}

bool 
WindowView::initialize(nox::logic::IContext* context)
{
    if (RenderSdlWindowView::initialize(context) == false)
    {
        return false;
    }

    listener.setup(this, context->getEventBroadcaster(), nox::logic::event::ListenerManager::StartListening_t());

    return true;
}

/*
 * This is called when the renderer is created and lets us set up the renderer to our preferences.
 */
void 
WindowView::onRendererCreated(nox::app::graphics::IRenderer* renderer)
{
    /*
     * Here we set up the renderer to our preferences.
     */

    assert(renderer != nullptr);

    /*
     * The graphics.json file describes all the texture atlases that must be loaded. It is passed to the renderer
     * so that it can load all of the atlases into graphics memory.
     */
    const auto graphicsResourceDescriptor = nox::app::resource::Descriptor{"graphics/graphics.json"};
    renderer->loadTextureAtlases(graphicsResourceDescriptor, getApplicationContext()->getResourceAccess());

    /*
     * The renderer has to have one atlas that is used to render the world objects. We set this to
     * "graphics/testTextureAtlas" as this is where all our textures are. The name passed must also be found
     * in the graphics.json file that was loaded.
     */
    renderer->setWorldTextureAtlas("graphics/testTextureAtlas");

    /*
     * We create a simple background gradient that will be renderered at the very back and is always relative to the window, not the world.
     */
    auto background = std::make_unique<nox::app::graphics::BackgroundGradient>();
    background->setBottomColor({0.0f, 0.0f, 0.0f});
    background->setTopColor({1.0f, 1.0f, 1.0f});
    renderer->setBackgroundGradient(std::move(background));

    // We want a fully lit world so we set it to 1.0. Setting this to 0.0 will make it completely dark.
    renderer->setAmbientLightLevel(1.0f);

    // The camera we created in the constructor should be used for projecting the world on screen.
    renderer->setCamera(camera);

    /*
     * We also set the root scene graph node that we created in the constructor. This will be the first node to
     * be visited when the renderer parses the scene graph. Other nodes should be attached as children to this node.
     */
    renderer->setRootSceneNode(rootSceneNode);

    /*
     * This has to be called so that the renderer properly can organize its render steps for the rendering.
     * (I should really make this automated...)
     */
    renderer->organizeRenderSteps();

    // We need to use the renderer later.
    renderer = renderer;
}

void 
WindowView::onWindowSizeChanged(const glm::uvec2& size)
{
    
    RenderSdlWindowView::onWindowSizeChanged(size);

    /*
     * The camera needs to be updated for the new window size.
     */
    camera->setSize(size);
}

void 
WindowView::onEvent(const std::shared_ptr<nox::logic::event::Event>& event)
{
    
    RenderSdlWindowView::onEvent(event);

    using SceneNodeEdit = nox::logic::graphics::SceneNodeEdited;

    /*
     * This handles the creation and removal of scene nodes in the logic. We want to attach
     * all scene nodes created to our renderer so that we can render it to the screen, and of course
     * removed them if they are removed from the logic.
     *
     * The scene nodes can come from various places. An example is the ActorGraphics and its derived class
     * ActorSprite. It will create a scene node when the component is created and remove it when it is destroyed.
     *
     * if we don't handle this, no actors will be renderer to the screen.
     */
    if (event->isType(SceneNodeEdit::ID))
    {
        auto nodeEvent = static_cast<SceneNodeEdit*>(event.get());

        if (nodeEvent->getEditAction() == SceneNodeEdit::Action::CREATE)
        {
            rootSceneNode->addChild(nodeEvent->getSceneNode());
        }
        else if (nodeEvent->getEditAction() == SceneNodeEdit::Action::REMOVE)
        {
            rootSceneNode->removeChild(nodeEvent->getSceneNode());
        }
    }
}

void 
WindowView::onKeyPress(const SDL_KeyboardEvent& event)
{
    /*
     * This enables more debug data to be renderer, e.g. the physics shapes.
     */
    if (event.keysym.sym == SDLK_q && renderer != nullptr)
    {
        renderer->toggleDebugRendering();

        // The logic should know that we enable/disable the debug rendering so that it doesn't need to use time to update the
        // rendering.
        const auto debugRenderEvent = std::make_shared<nox::logic::graphics::DebugRenderingEnabled>(renderer->isDebugRenderingEnabled());
        getLogicContext()->getEventBroadcaster()->queueEvent(debugRenderEvent);
    }
}
