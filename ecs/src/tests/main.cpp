#include <nox/ecs/Component.h>
#include <nox/ecs/OperationTypes.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/createMetaInformation.h>
#include <nox/ecs/EntityManager.h>
#include <nox/ecs/ComponentCollection.h>
#include <nox/util/nox_assert.h>
#include <nox/event/Event.h>
#include <nox/util/pms_debug.h>
#include <nox/ecs/Entity.h>
#include <nox/ecs/Factory.h>
#include <nox/ecs/component/Types.h>
#include <nox/ecs/Event.h>
#include <nox/ecs/createEventArgument.h>

#include <string>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <fstream>

enum Type : std::size_t
{
    MOCK_COMPONENT = 2,
    OTHER_COMPONENT = 3,
};

enum class State : std::size_t
{
    HIBERNATION,
    DEACTIVATED,
    ACTIVE,
};

void
testIdentifier()
{
    nox::ecs::TypeIdentifier equalIdentifiers1("Test1");
    nox::ecs::TypeIdentifier equalIdentifiers2("Test1");
    assert(equalIdentifiers1 == equalIdentifiers2 && "They are equal");
    assert(!(equalIdentifiers1 != equalIdentifiers2) && "They are not unEqual");

    nox::ecs::TypeIdentifier unEqualIdentifiers1("Test1");
    nox::ecs::TypeIdentifier unEqualIdentifiers2("Test2");
    assert(unEqualIdentifiers1 != unEqualIdentifiers2 && "They are unEqual");
    assert(!(unEqualIdentifiers1 == unEqualIdentifiers2) && "They are not equal");

    nox::ecs::TypeIdentifier equalNumeric1(2);
    nox::ecs::TypeIdentifier equalNumeric2(2);
    assert(equalNumeric1 == equalNumeric2 && "They are equal");
    assert(!(equalNumeric1 != equalNumeric2) && "They are not unEqual");

    nox::ecs::TypeIdentifier unEqualNumeric1(1);
    nox::ecs::TypeIdentifier unEqualNumeric2(2);
    assert(unEqualNumeric1 != unEqualNumeric2 && "They are unEqual");
    assert(!(unEqualNumeric1 == unEqualNumeric2) && "They are not equal");

    nox::ecs::TypeIdentifier assignEqual1("Test1");
    nox::ecs::TypeIdentifier assignEqual2 = assignEqual1;
    assert(assignEqual1 == assignEqual2 && "They are equal");
    assert(!(assignEqual1 != assignEqual2) && "They are not unEqual");
}

class B
{
public:

    std::string str{"********************************************************************************************************************************************************************************************************"};

};


class MockComponent
    : public nox::ecs::Component
{
public:
    //using nox::ecs::Component::Component;

    MockComponent(MockComponent&& source)
        : nox::ecs::Component(source.id, source.entityManager)
        , str(std::move(source.str))
        {
        }

    MockComponent& operator=(MockComponent&& source)
    {
        if (this != &source)
        {
            nox::ecs::Component::operator=(std::move(source));
            std::swap(str, source.str);
        }

        return *this;
    }

    MockComponent(const nox::ecs::EntityId& id, nox::ecs::EntityManager* manager)
        : nox::ecs::Component(id, manager)
        , str("MockComponent ")
        {

        }
    ~MockComponent()
    {
    }

    void update(const nox::Duration& deltaTime)
    {
        nox::ecs::TypeIdentifier type(size_t(0));
        //printf("update %zu %s\n", this->id, str.c_str());
        nox::ecs::Event broadcastEvent({"broadcastEvent"}, this->id);
        auto argument = nox::ecs::createEventArgument(std::string("value"), {"Argument"});
        broadcastEvent.addArgument({"Arg1"}, std::move(argument));
        this->entityManager->sendEntityEvent(std::move(broadcastEvent));

        nox::ecs::Event thisEvent({"thisEvent"}, this->id, this->id);
        auto arg = nox::ecs::createEventArgument(2,{"Argument"});
        thisEvent.addArgument({"Arg2"}, std::move(arg));
        this->entityManager->sendEntityEvent(std::move(thisEvent));

    }

    void awake()
    {
        str += "awake ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void activate()
    {
        str += "activate ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::ACTIVE;
    }
    
    void deactivate()
    {
        str += "deactivate ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void hibernate()
    {
        str += "hibernate ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::HIBERNATION;
    }

    void initialize(const Json::Value& value)
    {
        // auto val = value["value"].asInt();
        str += "initialize " + value["name"].asString();
        // 
        //PMS_DEBUG("%zu %s %d\n", id, str.c_str(), value["value"].asInt());
    }

    void receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
    {
        str += "receiveLogicEvent ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
    }

    void receiveEntityEvent(const nox::ecs::Event& event)
    {
        //str += "receiveComponentEvent ";
        if (event.getType() == nox::ecs::TypeIdentifier{"broadcastEvent"})
        {
            auto argument = static_cast<const std::string*>(event.getArgument({"Arg1"}).value());
            PMS_DEBUG("%zu %s\n", id, str.c_str());
            PMS_DEBUG("Received event from: %zu, type: %zu argument: %s\n", 
                      event.getSender(),
                      event.getType(),
                      argument->c_str());
        }
        else if (event.getType() == nox::ecs::TypeIdentifier{"thisEvent"})
        {
            auto argument = static_cast<const int*>(event.getArgument({"Arg2"}).value());
            PMS_DEBUG("Received event from: %zu, type: %zu argument: %d\n",
                      event.getSender(),
                      event.getType(),
                      *argument);
        }
    }
    

public:
    std::string str{};
    State state{State::HIBERNATION};
};

class OtherComponent
    : public nox::ecs::Component
{
public:
    OtherComponent(OtherComponent&& source)
        : nox::ecs::Component(source.id, source.entityManager)
        , str(std::move(source.str))
        {
        }

    OtherComponent& operator=(OtherComponent&& source)
    {
        if (this != &source)
        {
            nox::ecs::Component::operator=(std::move(source));
            std::swap(str, source.str);
        }

        return *this;
    }

    OtherComponent(const nox::ecs::EntityId& id, nox::ecs::EntityManager* manager)
        : nox::ecs::Component(id, manager)
        , str("Other Component ")
        {

        }
    ~OtherComponent()
    {
    }

    void update(const nox::Duration& deltaTime)
    {
        //printf("update %zu %s\n", this->id, str.c_str());
        //this->manager->
    }

    void awake()
    {
        str += "awake ";
        //printf("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void activate()
    {
        str += "activate ";
        //printf("%zu %s\n", id, str.c_str());
        state = State::ACTIVE;
    }
    
    void deactivate()
    {
        str += "deactivate ";
        //printf("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void hibernate()
    {
        str += "hibernate ";
        ///printf("%zu %s\n", id, str.c_str());
        state = State::HIBERNATION;
    }

    void initialize(const Json::Value& value)
    {
        str += "initialize " + value["name"].asString();
        //printf("%zu %s\n", id, str.c_str());
    }

    void receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
    {
        str += "receiveLogicEvent " + event->getType() + " ";
        //printf("%zu %s\n", id, str.c_str());
    }

    void receiveEntityEvent(const nox::ecs::Event& event)
    {
        PMS_DEBUG("I am: %zu, received from: %zu\n", this->id, event.getSender());
        //printf("%zu %s\n", id, str.c_str());
    }
    

public:
    std::string str{};
    State state{State::HIBERNATION};
};

void
test()
{
    // using namespace nox::ecs;
    // nox::Duration duration;

    // const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});
    // char* buffer = new char[metaInfo.size];

    // Component* begin = (Component*)(buffer);
    // Component* end = (Component*)(&buffer[metaInfo.size]);
    // metaInfo.construct(begin, 1);
    // metaInfo.awake(begin);
    // metaInfo.activate(begin);
    // metaInfo.update(begin, end, duration);
    // metaInfo.deactivate(begin);
    // metaInfo.hibernate(begin);
    // metaInfo.initialize(begin, {});
    // metaInfo.receiveLogicEvent(begin, end,{});
    // //metaInfo.receiveEntityEvent(begin, end,{}); 

    // metaInfo.destruct((Component*)buffer);
    // delete[] buffer;
}

void
testingCollection()
{
    // using namespace nox::ecs;
    // nox::Duration duration;

    // auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    // metaInfo.awake;// = nullptr;
    // ComponentCollection collection(metaInfo);
    // collection.update(duration);

    // PMS_DEBUG("Creating\n");
    // for (std::size_t i = 0; i < 5; ++i)
    // {
    //     collection.create(i);
    // }
    // printf("\n");

    // PMS_DEBUG("Awaking\n");
    // for (std::size_t i = 0; i < collection.count(); ++i)
    // {
    //     collection.awake(i);
    // }
    // printf("\n");

    // PMS_DEBUG("Activating\n");
    // for (std::size_t i = 0; i < collection.count(); ++i)
    // {
    //     collection.activate(i);
    // }
    // printf("\n");

    // PMS_DEBUG("Updating\n");
    // collection.update(duration);
    // printf("\n");

    // PMS_DEBUG("Deactivating\n");
    // for (std::size_t i = 0; i < collection.count(); ++i)
    // {
    //     collection.deactivate(i);
    // }
    // printf("\n"); 

    // PMS_DEBUG("Hibernating\n");
    // for (std::size_t i = 0; i < collection.count(); ++i)
    // {
    //     collection.hibernate(i);
    // }
    // printf("\n"); 

    // PMS_DEBUG("Removing\n");
    // for (std::size_t i = 0; i < 5; ++i)
    // {
    //     collection.remove(i);
    // }
    // printf("\n"); 


    // printf("Count: %zu\n", collection.count());
}

void
testHandle()
{
    // using namespace nox::ecs;
    // const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    // ComponentCollection collection(metaInfo);
    // collection.create(0);

    // ComponentHandle<MockComponent> test = collection.getComponent(0);

    
    // for (std::size_t i = 1; i < 5; ++i)
    // {
    //     PMS_DEBUG("Count: %zu\n", collection.count());
    //     collection.create(i);
    //     NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
    // }


    // collection.remove(0);



    // collection.update({});

    // NOX_ASSERT(test.get() == nullptr, "ID is not nullptr, dangling pointer!, %zu", test->id);
    //printf("Passed %d", test.get() == nullptr);
    //
    //PMS_DEBUG("Capacity: %zu\n", collection.capacity() / metaInfo.size);
}

void
otherTest()
{
    // using namespace nox::ecs;
    // const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    // ComponentCollection collection(metaInfo);
    // collection.create(0);

    // ComponentHandle<MockComponent> test = collection.getComponent(0);

    // for (std::size_t i = 1; i < 100; ++i)
    // {
    //     collection.create(i);
    //     NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
    // }

    // collection.remove(0);


    // collection.update({});

    // NOX_ASSERT(test.get() == nullptr, "ID is not nullptr, dangling pointer!, %zu", test->id);
    // //printf("Passed %d", test.get() == nullptr);
    // //
}

void
testMove()
{
    // PMS_DEBUG("Starting Move\n");

    // using namespace nox::ecs;
    // const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    // PMS_DEBUG("Created MetaInfo\n");

    // ComponentCollection collection(metaInfo);

    // //PMS_DEBUG("Get component 1 MetaInfo");
    // //ComponentHandle<MockComponent> test = collection.getComponent(1);

    // PMS_DEBUG("Creating\n");
    // for (std::size_t i = 0; i < 100; ++i)
    // {
    //     //PMS_DEBUG("Create %zu\n", i);
    //     //PMS_DEBUG("Capacity: %zu\n", collection.capacity() / metaInfo.size);
    //     collection.update({});
    //     collection.create(i);
    // }

    // PMS_DEBUG("Removing\n");
    // collection.remove(0);

    // PMS_DEBUG("Before move constructor\n");
    // ComponentCollection collection2(std::move(collection));

    // PMS_DEBUG("After move constructor\n");
    // collection2.update({});

    // PMS_DEBUG("Before move assignment\n");
    // ComponentCollection collection3 = std::move(collection2);

    // PMS_DEBUG("After move assignment\n");
    // collection3.update({});

}
template<class T> 
void
print_info(const char* name)
{
    std::printf("Name: %s\nSize: %zu\nAlignment: %zu\n", name, sizeof(T), alignof(T));
}

#define PRINT_TYPE_INFO(arg) print_info<arg>(#arg);

void
testEntityManager()
{
    using namespace nox::ecs;

    using MockHandle = ComponentHandle<MockComponent>;
    using OtherHandle = ComponentHandle<OtherComponent>;
    using TypeTag = TypeIdentifier;
    
    EntityManager manager;

    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_COMPONENT);
    auto otherInfo = createMetaInformation<OtherComponent>(Type::OTHER_COMPONENT);

    //otherInfo.awake = nullptr;

    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    manager.configureComponents();

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();

    manager.assignComponent(firstId, Type::MOCK_COMPONENT);
    manager.assignComponent(firstId, Type::OTHER_COMPONENT);

    manager.assignComponent(secondId, Type::MOCK_COMPONENT);


    manager.createStep();

    MockHandle mock1stHandle = manager.getComponent(firstId,
                                                    Type::MOCK_COMPONENT);

    OtherHandle other1stHandle = manager.getComponent(firstId,
                                                     Type::OTHER_COMPONENT);

    MockHandle mock2ndHandle = manager.getComponent(secondId,
                                                    Type::MOCK_COMPONENT);
    
    NOX_ASSERT(mock1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(other1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(mock2ndHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    
    manager.awakeEntity(firstId);
    manager.awakeComponent(secondId, Type::MOCK_COMPONENT);
    
    manager.awakeStep();
    NOX_ASSERT(mock1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(other1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(mock2ndHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::DEACTIVATED));

    manager.activateEntity(firstId);
    manager.activateComponent(secondId, Type::MOCK_COMPONENT);

    manager.activateStep();
    NOX_ASSERT(mock1stHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::ACTIVE));
    NOX_ASSERT(other1stHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::ACTIVE));
    NOX_ASSERT(mock2ndHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::ACTIVE));

//    auto event = std::make_shared<nox::event::Event>("Thing");

//    manager.onEvent(event);
    manager.step({});

    manager.deactivateEntity(firstId);
    manager.deactivateEntity(secondId);

    manager.deactivateStep();
    NOX_ASSERT(mock1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(other1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(mock2ndHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    
    manager.hibernateEntity(firstId);
    manager.hibernateEntity(secondId);
    manager.hibernateStep();
    NOX_ASSERT(mock1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(other1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(mock2ndHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::HIBERNATION));
   
    manager.removeEntity(secondId);
    manager.removeComponent(firstId,
                            Type::MOCK_COMPONENT);

    manager.removeStep(); 
    NOX_ASSERT(mock1stHandle.get() == nullptr,"Dangling pointer!\n");
    NOX_ASSERT(mock2ndHandle.get() == nullptr,"Dangling pointer!\n");    
    NOX_ASSERT(other1stHandle.get() != nullptr,"Wrong pointer deleted!\n");
}

void
entityTest()
{
using namespace nox::ecs;

    using MockHandle = ComponentHandle<MockComponent>;
    using OtherHandle = ComponentHandle<OtherComponent>;
    using TypeTag = TypeIdentifier;
    
    EntityManager manager;

    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_COMPONENT);
    auto otherInfo = createMetaInformation<OtherComponent>(Type::OTHER_COMPONENT);

    //otherInfo.awake = nullptr;

    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    manager.configureComponents();

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();

    Entity entity1(firstId, &manager);
    Entity entity2(secondId, &manager);

    entity1.assignComponent(Type::MOCK_COMPONENT);
    entity1.assignComponent(Type::OTHER_COMPONENT);

    entity2.assignComponent(Type::MOCK_COMPONENT);

    Entity entity3 = entity2;
    entity3.remove();

    manager.createStep();

    MockHandle mock1stHandle = entity1.getComponent(Type::MOCK_COMPONENT);

    OtherHandle other1stHandle = entity1.getComponent(Type::OTHER_COMPONENT);

    MockHandle mock2ndHandle = entity2.getComponent(Type::MOCK_COMPONENT);
    
    NOX_ASSERT(mock1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(other1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(mock2ndHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::HIBERNATION));
   
    entity1.awake();
    entity2.awakeComponent(Type::MOCK_COMPONENT); 
    
    manager.awakeStep();
    NOX_ASSERT(mock1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(other1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(mock2ndHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::DEACTIVATED));

    entity1.activate();
    entity2.activateComponent(Type::MOCK_COMPONENT);

    manager.activateStep();
    NOX_ASSERT(mock1stHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::ACTIVE));
    NOX_ASSERT(other1stHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::ACTIVE));
    NOX_ASSERT(mock2ndHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::ACTIVE));

//    auto event = std::make_shared<nox::event::Event>("Thing");

//    manager.onEvent(event);
    manager.step({});

    entity1.deactivate();
    entity2.deactivate();

    manager.deactivateStep();
    NOX_ASSERT(mock1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(other1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(mock2ndHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::DEACTIVATED));

    entity1.hibernate();
    entity2.hibernate();    

    manager.hibernateStep();
    NOX_ASSERT(mock1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(other1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(mock2ndHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::HIBERNATION));
  
    entity2.remove(); 
    //manager.removeEntity(entity2.getId());
    entity1.removeComponent(Type::MOCK_COMPONENT);

    manager.removeStep(); 
    NOX_ASSERT(mock1stHandle.get() == nullptr,"Dangling pointer!\n");
    NOX_ASSERT(mock2ndHandle.get() == nullptr,"Dangling pointer!\n");    
    NOX_ASSERT(other1stHandle.get() != nullptr,"Wrong pointer deleted!\n");
}

// int main() {
//     ifstream ifs("alice.json");
//     Json::Reader reader;
//     Json::Value obj;
//     reader.parse(ifs, obj); // reader can also read strings
//     cout << "Book: " << obj["book"].asString() << endl;
//     cout << "Year: " << obj["year"].asUInt() << endl;
//     const Json::Value& characters = obj["characters"]; // array of characters
//     for (int i = 0; i < characters.size(); i++){
//         cout << "    name: " << characters[i]["name"].asString();
//         cout << " chapter: " << characters[i]["chapter"].asUInt();
//         cout << endl;
//     }
// }


void
testFactory(const char* filepath)
{
    using namespace nox::ecs;

    std::ifstream file(filepath);
    if (!file)
    {
        PMS_DEBUG("Error, file not found\n");
        std::terminate();
    }

    Json::Reader reader;
    Json::Value value;
    reader.parse(file, value);
    
    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_COMPONENT);
    const auto otherInfo = createMetaInformation<OtherComponent>(Type::OTHER_COMPONENT);
    EntityManager manager;
    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);
    manager.registerComponent(createMetaInformation<Parent>(component_types::PARENT));
    manager.registerComponent(createMetaInformation<Children>(component_types::CHILDREN));


    PMS_DEBUG("Creating Definition\n");
    manager.createEntityDefinition(value);
    PMS_DEBUG("Created Definition\n");
    manager.createEntity("Entity");
    PMS_DEBUG("Created Entity\n");
    manager.createEntity("ExtendedEntity");
    PMS_DEBUG("Created ExtendedEntity\n");

    manager.createStep();

    PMS_DEBUG("Called Create step\n");
    ComponentHandle<Children> handle = manager.getComponent(0, component_types::CHILDREN);
    PMS_DEBUG("Component 0 has: %zu children\n", handle->size());
    ComponentHandle<MockComponent> child1MockHandle = manager.getComponent((**handle)[0], Type::MOCK_COMPONENT);
    ///ComponentHandle<MockComponent> child2MockHandle = manager.getComponent((**handle)[1], Type::MOCK_COMPONENT);

    PMS_DEBUG("child1MockHandle id: %zu name: %s\n", 
              child1MockHandle->id,
              child1MockHandle->str.c_str());
              //child2MockHandle->id,
              //child2MockHandle->str.c_str());
}

void
eventTest()
{
    using namespace nox::ecs;
    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_COMPONENT);
    const auto otherInfo = createMetaInformation<OtherComponent>(Type::OTHER_COMPONENT);
    EntityManager manager;
    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();
    manager.assignComponent(firstId, Type::MOCK_COMPONENT);
    manager.assignComponent(firstId, Type::OTHER_COMPONENT);
    manager.assignComponent(secondId, Type::OTHER_COMPONENT);
    manager.awakeEntity(firstId);
    manager.activateEntity(firstId);

    for(size_t i = 0; i < 10; ++i)
    {
        manager.step({});
    }

    PMS_DEBUG("event Test over\n");
}


void
eventTest()
{
    using namespace nox::ecs;
    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_COMPONENT);
    const auto otherInfo = createMetaInformation<OtherComponent>(Type::OTHER_COMPONENT);
    EntityManager manager;
    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();
    manager.assignComponent(firstId, Type::MOCK_COMPONENT);
    manager.assignComponent(firstId, Type::OTHER_COMPONENT);
    manager.assignComponent(secondId, Type::OTHER_COMPONENT);
    manager.awakeEntity(firstId);
    manager.activateEntity(firstId);

    for(size_t i = 0; i < 10; ++i)
    {
        manager.step({});
    }

    PMS_DEBUG("event Test over\n");
}



 int 
 main(int argc, char** argv)
 {
    // if (argc < 2)
    // {
    //     PMS_DEBUG("Enter filepath\n");
    //     return 1;
    // }
    // testingCollection();
    // testEntityManager();
    //entityTest();
    PMS_DEBUG("Event test\n");
    eventTest();
    //testFactory(argv[1]);
    // testHandle();
    // testMove();
    // otherTest();
    // //
    

//     testFactory(argv[1]);

	return 0;
}


