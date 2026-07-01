#include "state.h"

void State::add_condition(std::function<bool(char)> condition, State *next_state)
{
    next_states.push_back({condition, next_state});
}

State *State::get_next_state(char c)
{
    for (const auto &next_state_pair : next_states)
    {
        const auto &condition = next_state_pair.first;
        if (condition(c))
            return next_state_pair.second;
    }
    return nullptr;
}