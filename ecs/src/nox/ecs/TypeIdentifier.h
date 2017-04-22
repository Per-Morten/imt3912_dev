#ifndef NOX_ECS_TYPEIDENTIFIER_H_
#define NOX_ECS_TYPEIDENTIFIER_H_
#include <cstddef>
#include <string>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief Lightweight class used to map a name/tag to a type.
         *
         * @detail The class is used with either a numerical value, 
         *         or a hashed std::string value. 
         *         The intention of the class is to avoid memory dynamic
         *         memory allocation, which is why the identifier in its
         *         base is just a numerical value.
         *
         * @warning Mixing numerical and string based representation
         *          on the same type is illegal.  
         */
        class TypeIdentifier
        {
        public:
            /**
             * @brief Default constructor is deleted to avoid
             *        setting up the identifier in an invalid state.
             */  
            TypeIdentifier() = delete;

            /**
             * @brief Creates the TypeIdentifier with the given
             *        numerical value.
             *
             * @param identifier the numerical value to assign to value.
             *
             * @note The identifier argument is not hashed,
             *       allowing for pre_hashing if wanted.
             */
            inline TypeIdentifier(std::size_t identifier);

            /**
             * @brief Creates the TypeIdentifier with the numerical
             *        value given by std::hash<std::string>(identifier).
             *
             * @param identifier the identifier to hash for a numerical value.
             */
            inline TypeIdentifier(const std::string& identifier);

            /**
             * @brief Returns the underlying value of the TypeIdentifier
             *
             * @return The underlying value of the TypeIdentifier.
             */
            inline std::size_t getValue() const;

        private:
            std::size_t value{};

        };

        /**
         * @brief Compares equality between the two parameters based on their
         *        underlying value.
         *
         * @param lhs the left hand side argument of the operation.
         * @param rhs the right hand side argument of the operation.
         *
         * @return true if lhs.getValue() == rhs.getValue(), false otherwise.
         */
        inline bool 
        operator==(const TypeIdentifier& lhs,
                   const TypeIdentifier& rhs);
        
        /**
         * @brief Compares inequality between the two parameters based on their
         *        underlying value.
         *
         * @param lhs the left hand side argument of the operation.
         * @param rhs the right hand side argument of the operation.
         *
         * @return true if lhs.getValue() != rhs.getValue(), false otherwise.
         */
        inline bool
        operator!=(const TypeIdentifier& lhs,
                   const TypeIdentifier& rhs);
    }
}

#include <nox/ecs/TypeIdentifier.ipp>

#endif
