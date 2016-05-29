import time

#set up global constants for the puzzle.  these can be changed to alter the difficulty.
ALLOWED_TIME_FOR_PUZZLE = 30 #player has this many seconds to complete the puzzle
NUM_NODES = 3 #the game has 3 nodes.  until we fix the update_node() function to be dynamic, this is hardcoded to 3
COLORS = ['red', 'orange', 'yellow'] #these are the states the nodes can be in.


#current puzzle state.  updated as the player tries to solve the puzzle
active_user_id = None #the id of the user who last tagged in.  if set to null, the puzzle is not active.
NODES = None #when the puzzle is initialized, this represents the state of the puzzle.  the outputs (i.e. lights) should
#be updated to match the state of this array.
start_time = None #the system time the puzzle started.  used to determine if the time limit has been reached.



def set_idle_state():
    global active_user_id, NODES
    active_user_id = None
    NODES = None


def set_initial_puzzle_state_for_user(user_id):
    global active_user_id, NODES, start_time #declare global so this function modifies these global variables.
    NODES = [COLORS[0] for i in range(0,NUM_NODES)] #initialize nodes to first color in the sequence
    start_time = time.time()
    moves_made = 0
    active_user_id = user_id


def update_node(node_id, num_colors_to_change_by):
    #changes the state of a single node given by index 'node_id' by the specified number of color states, wrapping
    #around to the beginning of the color sequence if necessary
    global NODES
    cur_color = NODES[node_id]
    cur_idx_of_color = COLORS.index(cur_color)
    new_idx_of_color = (cur_idx_of_color + num_colors_to_change_by) % len(COLORS)
    new_color = COLORS[new_idx_of_color]
    NODES[node_id] = new_color


def update_nodes(id_of_node_activated):
    #changes the state of all the nodes in the puzzle, given that a single node has been activated. custom logic for
    #how nodes change can be implemented here, independent of the rest of the game.
    #TODO: hardcoded to 3 nodes for now.  Algorithm is: whatever node is activated, the other nodes change by N colors,
    #      where N is the distance
    if id_of_node_activated == 0:
        update_node(1, 1)
        update_node(2, 2)
    elif id_of_node_activated == 1:
        update_node(0, 1)
        update_node(2, 1)
    elif id_of_node_activated == 2:
        update_node(0, 2)
        update_node(1, 1)


def is_puzzle_solved():
    #returns True if the given puzzle is in the final state, otherwise returns false.
    for node in NODES:
        if node != COLORS[-1]:
            return False
    return True

def elapsed_time():
    return time.time() - start_time


# ------- output functions --------
#note in microcontroller, these would change the color of the lights or maybe make them flash a pattern for
#puzzle solved, timeout, etc.

def publish_puzzle_started():
    print 'puzzle is starting.  you have %s seconds to make all the nodes %s' % (ALLOWED_TIME_FOR_PUZZLE, COLORS[-1])

def publish_puzzle_solved():
    print 'you solved the puzzle in %s moves!' % ALLOWED_TIME_FOR_PUZZLE

def publish_puzzle_timeout():
    print 'failed to solve the puzzle in %s seconds ' % ALLOWED_TIME_FOR_PUZZLE

def publish_puzzle_state():
    # in a real microcontroller, this would ensure the colors reflect the internal state of the NODES array. for the
    # purposes of this demo, it will just print the current state of the game to the screen.
    print NODES


# ------- user input functions --------

def is_rfid_card_present_at_reader():
    #checks to see whether an RFID card is present at the reader.
    return False #TODO this would use the rfid library to make this check

def read_user_id_from_rfid_card():
    #reads the user id from the card
    return 1234 #TODO this would use the rfid library to read the user id from the card

# ------- button input functions -------

def is_user_pushing_button(button_id):
    return False #TODO this would check the input pin to see if the user is pushing teh button



def setup():
    set_idle_state()


def loop():
    if active_user_id is None:
        #no user is currently checked in to the puzzle. see if a user is trying to check in right now
        if is_rfid_card_present_at_reader():
            #ok, the user is scanning their card right now.  set the active user_id and return
            new_user_id = read_user_id_from_rfid_card()
            set_initial_puzzle_state_for_user(new_user_id)
            publish_puzzle_started()
            publish_puzzle_state()
    else:
        #ok we have an active user_id, run puzzle logic...

        #check for timeout condition
        if elapsed_time() > ALLOWED_TIME_FOR_PUZZLE:
            publish_puzzle_timeout()
            set_idle_state()

        #check for puzzle solved condition:
        if is_puzzle_solved():
            publish_puzzle_solved()
            return 1

        #check each node to see if user is pushing this button
        for button_id in range(0, NUM_NODES):
            if is_user_pushing_button(button_id):
                update_nodes(button_id)
                publish_puzzle_state()
    time.sleep(0.1) #sleep for 10 milliseconds
    return 0 #puzzle not solved


if __name__ == '__main__':
    setup()

    puzzle_solved = 0
    while not loop():
        pass