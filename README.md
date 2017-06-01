# Description
NOX ECS is a project to create a new component system for Suttung Digitals [NOX Engine](https://gitlab.com/suttungdigital/nox-engine).  
The project was implemented by Trond Lohne and Per-Morten Straume, and was started as a bachelor thesis at NTNU Gjøvik 2017.
NOX ECS is still under development, and will in the future hopefully be merged into the NOX Engine as an alternative to their Actor system.  
The library was supposed to be a relatively lightweight component system with a focus on performance through cache friendliness
and multi-threading support.
The library was also supposed to avoid high increase in compile times by avoiding heavy template usage.
Additionally NOX ECS follows an "opt-in with consequences" philosophy. By default the library supports safe and familiar syntax,
however, the user can opt-in to features like multi-threading support, at the cost of less safety and potentially more boilerplate.

# Overview
NOX ECS is relatively simple component system, with each main "module" being described in the following sections.
Components can be read more about in Nystroms [Game Programming Patterns](http://gameprogrammingpatterns.com/component.html)

## Entities & Components
Entities are defined through components that share an entity id, which is a simple handle.
Components are small classes that implement domain specific functionality, like rendering sprites,
or they can be simple data, like positions.
All components are required to inherit from the Component base class, which adds an ID variable,
and a pointer to the relevant entity manager.
Inheriting the component also creates all the different hooks NOX ECS requires,
however, NOX ECS has its own way of handling dynamic dispatch, so no virtual pointer table is added.
Additional information of the dynamic dispatch implementation can be found in the documentation of the library.

### Hooks
Component functionality is added through hooks, which are functions that the entity manager
will call on the different components in different situations.
These are:
- initialize, which allows for initialization of a component through a JSON object.
- Lifecycle hooks. These are functions that are run when certain lifecycle transitions are requested.
    - awake
    - activate
    - deactivate
    - hibernate
- update, per frame update call, includes the deltatime. This function is only called on active objects.
- receiveLogicEvent, called when the component is receiving a logic event from the NOX Engine.
- receiveEntityEvent, called when the component is receiving an entity event from within NOX ECS.

It should be noted that because of the way NOX ECS handles dynamic dispatch, non "overloaded" functions
are removed. Meaning that if a component does not supply its own update method, it is never scheduled
for updates.

### Lifecycle
Entities go through different stages of a lifecycle, which indicates what functionality that is offered.
The different stages are:
- Hibernation
- Inactive
- Active

Transitioning between the different stages are done with simple function calls:
- Hibernation to Inactive: awake
- Inactive to Active: activate
- Active to Inactive: deactivate
- Inactive to Hibernation: hibernate

Transitioning between the different stages are done asynchronously and happens the next time step,
or a relevant step function is called on the entity manager containing the different components.
The stages in the lifecycle indicate what sort of functionality that a component should partake in,
for example only active components actually get updated.
Additionally the stages allows a programmer to release unneeded resources in the situation where a component
will be inactive for a period of time.

### Storage & Lookup
All components are stored contiguously together on a per type basis within component collections.
This allows for efficient batched and cache friendly processing.
However, it also means that components are moved around quite a lot, for example a component
will be moved around to another memory location when its lifecycle stage shifts.
Because of this it is important not to hold references or pointers to components,
as they will be invalidated.
Referencing mechanics is done through ComponentHandles, or through Entity objects.
A ComponentHandle has mechanics allowing it to discern when it has been invalidated,
and correct itself.
Additionally it has smart pointer semantics.

### Communication
Communication between components can be done in two ways, either through events and messages,
or through direct communication. Direct communication is not advised unless really necessary,
as it can limit the possibilities of multi-threading.

#### Events
Creating events is done by calling the createEntityEvent on the entity manager,
specifying the event type, and sender and receiver ids.
Adding arguments to an event is done through the createEventArgument free function,
which will append the argument at the end of the event parameter.
For the receiver, accessing the arguments is done by get'ing the argument through
its identifier. Additionally the receiver needs to specify the type of the argument
as a template parameter, indicate what the event should be casted to.
For the event type safety is exchanged for flexibility, and events don't contain
type information.
More information about events can be found in the event documentation.

### Type Identifier
Components come in many different types, such as sprites or transform components.
To identify the different component types a type identifier is added.
This is a simple numerical value, which is either created through the hashing of a string,
or simply a straight numerical value given by the programmer.
When requesting access to a component, the programmer will need to supply both the entity id,
and the type identifier to the desired component.
As stated type identifiers can be created either through hashing a string, or just using a numerical value.
However, mixing and matching these two methods for the same type is illegal.
Additionally the range 0 to 999 is reserved for internal use within NOX ECS, and using any of these values
for type identifiers other than for standard types can lead to undefined behavior.

## Entity Manager
The entity manager can be seen as the entry point for NOX ECS. All manipulation of entities goes through the entity manager.
This includes creation of entities, requesting lifecycle transitions, sending events etc.

### Step
Controlling the execution of entity behavior is done through several step functions within the entity manager.
These step functions are as follow.
- distributeLogicEvents, distributes all logic events from the rest of NOX Engine to the components.
- update, updates all active components.
- distributeEntityEvents, distributes all entity events to the different components.
- deactivateStep, deactivate all requested components.
- hibernateStep, hibernate all requested components.
- removeStep, remove all requested components.
- createStep, create all requested components.
- awakeStep, awake all requested components.
- activateStep, activate all requested components.

These functions can be called in any order, so long as they don't end up invalidating the internal invariants of the ECS.
For example a component cannot go from active to hibernation, skipping the inactive stage.
A convenience step function is also supplied, which calls all the different step functions
in the order they appear in the list above.

### Async
It might be apparent from these step functions that entity behavior and manipulation is asynchronous.
This is intentional as it allows NOX ECS to do batched updates per frame, which also opens up
the possibility of running the updates on multiple threads.

#### Multi-threading
The internal computation of the different steps, for example updating components, can be run concurrently
if the programmer can guarantee that no race conditions can occur.
This is done through the data access enum, which is described below.
Each step function call that runs concurrently follows a fork and join model, meaning that
all threads are finished running when the function exits.
Currently multi-threading in steps is supported in the update function, and the distribution of events.
However, running multi-threaded in the distribution of events is not advised, as it is not particularly performant.

##### INDEPENDENT 
During this operation, the component is completely independent, or only communicate with thread safe resources.
The component can therefore safely be run concurrently.

##### READ_ONLY
The component will read from another component, and can be run concurrently as long as it does not run
with the component it reads from. This option requires the programmer to list to type identifiers
of the component types which this component can read from.

##### READ_WRITE
Currently not supported, but the intention is to support it in the future. Until implemented READ_WRITE
will be treaded as UNKNOWN

##### UNKNOWN
This component is either unsafe to run concurrently, or nothing is known about it, leading NOX ECS
to assume it cant be run concurrently. This is the default access, to avoid introducing race conditions.

## Meta Information
The meta information structure is responsible for containing information about the different component types
within NOX ECS. It allows the library to retain needed type information, without templates etc.
It contains info about the size of the object, as well as pointers to the different functions to call
on the object. This is stored on a per component type basis, and is one of the mechanisms that allows
for dynamic dispatch without the space overhead of virtual pointers, and without using templates.

# Example
Following is a simple example showing how to create two components, who communicates to each other
with the help of events.
Additional examples will be added in the future.

```cpp
#include <cstdio>
#include <nox/ecs/Component.h>
#include <nox/ecs/createEventArgument.h>
#include <nox/ecs/createMetaInformation.h>
#include <nox/ecs/Entity.h>
#include <nox/ecs/EntityManager.h>

class MessageSender
    : public nox::ecs::Component
{
public:
    // Use the standard nox::ecs::Component constructor
    using nox::ecs::Component::Component;

    void update(const nox::Duration&)
    {
        // Sending message event to "myself"
        auto event =
            std::move(this->entityManager->createEntityEvent({"MessageEvent"},
                                                             this->id,
                                                             this->id));

        // Adding the string Test as an argument, named TestEvent,
        // before sending it.
        nox::ecs::createEventArgument(event, std::string("Test"), {"TestEvent"});
        this->entityManager->sendEntityEvent(std::move(event));
    }
};

class MessageReceiver
    : public nox::ecs::Component
{
public:
    using nox::ecs::Component::Component;

    void receiveEntityEvent(const nox::ecs::Event& event)
    {
        std::printf("Message: %s\n",
                    event.getArgument({"TestEvent"})
                         .value<std::string>()
                         .c_str());
    }
};

int
main()
{
    // Using namespace for brevity
    using namespace nox::ecs;

    // Using strings to create the Type Identifier
    // The createMetaInformation function ensures the correct creation
    // of the MetaInformation structure.
    auto senderInfo = createMetaInformation<MessageSender>({"MessageSender"});
    // The MessageSender component does not interact with anything that is
    // not thread safe, and can therefore be run concurrently with anything.
    // It can be marked independent.
    senderInfo.updateAccess = DataAccess::INDEPENDENT;

    const auto receiverInfo = createMetaInformation<MessageReceiver>({"MessageReceiver"});

    EntityManager manager;

    // All component types that are going to be used
    // must be registered with the manager.
    manager.registerComponent(senderInfo);
    manager.registerComponent(receiverInfo);

    // After all components are registered, the manager needs to be configured.
    // This function decides which components that can be run concurrently, etc.
    manager.configureComponents();

    // Creates an entity ID which is used to further manipulate the entity.
    auto sender = manager.createEntity();
    manager.assignComponent(sender, {"MessageSender"});
    manager.assignComponent(sender, {"MessageReceiver"});

    // Awakens all components belonging to the entity sender
    manager.awakeEntity(sender);
    manager.activateEntity(sender);

    // This step invocation here will not update any of the two
    // entities, as the create-activate steps are done last.
    // An alternative could be to use the create-active steps individually.
    manager.step({});

    for (int i = 0; i < 5; ++i)
    {
        manager.step({});
    }

    manager.deactivateEntity(sender);
    manager.hibernateEntity(sender);

    manager.deactivateStep();
    manager.hibernateStep();
    manager.removeStep();

    return 0;
}
```

# Installation Instructions

Click the [link](https://gitlab.com/suttungdigital/nox-engine) for setting up the project.

For Linux, you need a few packages you might not have, which is not mentioned in the original nox instructions.
Run the following bash command to install all the packages needed.
```
sudo apt-get install libXmu-dev libXi-dev libgl-dev dos2unix git wget build-essential libxmu-dev libxi-dev libgl-dev libosmesa-dev libboost-all-dev libsdl1.2-dev libsdl1.2debian libglew-dev
```

## Building
Next you want to build the project, which is specied in detail in Suttings instructions mentioned above.
However, we have created a script for building the project on linux which is compile.sh that is found in the root folder of the repisitory.
The script has a possible parameters one can send in.

Make arguments is used to allow arguments to be sent to the make command.
```
--make-arguments="<args>"
```

Cmake arguments is used to allow arguments to be sent to the cmake command.
```
--cmake-arguments="<args>"
```

None of these need to be set for the build to run sucecessfully, but it is convenient to tell the cmake command to run using multiple threads with the command --cmake-arguments="-j4", which will make it run with four threads.

## Running Individual Tests
For running each individual test, you need to have your terminal in the root directory of the repository and run each test from there. This is because the tests assumes needs certain files from various folders in the project, which assumes that you are running it from the root directory.
There are various parameters one can send the tests, and the important ones are used in the run_ecs_tests.sh and run_nox_tests.sh scripts, so look there for reference. In addition, here is a few examples on a couple of tests:

Run the ecs memory usage test with transform componenets using 256 actors/entities.
```
./build/bin/ecs_memory_usage_transform -actor_amount 256
```

Run the ecs memory usage test with transform componenets using 32 actors/entities, and using the world in the path "world/20_world.json".
```
./build/bin/ecs_numerous_unique_components -actor_amount 32 -world_path world/20_world.json"
```

## Running the Test Scripts
To run and gather data from the tests you need to run the either the run_ecs_tests.sh or run_nox_tests.sh.
These scripts either runs all the ecs tests, or all the nox tests depending on which one you run.
They are located in the path "tests/nox/scripts".
Like running each indivual test, you need to have your terminal in the correct directory for the internal paths in the scripts to work properly.
The terminal should be placed in the "tests/nox/scripts" directory when running the scripts.
All the results of the tests are then located in the results folder, which is located in the root directory of the repository.
Each test case gets its own folder, where all the different test results from each iteration of the amount of actors are located.
For example the ecs memory usage test using sprite components with no optimization toggles will be located in:

```
ecs/no_optimization_toggles/memory_usage_sprite/
```

## Collecting the Data
In order to actually use the data from the test in a consistent and quick manner, we created another script create_csv.sh that goes through all the tests automatically and sets up a csv file with the results.
This script also has the restriction of where you can run it from, and it is placed in the same folder as run all tests scripts.
You also need to place the terminal in the "tests/nox/scripts" directory to run the scripts.
There are two parameters you need to send in order for the script to work.

The grep arguments are used to figure out which test case you want the data from.
The scripts starts in the results folder, and lists up every single file recursivly in the folders.
Then the script greps with all the arguments sent through the paramenter, which if entered correctly will result in a proper csv file being made.
```
--grep-arguments="<args>"
```

For example for the ecs no optimization memory usage test with sprite components, you could write:
```
--grep-arguments="ecs no_opt memory_usage_sprite"
```

Data type is used to specify which type of data you want to collect, which can either be time(real time), cpu(cpu cycles), or memory(heap allocation).
```
--data-type="<type>"
```

The csv file will be located in the results folder, named according to what grep arguments and data type specified. For example using the grep arguments above with time as data type, it would be:
```
ecs_no_opt_memory_usage_sprite_time.csv
```

You can now either look at the results, or plot them using gnuplot or similar tools.

# Additional Information
This information should give the reader a quick understanding of how NOX ECS works, and allow him/her to start
testing out NOX ECS. 
Additional information can be found in the documentation of the library, and further questions can be directed
to the authors, either through githubs issue system, or through messages.
