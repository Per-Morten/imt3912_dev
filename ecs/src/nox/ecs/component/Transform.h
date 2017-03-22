#ifndef NOX_ECS_TRANSFORM_H_
#define NOX_ECS_TRANSFORM_H_

#include <string>

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
            /**
             * @brief      Move constructor for Children component.
             *
             * @param[in]  position   The position
             * @param[in]  rotation   The rotation
             * @param[in]  scale      The scale
             * @param[in]  broadcast  If the changes should be broadcasted
             */
            Transform(const glm::vec2& position, 
                            const float rotation, 
                            const glm::vec2& scale, 
                            bool broadcast = true);

            bool initialize(const Json::Value& componentJsonObject);
            //void serialize(Json::Value& componentObject) override;
            void onCreate();
        
            /**
             * @brief      Get the transform matrix representing the operations
             *             scale->rotate->translate.
             *
             * @return     The transform matrix.
             */
            glm::mat4 getTransformMatrix();
        

            void setPosition(const glm::vec2& position, 
                             bool broadcast = true);

            void setScale(const glm::vec2& scale, 
                          bool broadcast = true);

            void setRotation(const float rotation, 
                             bool broadcast = true);

            void setTransform(const glm::vec2& position, 
                              const float rotation, 
                              const glm::vec2& scale, 
                              bool broadcast = true);
        
            inline const glm::vec2& getPosition() const;
            inline float getRotation() const;
            inline const glm::vec2& getScale() const;
        
        private:
            /**
             * @brief      Broadcast a transformation change to other
             *             components, and globally to the EventManager.
             */
            void broadcastTransformChange();
        

            glm::vec2 position;
            glm::vec2 scale;
            float rotation;
        };
    }
}

#include <nox/ecs/component/Transform.ipp>

#endif
