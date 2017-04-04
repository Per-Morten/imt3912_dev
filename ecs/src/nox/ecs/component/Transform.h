#ifndef NOX_ECS_TRANSFORM_H_
#define NOX_ECS_TRANSFORM_H_

#include <nox/ecs/Component.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Place an Entity with a position, rotation, and scale. An
         *             important component used by many other components.
         */
        class Transform final
            : public Component
        {
        public:
            using nox::ecs::Component::Component;

            /*
             * @brief      Initialize class member variables using a json object
             *
             * @param[in]  componentJsonObject  Json object to get the data from
             *
             * @return     True if initialization was successful
             */
            void initialize(const Json::Value& componentJsonObject);
            
            /*
             * @brief      Serialized class into a json object
             *
             * @param      componentObject  Json object to put the data from
             *                              this class in
             */
            void serialize(Json::Value& componentObject);

            /*
             * @brief      Sends broadcast event when component is created
             */
            void onCreate();
        
            /*
             * @brief      Get the transform matrix representing the operations
             *             scale->rotate->translate.
             *
             * @return     The transform matrix.
             */
            glm::mat4 getTransformMatrix() const;
            
            /*
             * @brief      Sets the position.
             *
             * @param[in]  position   The position
             * @param[in]  broadcast  If the changes should be broadcasted
             */
            void setPosition(const glm::vec2& position, 
                             bool broadcast = true);

            /*
             * @brief      Sets the scale.
             *
             * @param[in]  scale      The scale
             * @param[in]  broadcast  If the changes should be broadcasted
             */
            void setScale(const glm::vec2& scale, 
                          bool broadcast = true);

            /*
             * @brief      Sets the rotation.
             *
             * @param[in]  rotation   The rotation
             * @param[in]  broadcast  If the changes should be broadcasted
             */
            void setRotation(const float rotation, 
                             bool broadcast = true);

            /*
             * @brief      Sets all the member variables of the transform.
             *
             * @param[in]  position   The position
             * @param[in]  rotation   The rotation
             * @param[in]  scale      The scale
             * @param[in]  broadcast  If the changes should be broadcasted
             */
            void setTransform(const glm::vec2& position, 
                              const float rotation, 
                              const glm::vec2& scale, 
                              bool broadcast = true);
            
            /*
             * @brief      Gets the position.
             *
             * @return     The position.
             */
            inline const glm::vec2& getPosition() const;

            /*
             * @brief      Gets the rotation.
             *
             * @return     The rotation.
             */
            inline float getRotation() const;

            /*
             * @brief      Gets the scale.
             *
             * @return     The scale.
             */
            inline const glm::vec2& getScale() const;
        
        private:
            /**
             * @brief      Broadcast a transformation change to other
             *             components, and globally to the EventManager.
             */
            void broadcastTransformChange();
        
            glm::vec2 position{};
            glm::vec2 scale{};
            float rotation{};
        };
    }
}

#include <nox/ecs/component/Transform.ipp>

#endif
