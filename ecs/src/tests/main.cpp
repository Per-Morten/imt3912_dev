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

#include <string>
#include <cassert>

enum Type : std::size_t
{
    MOCK_COMPONENT,
    OTHER_COMPONENT,
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
        : nox::ecs::Component(source.id)
        , str(std::move(source.str))
        {
        }

    MockComponent& operator=(MockComponent&& source)
    {
        if (this != &source)
        {
            std::swap(id, source.id);
            std::swap(str, source.str);
        }

        return *this;
    }

    MockComponent(const nox::ecs::EntityId& id)
        : nox::ecs::Component(id)
        , str("MockComponent ")
        {

        }
    ~MockComponent()
    {
    }

    void update(const nox::Duration& deltaTime)
    {
        printf("update %zu %s\n", this->id, str.c_str());
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
        str += "initialize ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
    }

    void receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
    {
        str += "receiveLogicEvent ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
    }

    void receiveComponentEvent(const nox::ecs::Event& event)
    {
        str += "receiveComponentEvent ";
        PMS_DEBUG("%zu %s\n", id, str.c_str());
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
        : nox::ecs::Component(source.id)
        , str(std::move(source.str))
        {
        }

    OtherComponent& operator=(OtherComponent&& source)
    {
        if (this != &source)
        {
            std::swap(id, source.id);
            std::swap(str, source.str);
        }

        return *this;
    }

    OtherComponent(const nox::ecs::EntityId& id)
        : nox::ecs::Component(id)
        , str("Other Component ")
        {

        }
    ~OtherComponent()
    {
    }

    void update(const nox::Duration& deltaTime)
    {
        printf("update %zu %s\n", this->id, str.c_str());
    }

    void awake()
    {
        str += "awake ";
        printf("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void activate()
    {
        str += "activate ";
        printf("%zu %s\n", id, str.c_str());
        state = State::ACTIVE;
    }
    
    void deactivate()
    {
        str += "deactivate ";
        printf("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void hibernate()
    {
        str += "hibernate ";
        printf("%zu %s\n", id, str.c_str());
        state = State::HIBERNATION;
    }

    void initialize(const Json::Value& value)
    {
        str += "initialize ";
        printf("%zu %s\n", id, str.c_str());
    }

    void receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
    {
        str += "receiveLogicEvent " + event->getType() + " ";
        printf("%zu %s\n", id, str.c_str());
    }

    void receiveComponentEvent(const nox::ecs::Event& event)
    {
        str += "receiveComponentEvent ";
        printf("%zu %s\n", id, str.c_str());
    }
    

public:
    std::string str{};
    State state{State::HIBERNATION};
};

void
test()
{
    using namespace nox::ecs;
    nox::Duration duration;

    const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});
    char* buffer = new char[metaInfo.size];

    Component* begin = (Component*)(buffer);
    Component* end = (Component*)(&buffer[metaInfo.size]);
    metaInfo.construct(begin, 1);
    metaInfo.awake(begin);
    metaInfo.activate(begin);
    metaInfo.update(begin, end, duration);
    metaInfo.deactivate(begin);
    metaInfo.hibernate(begin);
    metaInfo.initialize(begin, {});
    metaInfo.receiveLogicEvent(begin, end,{});
    metaInfo.receiveComponentEvent(begin, end,{}); 

    metaInfo.destruct((Component*)buffer);
    delete[] buffer;
}

void
testingCollection()
{
    using namespace nox::ecs;
    nox::Duration duration;

    auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    metaInfo.awake;// = nullptr;
    ComponentCollection collection(metaInfo);
    collection.update(duration);

    PMS_DEBUG("Creating\n");
    for (std::size_t i = 0; i < 5; ++i)
    {
        collection.create(i);
    }
    printf("\n");

    PMS_DEBUG("Awaking\n");
    for (std::size_t i = 0; i < collection.count(); ++i)
    {
        collection.awake(i);
    }
    printf("\n");

    PMS_DEBUG("Activating\n");
    for (std::size_t i = 0; i < collection.count(); ++i)
    {
        collection.activate(i);
    }
    printf("\n");

    PMS_DEBUG("Updating\n");
    collection.update(duration);
    printf("\n");

    PMS_DEBUG("Deactivating\n");
    for (std::size_t i = 0; i < collection.count(); ++i)
    {
        collection.deactivate(i);
    }
    printf("\n"); 

    PMS_DEBUG("Hibernating\n");
    for (std::size_t i = 0; i < collection.count(); ++i)
    {
        collection.hibernate(i);
    }
    printf("\n"); 

    PMS_DEBUG("Removing\n");
    for (std::size_t i = 0; i < 5; ++i)
    {
        collection.remove(i);
    }
    printf("\n"); 


    printf("Count: %zu\n", collection.count());
}

void
testHandle()
{
    using namespace nox::ecs;
    const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    ComponentCollection collection(metaInfo);
    collection.create(0);

    ComponentHandle<MockComponent> test = collection.getComponent(0);

    
    for (std::size_t i = 1; i < 5; ++i)
    {
        PMS_DEBUG("Count: %zu\n", collection.count());
        collection.create(i);
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
    }


    collection.remove(0);



    collection.update({});

    NOX_ASSERT(test.get() == nullptr, "ID is not nullptr, dangling pointer!, %zu", test->id);
    //printf("Passed %d", test.get() == nullptr);
    //
    //PMS_DEBUG("Capacity: %zu\n", collection.capacity() / metaInfo.size);
}

void
otherTest()
{
    using namespace nox::ecs;
    const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    ComponentCollection collection(metaInfo);
    collection.create(0);

    ComponentHandle<MockComponent> test = collection.getComponent(0);

    for (std::size_t i = 1; i < 100; ++i)
    {
        collection.create(i);
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
    }

    collection.remove(0);


    collection.update({});

    NOX_ASSERT(test.get() == nullptr, "ID is not nullptr, dangling pointer!, %zu", test->id);
    //printf("Passed %d", test.get() == nullptr);
    //
}

void
testMove()
{
    PMS_DEBUG("Starting Move\n");

    using namespace nox::ecs;
    const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});

    PMS_DEBUG("Created MetaInfo\n");

    ComponentCollection collection(metaInfo);

    //PMS_DEBUG("Get component 1 MetaInfo");
    //ComponentHandle<MockComponent> test = collection.getComponent(1);

    PMS_DEBUG("Creating\n");
    for (std::size_t i = 0; i < 100; ++i)
    {
        //PMS_DEBUG("Create %zu\n", i);
        //PMS_DEBUG("Capacity: %zu\n", collection.capacity() / metaInfo.size);
        collection.update({});
        collection.create(i);
    }

    PMS_DEBUG("Removing\n");
    collection.remove(0);

    PMS_DEBUG("Before move constructor\n");
    ComponentCollection collection2(std::move(collection));

    PMS_DEBUG("After move constructor\n");
    collection2.update({});

    PMS_DEBUG("Before move assignment\n");
    ComponentCollection collection3 = std::move(collection2);

    PMS_DEBUG("After move assignment\n");
    collection3.update({});

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

 int 
 main(int argc, char** argv)
 {
//     using namespace nox::ecs;
//     PRINT_TYPE_INFO(MetaInformation);
//     PRINT_TYPE_INFO(TypeIdentifier);
//     PRINT_TYPE_INFO(std::size_t);
//     PRINT_TYPE_INFO(float);
//     PRINT_TYPE_INFO(void*);
//     PRINT_TYPE_INFO(std::vector<nox::event::Event::IdType>);
    testingCollection();
    testEntityManager();
    //PMS_DEBUG("Hey\n");
    testHandle();
    //PMS_DEBUG("Finish TestHandle");
    testingCollection();
    testHandle();
    testMove();
    otherTest();
    //

    PMS_DEBUG("Exiting cleanly\n");    

	return 0;
}
