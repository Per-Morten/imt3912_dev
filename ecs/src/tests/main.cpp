#include <nox/ecs/Component.h>
#include <nox/ecs/OperationTypes.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/createMetaInformation.h>

#include <string>
#include <cassert>

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

class MockComponent
    : public nox::ecs::Component
{
public:
    using nox::ecs::Component::Component;

    void update(const nox::Duration& deltaTime)
    {
        printf("update\n");
    }

    void awake()
    {
        printf("awake\n");
    }

    void activate()
    {
        printf("activate\n");
    }
    
    void deactivate()
    {
        printf("deactivate\n");
    }

    void hibernate()
    {
        printf("hibernate\n");
    }

    void initialize(const Json::Value& value)
    {
        printf("initialize\n");
    }

    void receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
    {
        printf("receiveLogicEvent\n");
    }

    void receiveComponentEvent(const nox::ecs::Event& event)
    {
        printf("receiveComponentEvent\n");
    }
    

private:
    std::string test;
};

class Other
{

};

int main(int argc, char** argv)
{
    using namespace nox::ecs;

    nox::Duration duration;

    const auto metaInfo = nox::ecs::createMetaInformation<MockComponent>({"MockComponent"});
    char* buffer = new char[metaInfo.sizeOfType];

    Component* begin = (Component*)(buffer);
    Component* end = (Component*)(&buffer[metaInfo.sizeOfType]);
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

    printf("Tests passed\n");
	return 0;
}
