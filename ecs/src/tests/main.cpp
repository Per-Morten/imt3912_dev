// More a scratchpad for prototyping than main. Hence the mess.

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
#include <nox/ecs/ComponentType.h>
#include <nox/ecs/Event.h>
#include <nox/ecs/createEventArgument.h>
#include <nox/thread/LockedQueue.h>
#include <nox/thread/Pool.h>
//#include <nox/ecs/component/Transform.h>
//#include <nox/memory/LockFreeAllocator.h>
//#include <nox/memory/HeapAllocator.h>
//#include <nox/memory/FixedSlotPool.h>
//#include <nox/ecs/EventSystem.h>

#include <thread>
#include <string>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <atomic>
#include <chrono>

enum Type : std::size_t
{
    MOCK_01 = 1000,
    MOCK_02 = 1001,
    MOCK_03 = 1002,
    MOCK_04 = 1003,
    MOCK_05 = 1004,
    MOCK_06 = 1005,
    MOCK_07 = 1006,
    MOCK_08 = 1007,
    MOCK_09 = 1008,
    MOCK_10 = 1009,
    MOCK_COUNT = MOCK_10 + 1,
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

    //nox::ecs::TypeIdentifier assignEqual1("Test1");
    //nox::ecs::TypeIdentifier assignEqual2 = assignEqual1;
    //assert(assignEqual1 == assignEqual2 && "They are equal");
    //assert(!(assignEqual1 != assignEqual2) && "They are not unEqual");
}

class B
{
public:

    std::string str{"********************************************************************************************************************************************************************************************************"};

};

struct TestEvent
{
    char var[1024];
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
        for (volatile int i = 0; i < 1000; ++i)
        {

        }

        //auto broadcastEvent = this->entityManager->createEntityEvent({"broadcastEvent"}, this->id);
        //nox::ecs::createEventArgument(broadcastEvent, std::string("value"), {"Arg1"});
        //this->entityManager->sendEvent(std::move(broadcastEvent));

        //printf("Updating %zu Mock\n", this->id);
        // nox::ecs::TypeIdentifier type(size_t(0));
        // //printf("update %zu %s\n", this->id, str.c_str());
        //nox::ecs::Event broadcastEvent = this->entityManager->createEntityEvent({"broadcastEvent"}, this->id);
        //nox::ecs::createEventArgument(broadcastEvent, std::string("value"), {"Arg1"});
        //this->entityManager->sendEntityEvent(std::move(broadcastEvent));

        nox::ecs::Event thisEvent = this->entityManager->createEntityEvent({"thisEvent"}, this->id);
        //nox::ecs::createEventArgument(thisEvent,2,{"Arg2"});
        nox::ecs::createEventArgument(thisEvent,std::string("THIS EVENT IS SO HUGE IT HAS TO DO HEAP ALLOCATION"), {"string"});
        this->entityManager->sendEntityEvent(std::move(thisEvent));

    }

    void awake()
    {
        str += "awake ";
        //PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void activate()
    {
        str += "activate ";
        //PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::ACTIVE;
    }

    void deactivate()
    {
        str += "deactivate ";
        //PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::DEACTIVATED;
    }

    void hibernate()
    {
        str += "hibernate ";
        //PMS_DEBUG("%zu %s\n", id, str.c_str());
        state = State::HIBERNATION;
    }

    // void initialize(const Json::Value& value)
    // {
    //     // auto val = value["value"].asInt();
    //     str += "initialize " + value["name"].asString();
    //     //
    //     //PMS_DEBUG("%zu %s %d\n", id, str.c_str(), value["value"].asInt());
    // }

    void receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
    {
        str += "receiveLogicEvent ";
        //PMS_DEBUG("%zu %s\n", id, str.c_str());
    }

    // void receiveEntityEvent(const nox::ecs::Event& event)
    // {

    //     if (event.getReceiver() != id && event.getReceiver() != nox::ecs::Event::BROADCAST)
    //     {
    //         //return;
    //     }
    //     else
    //     {
    //         messagesReceived++;
    //     }

    //     // for (volatile int i = 0; i < 1000; ++i)
    //     // {

    //     // }
    //     // if (event.getType() != nox::ecs::TypeIdentifier{"TransformChange"})
    //     // {
    //     //     return;
    //     // }
    //     // else
    //     // {
    //     //     //PMS_DEBUG("Received TransformChange\n");
    //     //     return;
    //     // }
    //     // //str += "receiveComponentEvent ";
    //     // if (event.getType() == nox::ecs::TypeIdentifier{"broadcastEvent"})
    //     // {
    //     //     auto argument = event.getArgument({"Arg1"}).value<const std::string*>();
    //     //     PMS_DEBUG("%zu %s\n", id, str.c_str());
    //     //     PMS_DEBUG("Received event from: %zu, type: %zu argument: %s\n",
    //     //               event.getSender(),
    //     //               event.getType(),
    //     //               argument->c_str());
    //     // }
    //     // else if (event.getType() == nox::ecs::TypeIdentifier{"thisEvent"})
    //     // {
    //     //     //messagesReceived++;
    //     //     auto argument = event.getArgument({"string"}).value<const std::string*>();
    //     //     PMS_DEBUG("Received event from: %zu, type: %zu argument: %s\n",
    //     //               event.getSender(),
    //     //               event.getType(),
    //     //               argument->c_str());
    //     // }
    // }


public:
    int messagesReceived{};
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
        for (volatile int i = 0; i < 1000; ++i)
        {

        }

        nox::ecs::Event thisEvent = this->entityManager->createEntityEvent({"thisEvent2"}, this->id);
        //nox::ecs::createEventArgument(thisEvent,2,{"Arg2"});
        nox::ecs::createEventArgument(thisEvent,std::string("THIS EVENT IS SO HUGE IT HAS TO DO HEAP ALLOCATION"), {"string"});
        this->entityManager->sendEntityEvent(std::move(thisEvent));

        // CREATE EVENT HERE!
        //printf("Update %zu Other\n", this->id);
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
        if (event.getReceiver() != id && event.getReceiver() != nox::ecs::Event::BROADCAST)
        {
            //return;
        }
        else
        {
            messagesReceived++;
        }

        // if (event.getType() == nox::ecs::TypeIdentifier{"thisEvent"})
        // {
        //     //messagesReceived++;
        //     auto argument = event.getArgument({"string"}).value<std::string>();
        //     PMS_DEBUG("Received event from: %zu, type: %zu argument: %s\n",
        //               event.getSender(),
        //               event.getType(),
        //               argument.c_str());
        // }


        for (volatile int i = 0; i < 1000; ++i)
        {

        }


        // if (event.getType() != nox::ecs::TypeIdentifier{"TransformChange"})
        // {
        //     return;
        // }
        // else
        // {
        //     PMS_DEBUG("Received TransformChange\n");

        //     const auto position = static_cast<const glm::vec2*>(event.getArgument({"Position"}).value());
        //     const auto rotation = static_cast<const glm::vec2*>(event.getArgument({"Rotation"}).value());
        //     const auto scale = static_cast<const float*>(event.getArgument({"Scale"}).value());


        //     PMS_DEBUG("Position: x: %f y: %f\n", position->x, position->y);
        //     PMS_DEBUG("Rotation: x: %f y: %f\n", rotation->x, rotation->y);
        //     PMS_DEBUG("Scale: %f\n", *scale);
        // }
        //PMS_DEBUG("I am: %zu, received from: %zu\n", this->id, event.getSender());
        //printf("%zu %s\n", id, str.c_str());
    }


public:
    int messagesReceived{};
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
    constexpr std::size_t maxComponents = 100;
    using namespace nox::ecs;
    const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>(Type::MOCK_01);

    EntityManager entityManager;
    entityManager.registerComponent(metaInfo);
    entityManager.configureComponents();

    entityManager.createEntity();
    entityManager.assignComponent(0, Type::MOCK_01);
    entityManager.createStep();

    ComponentHandle<MockComponent> test = entityManager.getComponent(0, Type::MOCK_01);
    std::vector<EntityId> idsToWorkWith(maxComponents);
    std::iota(std::begin(idsToWorkWith), std::end(idsToWorkWith), 0);

    const auto existsIn = [](const std::vector<EntityId>& vec, const auto& target)
    {
        return std::any_of(std::cbegin(vec),
                           std::cend(vec),
                           [target](const auto& item)
                           {
                            return item == target;
                           });
    };

    const auto eraseIf = [](std::vector<EntityId>& vec, const auto& predicate)
    {
        vec.erase(std::remove_if(std::begin(vec),
                                 std::end(vec),
                                 predicate),
                  std::cend(vec));
    };

    // This test is wrong!
    // I should create a vector up front that I eliminate numbers from, rather than doing this.
    // the modulus turns out wrong.

    for (std::size_t i = 1; i < maxComponents; ++i)
    {
        //PMS_DEBUG("Count: %zu\n", entityManager.count());
        entityManager.createEntity();
        entityManager.assignComponent(i, Type::MOCK_01);
        entityManager.createStep();
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
        NOX_ASSERT(test->state == State::HIBERNATION, "Wrong state!");
    }

    eraseIf(idsToWorkWith, [](const auto& item) { return item != 0 && item % 17 == 0; });

    for (std::size_t i = 0; i < maxComponents; ++i)
    {
        if (existsIn(idsToWorkWith, i))
        {
            entityManager.awakeComponent(i, Type::MOCK_01);
        }
        entityManager.awakeStep();
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
        NOX_ASSERT(test->state == State::DEACTIVATED, "Wrong state!");
    }

    eraseIf(idsToWorkWith, [](const auto& item) { return item != 0 && item % 15 == 0; });

    for (std::size_t i = 0; i < maxComponents; ++i)
    {
        if (existsIn(idsToWorkWith, i))
        {
            entityManager.activateComponent(i, Type::MOCK_01);
        }
        entityManager.activateStep();
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
        NOX_ASSERT(test->state == State::ACTIVE, "Wrong state!");
    }

    eraseIf(idsToWorkWith, [](const auto& item) { return item != 0 && item % 12 == 0; });


    for (std::size_t i = 0; i < maxComponents; ++i)
    {
        if (existsIn(idsToWorkWith, i))
        {
            entityManager.deactivateComponent(i, Type::MOCK_01);
        }
        entityManager.deactivateStep();
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
        NOX_ASSERT(test->state == State::DEACTIVATED, "Wrong state!");
    }

    eraseIf(idsToWorkWith, [](const auto& item) { return item != 0 && item % 9 == 0; });

    for (std::size_t i = 0; i < maxComponents; ++i)
    {
        if (existsIn(idsToWorkWith, i))
        {
            entityManager.hibernateComponent(i, Type::MOCK_01);
        }
        entityManager.hibernateStep();
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
        NOX_ASSERT(test->state == State::HIBERNATION, "Wrong state!");
    }

    eraseIf(idsToWorkWith, [](const auto& item) { return item != 0 && item % 9 == 0; });

    for (std::size_t i = 1; i < maxComponents; ++i)
    {
        if (existsIn(idsToWorkWith, i))
        {
            entityManager.removeComponent(i, Type::MOCK_01);
        }
        entityManager.removeStep();
        NOX_ASSERT(test->id == 0, "ID did not match! Iterator is not mapped to correct component");
    }

    eraseIf(idsToWorkWith, [](const auto& item) { return item != 0 && item % 6 == 0; });


    entityManager.removeComponent(0, Type::MOCK_01);

    entityManager.removeStep();

    NOX_ASSERT(test.get() == nullptr, "ID is not nullptr, dangling pointer!, %zu", test->id);
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
//    using TypeTag = TypeIdentifier;

    EntityManager manager;

    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_01);
    auto otherInfo = createMetaInformation<OtherComponent>(Type::MOCK_02);

    //otherInfo.awake = nullptr;

    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    manager.configureComponents();

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();

    manager.assignComponent(firstId, Type::MOCK_01);
    manager.assignComponent(firstId, Type::MOCK_02);

    manager.assignComponent(secondId, Type::MOCK_01);


    manager.createStep();

    MockHandle mock1stHandle = manager.getComponent(firstId,
                                                    Type::MOCK_01);

    OtherHandle other1stHandle = manager.getComponent(firstId,
                                                      Type::MOCK_02);

    MockHandle mock2ndHandle = manager.getComponent(secondId,
                                                    Type::MOCK_01);

    NOX_ASSERT(mock1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(other1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(mock2ndHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::HIBERNATION));

    manager.awakeEntity(firstId);
    manager.awakeComponent(secondId, Type::MOCK_01);

    manager.awakeStep();
    NOX_ASSERT(mock1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(other1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(mock2ndHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::DEACTIVATED));

    manager.activateEntity(firstId);
    manager.activateComponent(secondId, Type::MOCK_01);

    manager.activateStep();
    NOX_ASSERT(mock1stHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::ACTIVE));
    NOX_ASSERT(other1stHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::ACTIVE));
    NOX_ASSERT(mock2ndHandle->state == State::ACTIVE,"Incorrect state %zu, should be: State::ACTIVE %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::ACTIVE));

//    auto event = std::make_shared<nox::event::Event>("Thing");

//    manager.onEvent(event);
    for (std::size_t i = 0; i < 10000; ++i)
    {
        manager.step({});
    }

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
                            Type::MOCK_01);

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
  //  using TypeTag = TypeIdentifier;

    EntityManager manager;

    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_01);
    auto otherInfo = createMetaInformation<OtherComponent>(Type::MOCK_02);

    //otherInfo.awake = nullptr;

    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    manager.configureComponents();

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();

    Entity entity1(firstId, &manager);
    Entity entity2(secondId, &manager);

    entity1.assignComponent(Type::MOCK_01);
    entity1.assignComponent(Type::MOCK_02);

    entity2.assignComponent(Type::MOCK_01);

    Entity entity3 = entity2;
    entity3.remove();

    manager.createStep();

    MockHandle mock1stHandle = entity1.getComponent(Type::MOCK_01);

    OtherHandle other1stHandle = entity1.getComponent(Type::MOCK_02);

    MockHandle mock2ndHandle = entity2.getComponent(Type::MOCK_01);

    NOX_ASSERT(mock1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(other1stHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::HIBERNATION));
    NOX_ASSERT(mock2ndHandle->state == State::HIBERNATION,"Incorrect state %zu, should be: State::HIBERNATION %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::HIBERNATION));

    entity1.awake();
    entity2.awakeComponent(Type::MOCK_01);

    manager.awakeStep();
    NOX_ASSERT(mock1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(other1stHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(other1stHandle->state), static_cast<std::size_t>(State::DEACTIVATED));
    NOX_ASSERT(mock2ndHandle->state == State::DEACTIVATED,"Incorrect state %zu, should be: State::DEACTIVATED %zu\n", static_cast<std::size_t>(mock2ndHandle->state), static_cast<std::size_t>(State::DEACTIVATED));

    entity1.activate();
    entity2.activateComponent(Type::MOCK_01);

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
    entity1.removeComponent(Type::MOCK_01);

    manager.removeStep();
    NOX_ASSERT(mock1stHandle.get() == nullptr,"Dangling pointer!\n");
    NOX_ASSERT(mock2ndHandle.get() == nullptr,"Dangling pointer!\n");
    NOX_ASSERT(other1stHandle.get() != nullptr,"Wrong pointer deleted!\n");
}

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

    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_01);
    const auto otherInfo = createMetaInformation<OtherComponent>(Type::MOCK_02);
    EntityManager manager;
    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);
    manager.registerComponent(createMetaInformation<Parent>({component_type::PARENT}));
    manager.registerComponent(createMetaInformation<Children>({component_type::CHILDREN}));


    PMS_DEBUG("Creating Definition\n");
    manager.createEntityDefinition(value);
    PMS_DEBUG("Created Definition\n");
    manager.createEntity("Entity");
    PMS_DEBUG("Created Entity\n");
    manager.createEntity("ExtendedEntity");
    PMS_DEBUG("Created ExtendedEntity\n");



    manager.createStep();

    PMS_DEBUG("Called Create step\n");
    ComponentHandle<Children> handle = manager.getComponent(0, {component_type::CHILDREN});
    PMS_DEBUG("Component 0 has: %zu children\n", handle->size());
    ComponentHandle<MockComponent> child1MockHandle = manager.getComponent((**handle)[0], Type::MOCK_01);
    ///ComponentHandle<MockComponent> child2MockHandle = manager.getComponent((**handle)[1], Type::MOCK_01);

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
    const auto mockInfo = createMetaInformation<MockComponent>(Type::MOCK_01);
    const auto otherInfo = createMetaInformation<OtherComponent>(Type::MOCK_02);
    EntityManager manager;
    manager.registerComponent(mockInfo);
    manager.registerComponent(otherInfo);

    auto firstId = manager.createEntity();
    auto secondId = manager.createEntity();
    manager.assignComponent(firstId, Type::MOCK_01);
    manager.assignComponent(firstId, Type::MOCK_02);
    manager.assignComponent(secondId, Type::MOCK_02);
    manager.awakeEntity(firstId);
    manager.activateEntity(firstId);

    for(size_t i = 0; i < 10; ++i)
    {
        manager.step({});
    }

    PMS_DEBUG("event Test over\n");
}

void
testLockedQueue()
{
    const std::size_t pushesPerThread = 1000000;
    nox::thread::LockedQueue<std::size_t> queue;

    std::vector<std::thread> threads(8);

    for (auto& thread : threads)
    {
        thread = std::thread([&queue, pushesPerThread]()
        {
            for(std::size_t i = 0; i < pushesPerThread; ++i)
            {
                queue.push(1);
            }
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    PMS_DEBUG("Thread size: %zu\n", queue.size());

    std::atomic<std::size_t> sum{0};
    for (auto& thread : threads)
    {
        thread = std::thread([&queue, &sum]()
        {
            std::size_t value;
            while (queue.pop(value))
            {
                sum += value;
            }
        });
    }


    for (auto& thread : threads)
    {
        thread.join();
    }

    PMS_DEBUG("Sum: %zu\n", sum.load());
}

void
testPool()
{
    nox::thread::Pool<nox::thread::LockedQueue> pool;
    const std::size_t addCount = 1000;
    const std::size_t multiplier = 100;
    std::atomic<int> count{0};

    for (std::size_t i = 0; i < pool.threadCount() * multiplier; ++i)
    {
        const auto task = [addCount, &count]()
        {
            for(std::size_t i = 0; i < addCount; ++i)
            {
                std::atomic_fetch_add_explicit(&count, 1, std::memory_order_seq_cst);
            }
        };

        pool.addTask(task);
    }

    //PMS_DEBUG("Starting\n");
    pool.wait();
    PMS_DEBUG("Count: %d\n", std::atomic_load(&count));

    for (std::size_t i = 0; i < (pool.threadCount() * multiplier) / 2; ++i)
    {
        const auto task = [addCount, &count]()
        {
            for(std::size_t i = 0; i < addCount; ++i)
            {
                std::atomic_fetch_sub_explicit(&count, 1, std::memory_order_seq_cst);
            }
        };

        pool.addTask(task);
    }

    pool.wait();
    //PMS_DEBUG("Count: %d\n", std::atomic_load(&count));

    for (std::size_t i = 0; i < (pool.threadCount() * multiplier) / 2; ++i)
    {
        const auto task = [addCount, &count]()
        {
            for(std::size_t i = 0; i < addCount; ++i)
            {
                std::atomic_fetch_sub_explicit(&count, 1, std::memory_order_seq_cst);
            }
        };

        pool.addTask(task);
    }

    pool.wait();
    PMS_DEBUG("Count: %d\n", std::atomic_load(&count));
    //PMS_DEBUG("Exit scope\n");
}


// Run together with valgrind, to ensure that all is freed.
void
testAllocator()
{

    // nox::memory::HeapAllocator allocator;

    // int32_t** array = nullptr;
    // array = (int32_t**)allocator.allocate(100 * sizeof(int32_t*));
    // for (std::size_t i = 0; i < 100; ++i)
    // {
    //     array[i] = (int32_t*)allocator.allocate(100 * sizeof(int32_t));
    // }

    // for (std::size_t i = 0; i < 100; ++i)
    // {
    //     for (std::size_t j = 0; j < 100; ++j)
    //     {
    //         array[i][j] = i;
    //     }
    // }

    // for (std::size_t i = 0; i < 100; ++i)
    // {
    //     for (std::size_t j = 0; j < 100; ++j)
    //     {
    //         PMS_DEBUG("Value: %d\n", array[i][j]);
    //     }
    // }

    // for (std::size_t i = 0; i < 100; ++i)
    // {
    //     allocator.deallocate(array[i]);
    // }
    // allocator.deallocate(array);

    // PMS_DEBUG("Finished\n");
}

struct EventTest
{
    nox::ecs::EntityId receiverId;
    nox::ecs::EntityId senderId;
    nox::ecs::TypeIdentifier type{0};

    void* a;
    void* b;
    void* c;

    const char* c_str()
    {
        return " TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST ";
    }

};

void
testLockedPool(const std::size_t iterations, const std::size_t numberOfElements, const std::size_t randomUniquePtrs)
{

    // using Type = EventTest;
    // std::vector<volatile Type*> array(numberOfElements);
    // nox::memory::LinearAllocator<sizeof(Type) * 128> allocator;

    // for (std::size_t a = 0; a < iterations; ++a)
    // {
    //     for (std::size_t i = 0; i < numberOfElements / 2; ++i)
    //     {
    //         auto tmp = allocator.allocate(sizeof(Type));
    //         new (tmp)Type();
    //         array[i] = (volatile Type*)tmp;
    //     }

    //     std::vector<std::unique_ptr<volatile std::string>> randomMemory;
    //     for (std::size_t i = 0; i < randomUniquePtrs; ++i)
    //     {
    //         randomMemory.push_back(std::make_unique<std::string>("TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING"));
    //     }

    //     for (std::size_t i = numberOfElements / 2; i < numberOfElements; ++i)
    //     {
    //         auto tmp = allocator.allocate(sizeof(Type));
    //         new (tmp)Type();
    //         array[i] = (volatile Type*)tmp;
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         volatile auto temp = array[i];
    //     }

    //     for (const auto& item : randomMemory)
    //     {
    //         volatile auto& temp = item;
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         volatile auto temp = array[i];
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         auto tmp = array[i];
    //         tmp->~Type();
    //     }
    //     allocator.clear();
    // }

}

void
testVSPool(const std::size_t iterations, const std::size_t numberOfElements,  const std::size_t randomUniquePtrs)
{

    // using Type = EventTest;
    // std::vector<volatile Type*> array(numberOfElements);

    // for (std::size_t a = 0; a < iterations; ++a)
    // {
    //     for (std::size_t i = 0; i < numberOfElements / 2; ++i)
    //     {
    //         auto tmp = std::malloc(sizeof(Type));
    //         new (tmp)Type();
    //         array[i] = (volatile Type*)tmp;
    //     }

    //     std::vector<std::unique_ptr<volatile std::string>> randomMemory;
    //     for (std::size_t i = 0; i < randomUniquePtrs; ++i)
    //     {
    //         randomMemory.push_back(std::make_unique<std::string>("TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING"));
    //     }

    //     for (std::size_t i = numberOfElements / 2; i < numberOfElements; ++i)
    //     {
    //         auto tmp = std::malloc(sizeof(Type));
    //         new (tmp)Type();
    //         array[i] = (volatile Type*)tmp;
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         volatile auto temp = array[i];
    //     }

    //     for (const auto& item : randomMemory)
    //     {
    //         volatile auto& temp = item;
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         volatile auto temp = array[i];
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         auto tmp = array[i];
    //         tmp->~Type();
    //     }

    //     for (std::size_t i = 0; i < numberOfElements; ++i)
    //     {
    //         auto temp = (void*)array[i];
    //         std::free(temp);
    //     }
    // }

}


void
testEventSystem()
{
    // nox::ecs::EventSystem system;

    // for (std::size_t j = 0; j < 1000; ++j)
    // {
    //     for (std::size_t i = 0; i < 1000; ++i)
    //     {
    //         nox::ecs::Event& event = system.createEvent(0,0,0);
    //         nox::ecs::createEventArgument(event,i, {"test"});
    //         nox::ecs::createEventArgument(event,EventTest(), {"Event"});
    //     }

    //     while (!system.empty())
    //     {
    //         auto& eventPopped = system.readNextEvent();
    //         auto arg = (volatile int*)eventPopped.getArgument({"test"}).value();
    //         auto arg2 = (volatile EventTest*)eventPopped.getArgument({"Event"}).value();
    //     }

    //     system.clear();
    // }
}

float
testThreadedUpdate()
{
    constexpr std::size_t NUMBER_OF_ITERATIONS = 1;
    constexpr std::size_t MAX_ENTITIES = 100;
    using namespace nox::ecs;

    EntityManager manager;
    //const auto transformInfo = createMetaInformation<Transform>({component_type::TRANSFORM});
    //manager.registerComponent(transformInfo);

    //std::vector<ComponentHandle<Transform>> handles;
    //handles.reserve(MAX_ENTITIES);

    for (std::size_t i = Type::MOCK_01; i != Type::MOCK_COUNT; ++i)
    {
        if (i % 2 == 0)
        {
            auto mockInfo = createMetaInformation<MockComponent>(i);
            if (mockInfo.initialize)
            {
                PMS_DEBUG("Mock has initialize\n");
            }
            if (mockInfo.receiveEntityEvent)
            {
                PMS_DEBUG("Other has receiveEntityEvent\n");
            }

            mockInfo.updateAccess = DataAccess::READ_ONLY;
            mockInfo.receiveEntityEventAccess = DataAccess::READ_ONLY;
            for (std::size_t j = Type::MOCK_01; j != Type::MOCK_COUNT; ++j)
            {
                if (j % 5 == 0)
                {
                    mockInfo.updateDependencies.push_back(j);
                    mockInfo.receiveEntityEventDependencies.push_back(j);
                }
            }

            manager.registerComponent(mockInfo);
        }
        else
        {
            auto otherInfo = createMetaInformation<OtherComponent>(i);
            otherInfo.updateAccess = DataAccess::READ_ONLY;
            otherInfo.receiveEntityEventAccess = DataAccess::READ_ONLY;
            if (otherInfo.initialize)
            {
                PMS_DEBUG("Other has initialize\n");
            }
            if (otherInfo.receiveEntityEvent)
            {
                PMS_DEBUG("Other has receiveEntityEvent\n");
            }


            for (std::size_t j = Type::MOCK_01; j != Type::MOCK_COUNT; ++j)
            {
                if (j % 4 == 0)
                {
                    otherInfo.updateDependencies.push_back(j);
                    otherInfo.receiveEntityEventDependencies.push_back(j);

                }
            }
            manager.registerComponent(otherInfo);
        }
    }

    manager.configureComponents();
    PMS_DEBUG("Configured\n");

    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < MAX_ENTITIES / 2; ++i)
    {
        auto id = manager.createEntity();
        for (std::size_t j = Type::MOCK_01; j != Type::MOCK_COUNT; ++j)
        {
            manager.assignComponent(id, j);
        }
        //manager.assignComponent(id, {component_type::TRANSFORM});
        manager.awakeEntity(id);
        manager.activateEntity(id);
        //handles.push_back(manager.getComponent(id, {component_type::TRANSFORM}));
    }

    for (std::size_t i = 0; i < MAX_ENTITIES / 2; ++i)
    {
        auto id = manager.createEntity();
        for (std::size_t j = Type::MOCK_01; j != Type::MOCK_COUNT; ++j)
        {
            manager.assignComponent(id, j);
        }
        //manager.assignComponent(id, {component_types::TRANSFORM});
        manager.awakeEntity(id);
        manager.activateEntity(id);
        //handles.push_back(manager.getComponent(id, {component_types::TRANSFORM}));
    }

    manager.step({});

    //PMS_DEBUG("Update\n");
    for (volatile std::size_t i = 0; i < NUMBER_OF_ITERATIONS; ++i)
    {
        if (i % 100 == 0)
        {
            PMS_DEBUG("Updating %zu\n", i);
        }

        volatile EntityManager* m = &manager;


        manager.step({});
        // for (auto& handle : handles)
        // {
        //     handle->setPosition({0.0f, 0.0f});
        // }
    }
    PMS_DEBUG("Before deactivation\n");

    for (std::size_t i = 0; i < MAX_ENTITIES; ++i)
    {
        ComponentHandle<MockComponent> mock = manager.getComponent(i, Type::MOCK_01);
        ComponentHandle<OtherComponent> other = manager.getComponent(i, Type::MOCK_02);
        //PMS_DEBUG("Mock  Received: %d of events\n", mock->messagesReceived);//  NUMBER_OF_ITERATIONS * (MOCK_COUNT % 1000) - (MOCK_COUNT % 1000));
        if (i == 0)
        {
            PMS_DEBUG("Other Received: %d of %zu events\n", other->messagesReceived, NUMBER_OF_ITERATIONS * (MOCK_COUNT % 1000) - (MOCK_COUNT % 1000));
            PMS_DEBUG("Mock Received: %d of %zu events\n", mock->messagesReceived, NUMBER_OF_ITERATIONS * (MOCK_COUNT % 1000) - (MOCK_COUNT % 1000));

        }
        //NOX_ASSERT(mock->messagesReceived == NUMBER_OF_ITERATIONS * (MOCK_COUNT % 1000) - (MOCK_COUNT % 1000), "Component has not received enough events!");
        //NOX_ASSERT(other->messagesReceived == NUMBER_OF_ITERATIONS * (MOCK_COUNT % 1000) - (MOCK_COUNT % 1000), "Component has not received enough events!");

    }

    for (std::size_t i = 0; i < MAX_ENTITIES; ++i)
    {
        manager.deactivateEntity(i);
        manager.hibernateEntity(i);
        manager.removeEntity(i);
    }

    PMS_DEBUG("Last step\n");
    manager.step({});

    const auto stop = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float, std::chrono::seconds::period>(stop - start).count();
}

int
main(int argc, char** argv)
{
    // PRINT_TYPE_INFO(std::vector<std::thread>);
    // PRINT_TYPE_INFO(nox::thread::LockedQueue<std::function<void(void)>>);
    // PRINT_TYPE_INFO(std::atomic_size_t);
    // PRINT_TYPE_INFO(std::condition_variable);
    // PRINT_TYPE_INFO(std::mutex);
    // PRINT_TYPE_INFO(std::atomic_bool);
    //PRINT_TYPE_INFO(nox::thread::Pool<nox::thread::LockedQueue>);
    // if (argc < 2)
    // {
    //     PMS_DEBUG("Enter filepath\n");
    //     return 1;
    // }
    // testingCollection();
    // testEntityManager();
    //entityTest();
    //PMS_DEBUG("Event test\n");
    //eventTest();
    //testFactory(argv[1]);
    // testHandle();
    // testMove();
    // otherTest();
    //testPool();
    //testLockedQueue();
    //testAllocator();
    //
    // const auto timeFunc = [](const auto& func) -> float
    // {
    //     float totalTime = 0.0f;
    //     const auto start = std::chrono::high_resolution_clock::now();
    //     func(100, 1000, 1000);
    //     const auto stop = std::chrono::high_resolution_clock::now();
    //     totalTime += std::chrono::duration<float, std::chrono::milliseconds::period>(stop - start).count();

    //     return totalTime;
    // };

    // auto lockedTime = 0.0f;
    // auto vsTime = 0.0f;

    // for (std::size_t i = 0; i < 8; ++i)
    // {
    //     if (i % 2 == 0)
    //     {
    //         lockedTime += timeFunc(testLockedPool);
    //         vsTime += timeFunc(testVSPool);
    //     }
    //     else
    //     {
    //         lockedTime += timeFunc(testLockedPool);
    //         vsTime += timeFunc(testVSPool);
    //     }
    // }
    // PMS_DEBUG("Time LockedPool %f\n",lockedTime);
    // PMS_DEBUG("Time VsPool %f\n",vsTime);


    // const auto start = std::chrono::high_resolution_clock::now();
    // testEventSystem();
    // const auto stop = std::chrono::high_resolution_clock::now();

    // PMS_DEBUG("Time: %f\n", std::chrono::duration<float, std::chrono::milliseconds::period>(stop - start).count());

    // PMS_DEBUG("Test: testEntityManager\n");
    // testEntityManager();
    // PMS_DEBUG("\n");

    // PMS_DEBUG("Test: entityTest\n");
    // entityTest();
    // PMS_DEBUG("\n");

    // PMS_DEBUG("Test: testFactory\n");
    // testFactory(argv[1]);
    //

    testHandle();
    //const auto start = std::chrono::high_resolution_clock::now();
    auto time = testThreadedUpdate();
    //const auto stop = std::chrono::high_resolution_clock::now();
    PMS_DEBUG("Time: %f\n", time); //std::chrono::duration<float, std::chrono::seconds::period>(stop - start).count());

    return 0;
}

