#include "state_list.h"

void populate_state_list()
{
    for (const auto &transition : transitions)
    {
        transition.begin_state->add_condition(transition.condition, transition.end_state);
    }
}