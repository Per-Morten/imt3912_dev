#ifndef NOX_ECS_SMARTHANDLE_H_
#define NOX_ECS_SMARTHANDLE_H_
#include <cstddef>

#include <nox/ecs/MetaInformation.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief Class used to have pointers that "survives" iterator invalidation.
         */
        template<class T, class Collection>
        class SmartHandle
        {
        public:
            /**
             * @brief Standard constructor, creates a SmartHandle that does not point to anything.
             *        However that also allows one to create the SmartHandle and assign it at a later stage.
             */
            SmartHandle() = default;

            /**
             * @brief      Creates a smart handle with the given object,
             *             generation and metaInformation.
             *
             * @param[in]  id          The id of the entity the component belongs to.
             * @param      object      The object the handle points to.
             * @param      generation  The generation this handle was set.
             * @param      collection  The collection containing the component.
             */
            SmartHandle(const EntityId& id,
                        T* object,
                        std::size_t generation,
                        Collection* collection);

            /**
             * @brief User defined conversion, allows for down-casting of type.
             *
             * @return Smart handle of type U, instead of type T.
             */
            template<class U, class Coll2>
            operator SmartHandle<U, Coll2>() const;

            /**
             * @brief Gets the underlying pointer to the object.
             *        Access is constant if pointer is not invalidated.
             *
             * @return Pointer to the object.
             *         nullptr if the object was not found.
             */
            T* get();

            /**
             * @brief Gets the underlying pointer to the object.
             *        Access is constant if pointer is not invalidated.
             *
             * @return Pointer to the object.
             *         nullptr if the object was not found.
             */
            T* operator*();

            /**
             * @brief Gets the underlying pointer to the object.
             *        Access is constant if pointer is not invalidated.
             *
             * @return Pointer to the object.
             *         nullptr if the object was not found.
             */
            T* operator->();

        private:
            std::size_t generation{};
            EntityId id{};
            Collection* collection{};
            T* ptr{};
        };
        
        /**
         * @brief use this alias rather than the SmartHandle.
         *        Had to use a template trick to avoid cyclic dependency.
         */
        class ComponentCollection;
        template<class T>
        using ComponentHandle = SmartHandle<T, ComponentCollection>;
    }
}

#include <nox/ecs/SmartHandle.tpp>
#endif
