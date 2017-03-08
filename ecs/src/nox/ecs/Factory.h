#ifndef NOX_ECS_FACTORY_H_
#define NOX_ECS_FACTORY_H_
#include <vector>
#include <string>

#include <nox/ecs/Component.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/component/Children.h>

#include <json/json.h>

namespace nox
{
    namespace ecs
    {
        class EntityManager;
    }
}

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Factory used to create entities based on json files. Non
         *             json based entities are created just through the manager.
         */
        class Factory
        {
        public:
            /**
             * @brief      Regular constructor, a Factory needs an EntityManager to function properly.
             *
             * @param      entityManager  The manager in charge of creating entities. 
             *                            The factory will mainly deal with json files, and forward 
             *                            creation into the entityManager.
             */
            Factory(EntityManager& entityManager);

            /**
             * @brief      Creates an entity with the given id, based on the
             *             json definition with name == definitionName.
             *
             * @param[in]  id              The id of the entity.
             * @param[in]  definitionName  The name of the json definition to
             *                             create the entity from.
             */
            void 
            createEntity(const EntityId& id,
                         const std::string& definitionName);

            /**
             * @brief      Creates entity definitions based on a json root.
             *
             * @note       This function is a temp until support for
             *             nox-resources are added to the factory.
             *
             * @param[in]  root  The json root object containing all
             *                   definitions.
             */
            void 
            createEntityDefinition(const Json::Value& root);
        
        private:

            /**
             * @brief      EntityDefinition, basically just a struct for keeping
             *             relevant json information togheter.
             */
            struct EntityDefinition
            {
                std::string name;
                std::string package; //< Not implemented yet!
                Json::Value json;

                std::string getFullName() const;
            };

            using JsonStack = std::stack<Json::Value, std::vector<Json::Value>>;

            /**
             * @brief      Creates an entity with the given id based on the
             *             definition parameter.
             *
             * @param[in]  id          The id of the entity to create.
             * @param[in]  definition  The definition object holding all
             *                         relevant information.
             */
            void 
            createEntity(const EntityId& id,
                         const EntityDefinition& definition);

            /**
             * @brief      Extends the json values within destination based on source.
             *             Function is taken from the existing nox function extendActorComponents. 
             *
             * @param      destination  The json value to extend into.
             * @param[in]  source       The source to extend from.
             */
            void
            extendComponents(Json::Value& destination, const Json::Value& source);

            /**
             * @brief      Extends the json value within destination based on source.
             *             Function is taken from the existing nox function extendJsonValue. 
             *
             * @param      destination  The json value to extend into.
             * @param[in]  source       The source to extend from.
             */
            void
            extendJsonValue(Json::Value& destination, const Json::Value& source);

            /**
             * @brief      Creates a stack with the components to extend from.
             *
             * @param[in]  root  The root json value.
             *
             * @return     Stack containing all json objects to extend from.
             */
            JsonStack
            createExtensionStack(const Json::Value& root);

            /**
             * @brief      Creates a children component based on the values in children.
             *             The function also creates all the children, and assigns them
             *             a parent component allowing them to get back to their parent.
             *             
             * @param[in]  id        The id of the parent entity.
             * @param[in]  children  The json value holding all the children.
             *
             * @return     A children component with all children added in it.
             */
            Children
            parseChildren(const EntityId& id,
                          const Json::Value& children);
           
            /**
             * @brief      Returns the TypeIdentifier within the value, or just 
             *             the name of json value. This allows the user to have numerical identifiers. 
             *
             * @param[in]  value  The value to get the identifier from.
             * @param[in]  name   The name of the value, which will be used if no other identifier is given.
             *
             * @return     Type identifier used to represent the type parsed from value.
             */
            TypeIdentifier
            getTypeIdentifier(const Json::Value& value, const std::string& name);

            EntityManager& entityManager;
            std::vector<EntityDefinition> definitions;
        };
    }
}
#endif
