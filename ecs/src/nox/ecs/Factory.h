#ifndef NOX_ECS_FACTORY_H_
#define NOX_ECS_FACTORY_H_
#include <vector>
#include <string>

#include <nox/ecs/EntityManager.h>

#include <json/json.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief Factory used to create entities based on json files.
         *        Non json based entities are created just through the manager.
         */
        class Factory
        {
        public:
            Factory(EntityManager& entityManager);

            void 
            createEntity(const EntityId& id,
                         const std::string& definitionName);

            void 
            createEntityDefinition(const Json::Value& root);
        
        private:
            struct EntityDefinition
            {
                std::string name;
                std::string package;
                Json::Value json;

                std::string getFullName() const;
            };

            using JsonStack = std::stack<Json::Value, std::vector<Json::Value>>;

            void 
            createEntity(const EntityId& id,
                         const EntityDefinition& definition);

            void
            extendComponents(Json::Value& destination, const Json::Value& source);

            void
            extendJsonValue(Json::Value& destination, const Json::Value& source);

            JsonStack
            createExtensionStack(const Json::Value& root);

            TypeIdentifier
            getTypeIdentifier(const Json::Value& value, const std::string& name);

            EntityManager& entityManager;
            std::vector<EntityDefinition> definitions;

        };
    }
}
#endif
