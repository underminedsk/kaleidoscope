MAX_ALLOWED_MOVES = 9999
NUM_NODES = 3

#STATES = ['RED', 'ORANGE', 'YELLOW']
STATES = ['RED', 'ORANGE', 'YELLOW', 'GREEN', 'BLUE']


def initial_game_state():
    return [STATES[0] for i in range(0,NUM_NODES)]


def get_user_input():
    choice = raw_input('choose a node (1-%s)):' % (NUM_NODES))
    try:
        choice = int(choice)
        if choice <=0 or choice > NUM_NODES:
            raise ValueError()
        else:
            return choice
    except:
        print 'ERROR: enter a number between 1 and %s' % NUM_NODES
        return None


def next_node_state(state):
    idx = STATES.index(state)+1
    return STATES[idx] if idx < len(STATES) else STATES[0]


def next_game_state(current_game_state, user_choice):
    next_state = []
    for node_idx in range(0, NUM_NODES):
        cur_node_state = current_game_state[node_idx]
        if node_idx+1 != user_choice:
            next_state.append(next_node_state(cur_node_state))
        else:
            next_state.append(cur_node_state)
    return next_state


def puzzle_solved(current_game_state):
    for node_state in current_game_state:
        if node_state != STATES[-1]:
            return False
    return True


if __name__ == '__main__':
    print 'rules: (1) all nodes start as %s' % STATES[0]
    print '       (2) win make all the nodes %s' % STATES[-1]
    print '       (3) choosing a node changes the color of the other nodes. '
    print '       (4) node colors are %s' % ' -> '.join(STATES)
    moves = 0
    current_game_state = initial_game_state()
    while moves < MAX_ALLOWED_MOVES:
        print current_game_state
        moves += 1
        user_choice = get_user_input()
        if user_choice:
            current_game_state = next_game_state(current_game_state, user_choice)
            if puzzle_solved(current_game_state):
                print 'Success! You solved the puzzle in %s moves' % moves
                break




