#ifndef NOX_ECS_DATAACCESS_H_
#define NOX_ECS_DATAACCESS_H_

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Used to indicate what interactions are done with non
         *             thread-safe systems during an operation.
         */
        enum class DataAccess : std::uint8_t
        {
            /**
             * @brief      Operation is completely independent or only interacts
             *             with thread-safe systems and can be run concurrently
             *             with other functions.
             */
            INDEPENDENT, 

            /**
             * @brief      Operation is reading from other Components, and can't
             *             be run concurrently with the components listed.
             */ 
            READ_ONLY,

            /**
             * @brief      Operation is a read write operation, both reading and
             *             modifying a non-thread safe system/component, and
             *             can't be run concurrently with the components listed.
             *
             * @note       This functionality is not implemented yet, and a
             *             READ_WRITE will be treated like UNKNOWN.
             */
            READ_WRITE,

            /**
             * @brief      It is unknown what interactions this function does,
             *             it therefore can't be run concurrently.
             */
            UNKNOWN,
        };
    }
}

#endif
