import sys

import math

MAX_STATE=5
NUM_NODES=3

#initialize initial state to all zeros
initial_node_state = [0 for i in range(0, NUM_NODES)]

#only one possible initial node state: all zeros
possible_states = [initial_node_state]
print 'hit enter to see all possible states at next level'
while 1: #loop forever
    raw_input()
    #compute all new possible states
    print len(possible_states), 'possible states'
    print possible_states
    new_states = []
    for state in possible_states: #for each current possible state of the system...
        for j in range(0,NUM_NODES): #if the user activates node 'j'...
            new_state = []
            for state_idx in range(0,NUM_NODES): #loop through nodes in current state
                #if this is the node user activated, then leave value the same, otherwise increase state by 1
                new_value_for_idx = state[state_idx]+abs(state_idx-j)
                #new_value_for_idx = state[state_idx]+1 if state_idx != j else state[state_idx]
                #if we've reached max state, drop back to zero.
                if new_value_for_idx >= MAX_STATE:
                    new_value_for_idx = 0
                new_state.append(new_value_for_idx)
            new_states.append(new_state) #add computed state to possible states
    possible_states = new_states




