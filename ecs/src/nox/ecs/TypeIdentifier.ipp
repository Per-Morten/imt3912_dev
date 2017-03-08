nox::ecs::TypeIdentifier::TypeIdentifier(std::size_t identifier)
    : value(identifier)
{

}


nox::ecs::TypeIdentifier::TypeIdentifier(const std::string& identifier)
    : value(std::hash<std::string>{}(identifier))
{

}

std::size_t
nox::ecs::TypeIdentifier::getValue() const
{
    return this->value; 
}

bool
nox::ecs::operator==(const nox::ecs::TypeIdentifier& lhs,
                     const nox::ecs::TypeIdentifier& rhs)
{
    return lhs.getValue() == rhs.getValue();
}

bool
nox::ecs::operator!=(const nox::ecs::TypeIdentifier& lhs,
                     const nox::ecs::TypeIdentifier& rhs)
{
    return !(lhs == rhs);
}
