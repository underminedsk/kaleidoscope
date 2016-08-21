#define WAIT_FOR_PUZZLE_START 0 //Starting state should always be 0
#define PUZZLE_RUNNING 1
#define PUZZLE_DONE 2

void updateGameState() {
  switch (puzzle_state)
  {
    case WAIT_FOR_PUZZLE_START:

      lcdMessage("",String((char*)playerName));

      puzzle_state = PUZZLE_RUNNING;
      break;
    case PUZZLE_RUNNING:
      keypadSetup();
      //delay(1000);
      if (gameDone == true) { 
        puzzle_state = PUZZLE_DONE;
      }  
      break;
    case PUZZLE_DONE:
      
      puzzle_state = WAIT_FOR_PUZZLE_START;
      break;
    default:
      break; 
  }
}

