#ifndef NOX_ECS_SPRITE_H_
#define NOX_ECS_SPRITE_H_

#include <nox/ecs/Component.h>
#include <nox/ecs/component/Transform.h>
#include <nox/ecs/SmartHandle.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace nox
{
    namespace app
    {
        namespace graphics
        {
            class SpriteRenderNode;
            class SceneGraphNode;
            class TransformationNode;
        }
    }
}

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Defines a sprite to be rendered. Allows the change of the
         *             entity transform via events.
         */
        class Sprite final
            : public Component
        {
        public:          
            using nox::ecs::Component::Component;

            /**
             * @brief      Creates a sprite with the given entityId.
             *
             * @param      entityId       the id of the entity this component is
             *                            tied to.
             * @param      entityManager  The entity manager this component is
             *                            handled by.
             */
            Sprite(const EntityId& entityId, 
                      EntityManager* entityManager);

            /**
             * @brief      Initializes the sprite objects member variables from
             *             a json object
             *
             * @param[in]  componentJsonObject  The component json object
             */
            void 
            initialize(const Json::Value& componentJsonObject);

            /**
             * @brief      Serializes the sprite objects member variables to
             *             file via a json object
             *
             * @param[in]  componentObject  The component object
             */
            void 
            serialize(Json::Value& componentObject);

            /**
             * @brief      Called when the component is deactivated, broadcasts
             *             an event to the rendering system that this sprite
             *             should not be drawn.
             */
            void 
            deactivate();

            /**
             * @brief      Called when the component is activated, broadcasts
             *             an event to the rendering system that this sprite
             *             should be drawn.
             */
            void 
            activate();    

            /**
             * @brief      Updates the transform data in the object if a change
             *             has happened in the corresponding transform component
             *             of the same entity.
             *
             * @param[in]  event  The event
             */
            void 
            receiveEntityEvent(const ecs::Event& event);

            /**
             * @brief      Sets the center.
             *
             * @param[in]  center  The center
             */
            void 
            setCenter(const glm::vec2& center);
            
            /**
             * @brief      Sets the name of which sprite to be drawn currently.
             *
             * @warning    The sprite references in the spriteName parameter
             *             needs to be registered with the addSprite function
             *             first.
             *
             * @param[in]  spriteName  The sprite name
             */
            void 
            setActiveSprite(const std::string& spriteName);
        
            /**
             * @brief      Sets if the object should be rendered or not.
             *
             * @param[in]  state  if true the object will be rendered.
             */
            void 
            setRenderingState(const bool state);
        
            /**
             * @brief      Sets and, if necessary, updates the renderlevel of
             *             the spriteRenderNode.
             *
             * @param[in]  renderLevel  Level to render sprite on. Higher value
             *                          means closer to camera. Currently in the
             *                          range [0, 10000]. Default 0.
             */
            void 
            setRenderLevel(unsigned int renderLevel);

            /**
             * @brief      Adds a new sprite which can be used to draw from
             *
             * @param[in]  spriteName  The sprite name
             */
            void 
            addSprite(const std::string& spriteName);
        
            /**
             * @brief      Gets the color.
             *
             * @return     The color.
             */
            inline const glm::vec4& 
            getColor() const;
            
            /**
             * @brief      Gets the scale.
             *
             * @return     The scale.
             */
            inline const glm::vec2& 
            getScale() const;
            
            /**
             * @brief      Gets the center.
             *
             * @return     The center.
             */
            inline const glm::vec2& 
            getCenter() const;
        
            /**
             * @brief      Sets the color.
             *
             * @param[in]  color  The color
             */
            inline void 
            setColor(const glm::vec4& color);
            
            /**
             * @brief      Sets the scale.
             *
             * @param[in]  scale  The scale
             */
            inline void 
            setScale(const glm::vec2& scale);
            
            /**
             * @brief      Sets the opacity.
             *
             * @param[in]  opacity  Opacity of the Actor in range [0, 1], where
             *                      0 is transparent and 1 is opaque.
             */
            inline void 
            setOpacity(const float opacity);
            
            /**
             * @brief      Sets the rotation.
             *
             * @param[in]  rotation  The rotation
             */
            inline void 
            setRotation(const float rotation);
            
            /**
             * @brief      Sets the position.
             *
             * @param[in]  position  The position
             */
            inline void 
            setPosition(const glm::vec2& position);

            /**
             * @brief      Sets if the rendering should be flipped horizontally.
             *
             * @param[in]  flipped  True if setting to flipped.
             */
            inline void 
            setFlippedHorizontal(const bool flipped);
        
            /**
             * @brief      Sets if the rendering should be flipped vertically.
             *
             * @param[in]  flipped  True if setting to flipped.
             */
            inline void 
            setFlippedVertical(const bool flipped);

            /**
             * @brief      Sets the offset.
             *
             * @param[in]  offset  The offset
             */
            inline void 
            setOffset(const glm::vec2& offset);    

        private:
            /**
             * @brief      Called when the color of the render component has
             *             changed.
             *
             * @param      color  The new color.
             */
            void 
            onColorChange(const glm::vec4& color);
            
            /**
             * @brief      Called when the light multiplier has changed.
             *
             * @param[in]  lightMultiplier  The light multiplier
             */
            void 
            onLightMultiplierChange(const float lightMultiplier);
            

            /**
             * @brief      Called when the emissive light has changed.
             *
             * @param[in]  emissiveLight  The emissive light
             */
            void 
            onEmissiveLightChange(const float emissiveLight);

            /**
             * @brief      Creates a scene node.
             *
             * @return     The new scene node.
             */
            std::shared_ptr<nox::app::graphics::SceneGraphNode> 
            createSceneNode() const;
            
            /**
             * @brief      Called when certain member variables are changed, and
             *             broadcasts those changes to the nox rendering system.
             */
            void 
            updateRenderTransform();
            
            /**
             * @brief      Called when certain member variables are changed, and
             *             broadcasts those changes to the nox rendering system.
             */
            void 
            updateEntityTransform();
            
            /**
             * @brief      Broadcasts a scene node creation.
             */
            void 
            broadcastSceneNodeCreation();
            
            /**
             * @brief      Broadcasts a scene node removal.
             */
            void 
            broadcastSceneNodeRemoval();

            glm::vec4 color{};
            glm::vec2 position{};
            glm::vec2 scale{};
            glm::vec2 flipScale{};
            glm::vec2 offset{};
            glm::vec2 center{};

            ComponentHandle<Transform> transformHandle{};
            std::shared_ptr<app::graphics::TransformationNode> entityTransformNode{};
            std::shared_ptr<app::graphics::TransformationNode> renderTransformNode{};
            std::shared_ptr<nox::app::graphics::SpriteRenderNode> spriteRenderNode{};

            float rotation{};
            float lightMultiplier{};
            float emissiveLight{};
            unsigned int renderLevel{};
            std::string spriteName{};
            bool renderingEnabled{};
        
        };
    }
}

#include <nox/ecs/component/Sprite.ipp>

#endif
