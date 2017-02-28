#ifndef NOX_ECS_CREATEMETAINFORMATION_H_
#define NOX_ECS_CREATEMETAINFORMATION_H_
#include <nox/ecs/MetaInformation.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/OperationTypes.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Creates a MetaInformation struct, 
         *             containing information about a specific component.
         *             This is the default creation function, and it is here expected
         *             that all lifecycle functions are implemented as member functions.
         *
         * @param[in]  typeIdentifier  The type identifier of the component type.
         *
         * @tparam     ComponentType   The component type, used to set up proper constructor and destructor.
         *
         * @return     A MetaInformation struct with the correct operations to execute on the object.
         */
        template<class ComponentType>
        MetaInformation
        createMetaInformation(const TypeIdentifier& typeIdentifier);

        /**
         * @brief      Creates a MetaInformation struct, 
         *             containing information about a specific component.
         *             This is the default creation function, and it is here expected
         *             that all lifecycle functions are implemented as member functions.
         *
         * @param[in]  typeIdentifier  The type identifier of the component type.
         * @param[in]  interestingLogicEvents A collection of events that the component wants to listen to.
         *
         * @tparam     ComponentType   The component type, used to set up proper constructor and destructor.
         *
         * @return     A MetaInformation struct with the correct operations to execute on the object.
         */
        template<class ComponentType>
        MetaInformation
        createMetaInformation(const TypeIdentifier& typeIdentifier,
                              const std::vector<nox::event::Event::IdType>& interestingLogicEvents);
    }
}


#include <nox/ecs/createMetaInformation.tpp>
#endif
