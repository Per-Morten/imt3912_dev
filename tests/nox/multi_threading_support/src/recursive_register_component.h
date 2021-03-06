#pragma once
#include <nox/logic/world/Manager.h>

#include <components/trivial_component.h>


// Functions for registering N amount of trivial components in the world manager.
// It does so via meta programming, and recursively calls itself while reducing N by 1.


// Initial function to call, is the one that recursively calls itself and counts down.
template<std::size_t N>
void
registerTrivialComponent(nox::logic::world::Manager* world);

// Terminate function. Template specialization when N is equal to 0.
// Exactly the same as the other function, except that it does not call itself.
template<>
void
registerTrivialComponent<0>(nox::logic::world::Manager* world);

#include "recursive_register_component.tpp"
