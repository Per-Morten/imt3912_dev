#include <nox/ecs/MetaInformation.h>

nox::ecs::MetaInformation::MetaInformation(const TypeIdentifier& typeIdentifier,
                                           std::size_t sizeOfType)
    : typeIdentifier(typeIdentifier)
    , size(sizeOfType)
{
    
}
