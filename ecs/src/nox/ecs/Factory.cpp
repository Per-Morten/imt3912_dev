#include <nox/ecs/Factory.h>

#include <nox/ecs/EntityManager.h>
#include <nox/ecs/ComponentType.h>

nox::ecs::Factory::Factory(EntityManager& entityManager)
    : entityManager(entityManager)
{

}


void
nox::ecs::Factory::createEntity(const EntityId& id,
                                const std::string& definitionName)
{
    const auto definition = std::find_if(std::cbegin(this->definitions),
                                         std::cend(this->definitions), 
                                         [&definitionName](const auto& item)
                                         { return item.name == definitionName; });
    if (definition != std::cend(this->definitions))
    {
        this->createEntity(id, *definition);
    }
}

void 
nox::ecs::Factory::createEntityDefinition(const Json::Value& root)
{
    const auto names = root.getMemberNames();
    for (const auto& item : names)
    {
        EntityDefinition definition;
        definition.name = item;
        definition.json = root[item];
        this->definitions.push_back(definition);
    }
}

void
nox::ecs::Factory::createEntity(const EntityId& id,
                                const EntityDefinition& definition)
{
    Json::Value components = definition.json["components"];
    
    // Handle extension
    auto extensionStack = createExtensionStack(definition.json);

    while (!extensionStack.empty())
    {
        const auto& value = extensionStack.top();
        this->extendComponents(components, value["components"]);

        extensionStack.pop();
    }

    // Process children.
    const auto& children = components.get("Children", Json::nullValue);
    if (children != Json::nullValue)
    {
        auto childrenComponent = std::move(this->parseChildren(id, children));
        this->entityManager.assignComponent(id, {component_type::CHILDREN}, std::move(childrenComponent));
    }

    // Start creating the types. 
    const auto componentNames = [](auto names) -> std::vector<std::string>
    {
        names.erase(std::remove_if(std::begin(names),
                                   std::end(names),
                                   [](const auto& item) 
                                   { return item == "Children"; }),
                    std::end(names));
        
        return names;
    }(components.getMemberNames());
    
    for (const auto& item : componentNames)
    {
        auto& component = components[item];
        const auto identifier = this->getTypeIdentifier(component, item);

        this->entityManager.assignComponent(id,
                                            identifier,
                                            component);
    } 
}

void
nox::ecs::Factory::extendComponents(Json::Value& destination, const Json::Value& source)
{
    const auto names = source.getMemberNames();

    for (const auto& name : names)
    {
        auto& component = destination[name];

        if (component.isNull() == false)
        {
            this->extendJsonValue(component, source[name]);
        }
        else
        {
            destination[name] = source[name];
        }
    }
}

void
nox::ecs::Factory::extendJsonValue(Json::Value& destination, const Json::Value& source)
{
    if (source.isObject() == true && destination.isObject() == true)
    {
        const Json::Value::Members& sourceMembers = source.getMemberNames();

        for (const auto& name : sourceMembers)
        {
            this->extendJsonValue(destination[name], source[name]);
        }
    }
    else if (source.isArray() == true && destination.isArray() == true)
    {
        for (Json::ArrayIndex i = 0; i < source.size(); i++)
        {
            this->extendJsonValue(destination[i], source[i]);
        }
    }
    else
    {
        destination = source;
    }
}

nox::ecs::Factory::JsonStack
nox::ecs::Factory::createExtensionStack(const Json::Value& root)
{
    JsonStack stack;
    stack.push({root});

    Json::Value extension = root.get("extend", Json::nullValue);
    while (extension.isNull() == false)
    {
        const auto extensionName = extension.asString();

        const auto& definition = std::find_if(std::cbegin(this->definitions),
                                              std::cend(this->definitions), 
                                              [&extensionName](const auto& item)
                                              { return item.name == extensionName; });

        if (definition != std::cend(this->definitions))
        {
            stack.push({definition->json});
            extension = definition->json.get("extend", Json::nullValue);
        }
        else
        {
            extension = Json::nullValue;
        }
    }

    return stack;
}

nox::ecs::Children
nox::ecs::Factory::parseChildren(const EntityId& id, 
                                 const Json::Value& children)
{
    Children childrenComp(id, &entityManager);
    for (const auto& item : children)
    {
        const auto childId = this->entityManager.createEntity(item.asString());
        Parent parent(childId, &entityManager);
        parent.parentId = id;
        this->entityManager.assignComponent(childId, {component_type::PARENT}, std::move(parent));

        childrenComp.addChild(childId);
    }

    return std::move(childrenComp);
}

nox::ecs::TypeIdentifier
nox::ecs::Factory::getTypeIdentifier(const Json::Value& value, const std::string& name)
{
    if (value.isMember("identifier"))
    {
        return TypeIdentifier(value["identifier"].asUInt());
    }
    return TypeIdentifier(name);
}
