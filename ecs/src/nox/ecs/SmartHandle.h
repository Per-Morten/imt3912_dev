#ifndef NOX_ECS_SMARTHANDLE_H_
#define NOX_ECS_SMARTHANDLE_H_
#include <cstdint>
#include <nox/ecs/MetaInformation.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief Class used to have pointers that "survives" iterator invalidation.
         */
        template<class T>
        class SmartHandle
        {
        public:
            /**
             * @brief Standard constructor, creates a SmartHandle that does not point to anything.
             *        However that also allows one to create the SmartHandle and assign it at a later stage.
             */
            SmartHandle() = default;

            /**
             * @brief Creates a smart handle with the given object, 
             *        generation and metaInformation.
             *
             * @param object the object the handle points to.
             * @param generation the generation this handle was set.
             * @param metaInformation the metaInformation that this object shall query
             *                        for the current generation.
             */
            SmartHandle(T* object,
                        std::size_t generation,
                        const MetaInformation* metaInformation);

            /**
             * @brief User defined conversion, allows for down-casting of type.
             *
             * @return Smart handle of type U, instead of type T.
             */
            template<class U>
            operator SmartHandle<U>() const;

            /**
             * @brief Gets the underlying pointer to the object.
             *        Access is constant if pointer is not invalidated.
             *
             * @return Pointer to the object.
             *         nullptr if the object was not found.
             */
            T* get() const;

            /**
             * @brief Gets the underlying pointer to the object.
             *        Access is constant if pointer is not invalidated.
             *
             * @return Pointer to the object.
             *         nullptr if the object was not found.
             */
            T* operator*() const;

            /**
             * @brief Gets the underlying pointer to the object.
             *        Access is constant if pointer is not invalidated.
             *
             * @return Pointer to the object.
             *         nullptr if the object was not found.
             */
            T* operator->() const;

        private:
            std::size_t generation{};
            const MetaInformation* metaInformation{};
            T* ptr{};
        };
    }
}

#include <nox/ecs/SmartHandle.tpp>
#endif
