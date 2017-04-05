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
            getOperation(Operation)
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
            getOperation(Operation operation)
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
    using namespace create_meta_info_meta;
    static_assert(std::is_base_of<nox::ecs::Component, T>::value, "Type T must inherit from nox::ecs::Component");

    MetaInformation info(typeIdentifier, sizeof(T));

    info.construct = [](Component* component, const EntityId& id, EntityManager* manager)
    {
        new(component)T(id, manager);
    };

    info.destruct = [](Component* component)
    {
        static_cast<T*>(component)->~T();
    };

    info.initialize =
        getOperation<decltype(&Component::initialize),
                     decltype(&T::initialize),
                     decltype(info.initialize)>(
                     [](Component* component,
                        const Json::Value& value)
                     {
                         static_cast<T*>(component)->initialize(value);
                     });

    info.awake =
        getOperation<decltype(&Component::awake),
                     decltype(&T::awake),
                     decltype(info.awake)>(
                     [](Component* component)
                     {
                         static_cast<T*>(component)->awake();
                     });

    info.activate =
        getOperation<decltype(&Component::activate),
                     decltype(&T::activate),
                     decltype(info.activate)>(
                     [](Component* component)
                     {
                         static_cast<T*>(component)->activate();
                     });

    info.deactivate =
        getOperation<decltype(&Component::deactivate),
                     decltype(&T::deactivate),
                     decltype(info.deactivate)>(
                     [](Component* component)
                     {
                         static_cast<T*>(component)->deactivate();
                     });

    info.hibernate =
        getOperation<decltype(&Component::hibernate),
                     decltype(&T::hibernate),
                     decltype(info.hibernate)>(
                     [](Component* component)
                     {
                         static_cast<T*>(component)->hibernate();
                     });

    info.update =
        getOperation<decltype(&Component::update),
                     decltype(&T::update),
                     decltype(info.update)>(
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
        getOperation<decltype(&Component::receiveLogicEvent),
                     decltype(&T::receiveLogicEvent),
                     decltype(info.receiveLogicEvent)>(
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
        getOperation<decltype(&Component::receiveEntityEvent),
                     decltype(&T::receiveEntityEvent),
                     decltype(info.receiveEntityEvent)>(
                     [](Component* first,Component* last,
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

    return info;
}
