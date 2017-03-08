#ifndef NOX_ECS_OPERATIONTYPES_H_
#define NOX_ECS_OPERATIONTYPES_H_
#include <json/value.h>
#include <memory>

#include <nox/common/types.h>
#include <nox/ecs/Component.h>
#include <nox/ecs/EntityId.h>
#include <nox/ecs/Event.h>
#include <nox/event/Event.h>

namespace nox
{
    namespace ecs
    {
        namespace operation
        {
            /**
             * @brief Function to be applied to a range of elements.
             *        Iterating is done in an stl-iterator like manner.
             *
             * @param first pointer to the first element in the range.
             * @param last past-the-end pointer of the range.
             *
             * @warning Casting to the correct component type is the
             *          users responsibility. 
             */
            using RangedOp = void(*)(Component* first,
                                     Component* last);
           
            /** 
             * @brief Function to be applied to a single element.
             *
             * @param component the component to apply the function on.
             *
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */ 
            using UnaryOp = void(*)(Component* component);

            /**
             * @brief Function to be applied to move an object in memory
             *        usually through a move construction or move assignment.
             *        
             * @param destination The memory/component to move source into.
             * @param source The object to move from.
             * 
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */ 
            using MoveOp = void(*)(Component* destination,
                                   Component* source);

            /**
             * @brief Function to be applied to construct a single element.
             * 
             * @param component the component to construct.
             * @param id the id to give the component.
             * 
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */
            using ConstructOp = void(*)(Component* component,
                                        const EntityId& id);

            /**
             * @brief Function used for updating a range of elements.
             * 
             * @param first pointer to the first element in the range.
             * @param last past-the-end pointer of the range.
             * @param deltaTime time since last update call.
             * 
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */
            using UpdateOp = void(*)(Component* first,
                                     Component* last,
                                     const nox::Duration& deltaTime);

            /**
             * @brief Function used for initialization a component
             *        with a json value.
             *
             * @param component the component to initialize.
             * @param value the json object containing the data for
             *        initialization.
             *
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */
            using InitializeOp = void(*)(Component* component,
                                         const Json::Value& value);

            /**
             * @brief Function to be called on events coming from the
             *        NOX event system.
             *
             * @param first pointer to the first element in the range.
             * @param last past-the-end pointer of the range.
             * @param event the event to respond to.
             *
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */
            using LogicEventOp = void(*)(Component* first,
                                         Component* last,
                                         const std::shared_ptr<nox::event::Event>& event);

            /**
             * @brief Function to be called on inter component communication.
             * 
             * @param component pointer to the component receiving the event.
             * @param event the event to respond to.
             *
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */
            using ComponentEventOp = void(*)(Component* first,
                                             Component* last,
                                             const ecs::Event& event);
        }
    }
}


#endif
