import sys

import math
 
NUM_NODES=3

#initialize initial state to all zeros
COLORS = ['RED', 'ORANGE', 'YELLOW']
initial_node_state = [COLORS[0] for i in range(0, NUM_NODES)]


#only one possible initial node state: all zeros
possible_states = [initial_node_state]

print len(possible_states), 'possible states'
print possible_states
raw_input()


print 'hit enter to see all possible states at next level'
while 1: #loop forever
    #compute all new possible states
    new_states = []
    for state in possible_states: #for each current possible state of the system...
        for j in range(0,NUM_NODES): #if the user activates node 'j'...
            new_state = []
            for state_idx in range(0,NUM_NODES): #loop through nodes in current state
                #if this is the node user activated, then leave value the same, otherwise increase state by the distance from the node
                cur_color = COLORS[state_idx] 
                cur_idx_of_color = COLORS.index(cur_color)
                new_index_of_color_before_mod = cur_idx_of_color+abs(state_idx-j)
                new_idx_of_color = new_index_of_color_before_mod % len(COLORS) if new_index_of_color_before_mod >= len(COLORS) else new_index_of_color_before_mod
                new_value_for_idx = COLORS[new_idx_of_color]
                print 'node_activated=%s, state_idx=%s, cur_color=%s, cur_idx_of_color=%s, new_idx_of_color_before_mod=%s, new_idx_of_color=%s, new_value_for_idx=%s' % (j, state_idx, cur_color, cur_idx_of_color, new_index_of_color_before_mod, new_idx_of_color, new_value_for_idx)
                #new_value_for_idx = state[state_idx]+1 if state_idx != j else state[state_idx]
                #if we've reached max state, drop back to zero. 
                new_state.append(new_value_for_idx)
            new_states.append(new_state) #add computed state to possible states
    possible_states = new_states

    print len(possible_states), 'possible states'
    print possible_states

    raw_input()




