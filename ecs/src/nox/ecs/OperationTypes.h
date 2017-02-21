#ifndef NOX_ECS_OPERATIONTYPES_H_
#define NOX_ECS_OPERATIONTYPES_H_
#include <memory>
#include <json/value.h>

#include <nox/ecs/Event.h>
#include <nox/ecs/IComponent.h>
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
            using RangedOp = void(*)(IComponent* first,
                                     IComponent* last);
           
            /** 
             * @brief Function to be applied to a single element.
             *
             * @param component the component to apply the function on.
             *
             * @warning Casting to the correct component type is the
             *          users responsibility.
             */ 
            using UnaryOp = void(*)(IComponent* component);

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
            using InitializeOp = void(*)(IComponent component,
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
            using LogicEventOp = void(*)(IComponent* first,
                                         IComponent* last,
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
            using ComponentEventOp = void(*)(IComponent* component,
                                             const ecs::Event& event);
        }
    }
}


#endif
