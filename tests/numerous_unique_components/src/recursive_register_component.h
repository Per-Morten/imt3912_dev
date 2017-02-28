#pragma once
#include <nox/logic/world/Manager.h>

#include <components/trivial_component.h>


template<std::size_t N>
void
registerTrivialComponent(nox::logic::world::Manager* world);

template<>
void
registerTrivialComponent<0>(nox::logic::world::Manager* world);

#include "recursive_register_component.tpp"
