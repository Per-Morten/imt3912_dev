#ifndef NOX_ECS_TRANSFORM_H_
#define NOX_ECS_TRANSFORM_H_

#include <string>

#include <nox/ecs/Component.h>
#include <nox/ecs/EntityManager.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>



namespace nox
{
    namespace ecs
    {
        /**
         * Place an Actor with a position, rotation, and scale.
         * An important component used by many other components.
         *
         * # JSON Description
         * ## Name
         * %Transform
         *
         * ## Properties
         * - __position__:vec2 - Position of the Actor. Default vec2(0, 0).
         * - __rotation__:real - Rotation of the Actor. Default 0.
         * - __scale__:vec2 - Scale of the actor. Default vec2(1, 1).
         */
        class Transform final
            : public Component
        {
        public:
            const static std::string NAME;
        
            virtual ~Transform() = default;
        
            bool initialize(const Json::Value& componentJsonObject);
            //void serialize(Json::Value& componentObject) override;
            void onCreate();
            const std::string& getName() const;
        
            /**
             * Get the transform matrix representing the operations scale->rotate->translate.
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
             * Broadcast a transformation change to other components,
             * and globally to the EventManager.
             */
            void broadcastTransformChange();
        

            glm::vec2 position;
            glm::vec2 scale;
            float rotation;
        };
    }
}

#include "Transform.ipp"

#endif
