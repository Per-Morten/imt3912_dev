#ifndef NOX_ECS_METAINFORMATION_H_
#define NOX_ECS_METAINFORMATION_H_
#include <cstdint>

#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/OperationTypes.h>

#include <nox/event/Event.h>

namespace nox
{
    namespace ecs
    {
        struct MetaInformation
        {
            /**
             * @brief To ensure that the two most important pieces of information are given,
             *        the default constructor is deleted.
             */
            MetaInformation() = delete;

            /**
             * @brief Constructs the MetaInformation with the given parameters.
             *
             * @param typeIdentifier the typeIdentifier of the type with this MetaInformation.
             * @param sizeOftype the size of the type with this MetaInformation.
             */
            MetaInformation(const TypeIdentifier& typeIdentifier,
                            std::size_t sizeOfType);
            /**
             * @brief Used to identify the type this MetaInformation is related to.
             */
            const TypeIdentifier typeIdentifier;

            /**
             * @brief Holds the size of the type this MetaInformation is related to.
             *        used for knowing how much to allocate.
             */
            const std::size_t sizeOfType{};

            /**
             * @brief Operation indicating how the components shall be constructed.
             */
            operation::ConstructOp construct{};

            /**
             * @brief Operation indicating how the components shall be destructed.
             */
            operation::UnaryOp destruct{};

            /**
             * @brief Operation indicating how the components shall be initialized.
             */
            operation::InitializeOp initialize{};

            /**
             * @brief Operation to run when a component is awoken.
             */
            operation::UnaryOp awake{};

            /**
             * @brief Operation to run when a component is activated.
             */
            operation::UnaryOp activate{};

            /**
             * @brief Operation to run when a component is deactivated.
             */
            operation::UnaryOp deactivate{};

            /**
             * @brief Operation to run when a component is set to hibernation.
             */
            operation::UnaryOp hibernate{};

            /**
             * @brief Operation to run when a component is updated.
             */
            operation::UpdateOp update{};

            /**
             * @brief Operation to run when a component is supposed to receive a logic event.
             */
            operation::LogicEventOp receiveLogicEvent{};

            /**
             * @brief Operation to run when a component is supposed to receive a component event.
             */
            operation::ComponentEventOp receiveComponentEvent{};

            /**
             * @brief Collection of logicEvents that this component wants to subscribe to.
             */
            std::vector<nox::event::Event::IdType> interestingLogicEvents{};
        };
    }
}

#endif
