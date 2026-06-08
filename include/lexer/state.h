#ifndef __STATE_H__
#define __STATE_H__

#include <map>
#include <vector>
#include <utility>
#include <functional>

class State {
public: 
    void add_condition(std::function<bool(char)> condition, State* next_state);

    State* get_next_state(char c);

private:
    std::vector<std::pair<std::function<bool(char)>, State*>> next_states;
};

#endif 