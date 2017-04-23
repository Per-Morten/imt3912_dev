#include <type_traits>
#include <nox/ecs/Component.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Namespace created to avoid polluting the global namespace
         *             with these utility TMP functions.
         *
         *             The general idea behind these functions is to find out if
         *             a member function has been overloaded in a sub class.
         *             Member functions in C++ take in an additional implicit
         *             parameter, which is the "this" pointer. Because of this
         *             the prototype for a member function is:
         *
         *             func(Type type, <other args..>);
         *
         *             When a member function is called, the type is replaced
         *             with the instance variable of the object automatically.
         *             Meaning it becomes "this".
         *
         *             Using this information we should be able to detect if a
         *             function is overloaded, because in that case it will have
         *             a different parameter in the "Type" field.
         *
         *             Example:
         *             Base
         *             {
         *                  void(int a); // Prototype = void (Base::*)(int a);
         *             }
         *
         *             Derived
         *             {
         *                  void(int a); // Prototype = void(Derived::*)(int a);
         *             }
         *
         *             Meaning that these two functions are not of the same
         *             type.
         */
        namespace create_meta_info_meta
        {
            /**
             * @brief      Function for the situation where a class has not
             *             overloaded a function in base component. In this
             *             case, instead of doing a empty function call, we
             *             would just like to jump out early. So in component
             *             collection, if a component does not have a update
             *             function, we don't want to do calls to an empty
             *             function. We are unsure if that call can actually be
             *             optimized out, because going through a pointer is a
             *             pretty opaque thing.
             *
             * @param[in]  Unused
             *
             * @tparam     T              Function in Base Component.
             * @tparam     U              Function in Derived Component.
             * @tparam     OperationType  The OperationType that we are going to store the nullptr in.
             * @tparam     Operation      The operation itself, i.e. the lambda, in this case it is ignored.
             *
             * @return     nullptr.
             */
            template<class T, class U, class OperationType, class Operation>
                     typename std::enable_if<std::is_same<T, U>::value, OperationType>::type
            getOperation(T, U, OperationType, Operation)
            {
                return nullptr;
            }

            /**
             * @brief      Function for the situation where a class has
             *             overloaded a function from base component. In this
             *             case, we actually want to execute that function.
             *
             * @param[in]  operation      The operation i.e. lambda calling that should be returned.
             *
             * @tparam     T              Function in Base Component.
             * @tparam     U              Function in Derived Component.
             * @tparam     OperationType  The OperationType that we are going to store the operation in.
             * @tparam     Operation      The operation itself, i.e. the lambda.
             *
             * @return     The operation.
             */
            template<class T, class U, class OperationType, class Operation>
                     typename std::enable_if<!std::is_same<T, U>::value, OperationType>::type
            getOperation(T, U, OperationType, Operation operation)
            {
                return operation;
            }
        }
    }
}

template<class T>
nox::ecs::MetaInformation
nox::ecs::createMetaInformation(const TypeIdentifier& typeIdentifier)
{
    return createMetaInformation<T>(typeIdentifier, {});
}

template<class T>
nox::ecs::MetaInformation
nox::ecs::createMetaInformation(const TypeIdentifier& typeIdentifier,
                                const std::vector<nox::event::Event::IdType>& interestingLogicEvents)
{
    namespace meta = create_meta_info_meta;
    static_assert(std::is_base_of<nox::ecs::Component, T>::value, "Type T must inherit from nox::ecs::Component");

    MetaInformation info(typeIdentifier, sizeof(T));

    info.construct = [](Component* component,
                        const EntityId& id,
                        EntityManager* manager)
    {
        new(component)T(id, manager);
    };

    info.destruct = [](Component* component)
    {
        static_cast<T*>(component)->~T();
    };

    info.initialize =
        meta::getOperation(&Component::initialize,
                           &T::initialize,
                           info.initialize,
                           [](Component* component,
                              const Json::Value& value)
                           {
                               static_cast<T*>(component)->initialize(value);
                           });

    info.awake =
        meta::getOperation(&Component::awake,
                           &T::awake,
                           info.awake,
                           [](Component* component)
                           {
                               static_cast<T*>(component)->awake();
                           });

    info.activate =
        meta::getOperation(&Component::activate,
                           &T::activate,
                           info.activate,
                           [](Component* component)
                           {
                               static_cast<T*>(component)->activate();
                           });

    info.deactivate =
        meta::getOperation(&Component::deactivate,
                           &T::deactivate,
                           info.deactivate,
                           [](Component* component)
                           {
                               static_cast<T*>(component)->deactivate();
                           });

    info.hibernate =
        meta::getOperation(&Component::hibernate,
                           &T::hibernate,
                           info.hibernate,
                           [](Component* component)
                           {
                               static_cast<T*>(component)->hibernate();
                           });

    info.update =
        meta::getOperation(&Component::update,
                           &T::update,
                           info.update,
                           [](Component* first,
                              Component* last,
                              const nox::Duration& deltaTime)
                           {
                               auto begin = static_cast<T*>(first);
                               auto end = static_cast<T*>(last);

                               while (begin != end)
                               {
                                   begin->update(deltaTime);
                                   ++begin;
                               }
                           });

    info.updateAccess = (info.update) ? info.updateAccess :
                                        DataAccess::INDEPENDENT;

    info.receiveLogicEvent =
        meta::getOperation(&Component::receiveLogicEvent,
                           &T::receiveLogicEvent,
                           info.receiveLogicEvent,
                           [](Component* first,
                              Component* last,
                              const std::shared_ptr<nox::event::Event>& event)
                           {
                               auto begin = static_cast<T*>(first);
                               auto end = static_cast<T*>(last);

                               while (begin != end)
                               {
                                   begin->receiveLogicEvent(event);
                                   ++begin;
                               }
                           });

    info.receiveLogicEventAccess = (info.receiveLogicEvent) ? info.receiveLogicEventAccess :
                                                              DataAccess::INDEPENDENT;

    info.receiveEntityEvent =
        meta::getOperation(&Component::receiveEntityEvent,
                           &T::receiveEntityEvent,
                           info.receiveEntityEvent,
                           [](Component* first,
                              Component* last,
                              const nox::ecs::Event& event)
                           {
                               auto begin = static_cast<T*>(first);
                               auto end = static_cast<T*>(last);

                               while (begin != end)
                               {
                                   begin->receiveEntityEvent(event);
                                   ++begin;
                               }
                           });

    info.receiveEntityEventAccess = (info.receiveEntityEvent) ? info.receiveEntityEventAccess :
                                                                DataAccess::INDEPENDENT;

    info.moveConstruct = [](Component* dest, Component* src)
    {
        auto srcComp = static_cast<T*>(src);

        new(dest)T(std::move(*srcComp));
    };

    info.moveAssign = [](Component* dest, Component* src)
    {
        auto destComp = static_cast<T*>(dest);
        auto srcComp = static_cast<T*>(src);

        *destComp = std::move(*srcComp);
    };

    #ifdef NOX_ECS_COMPONENT_UNIQUE_PTR_VIRTUAL
    const auto create = [](const nox::ecs::EntityId& id, nox::ecs::EntityManager* manager) -> std::unique_ptr<Component>
    {
        return std::make_unique<T>(id, manager);
    };

    const auto moveCreate = [](Component& comp) -> std::unique_ptr<Component>
    {
        auto& tmp = static_cast<T&>(comp);
        return std::make_unique<T>(std::move(tmp));
    };

    info.virtualCreate = create;
    info.virtualMoveCreate = moveCreate;
    #endif 

    return info;
}
