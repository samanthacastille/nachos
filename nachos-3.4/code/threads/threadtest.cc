// threadtest.cc
//
// Copyright (c) 1992-1993,2021 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.


// Samantha Castille - C00134220
// All of the code in this file is my own work.


#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <iostream>
using namespace std;

// global flag to execute a particular task
int taskFlag;

// Functions used in multiple tasks
char* getInput(bool, int, int, int, char*, char*);
void invalidInput(char*);
int randomInteger(int, int);
void busyWaitingLoop(int);

// Functions for Project 1 -> Tasks 1 and 2
// Task 1
void identifyInput(int);
int validateInputSize(char*, int);
char* inputIdentification(char*, int);
// Task 2
void shoutingTask();
void forkShoutingThreads();
void shouter(int);

// Functions for Project 2 -> Tasks 1-4
// Task 1
void diningPhilosophersBusyWaiting();
void philosopherBWL(int);
void forkPhilosophersBWL(int);
// Task 2
void diningPhilosophersSemaphores();
void philosopherSem(int);
void forkPhilosophersSem(int);
// Task 3
void postOfficeSimulator();
void Producers(int);
void Consumer(int);
// Task 4
void readersWritersProblem();
void Readers(int);
void Writers(int);



//------------------------------------------------------------------------------
// ThreadTest (main function)
// Depending on the value of the taskFlag, which is taken in
// in the terminal command when running nachOS, it will execute one of
// tasks 1-6 or print an error if the task is not specified properly.
//------------------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    if (taskFlag==1){
      Thread *t = new Thread("forked thread");
      t->Fork(identifyInput, 0);
    } else if (taskFlag==2) {
      shoutingTask();
    } else if (taskFlag==3) {
      diningPhilosophersBusyWaiting();
    } else if (taskFlag==4) {
      diningPhilosophersSemaphores();
    } else if (taskFlag==5) {
      postOfficeSimulator();
    } else if (taskFlag==6) {
      readersWritersProblem();
    } else {
      printf("You didn't use the -A command, or you didn't use it correctly.\n");
      printf("Please insert '-A' followed by which task you'd like to execute (1/2/3/4/5/6).\n");
      printf("Exiting ->->->->->->->->->->->->->->->->->->->->->->->->->->->\n\n\n\n");
    }

	  currentThread->Finish();
}

// -----------------------------------------------------------------------------
// Functions used by multiple tasks
// -----------------------------------------------------------------------------


// Gets user input and check if it's an integer and in the correct range.
// Returns either: the integer value correctly input from the user OR -1
// if the value entered is not valid.
char* getInput(bool intRequired, int maxInputSize, int maxIntegerSize, int minIntegerSize, char* inputRequest, char* invalidInputMessage) {
  char* input = new char[maxInputSize];
  char* inputType;
  int integerInput;

  printf("%s\n", inputRequest);

  fgets(input, maxInputSize, stdin);

  int validateResult = validateInputSize(input, maxInputSize);
  if (validateResult==1) {
    inputType = inputIdentification(input, maxInputSize);
    if (inputType=="integer") {
      if (intRequired==true) {
        integerInput = atoi(input);
        if ((integerInput>0) && (integerInput<maxIntegerSize) && (integerInput>=minIntegerSize)) {
          return input;
        } else {
          invalidInput(invalidInputMessage);
        }
      } else {
        return inputType;
      }
    } else {
      if (intRequired==true) {
        invalidInput(invalidInputMessage);
      } else {
        return inputType;
      }
    }
  } else {
    invalidInput(invalidInputMessage);
  }
}

// Prints error if input was invalid, and then ends the current thread to exit the program
void invalidInput(char* invalidInputMessage) {
  printf("%s\n", invalidInputMessage);
  currentThread->Finish();
}

// Generates a random number in a specific range
int randomInteger(int upperLimit, int lowerLimit) {
  int random = (Random()%((upperLimit)-lowerLimit))+lowerLimit;
}

void busyWaitingLoop(int loops) {
  for (int i=0; i<loops; i++) {
    currentThread->Yield();
  }
}

//----------------------------------------------------------------------
// THIS IS THE START OF CODE FOR TASK 1
// IT INCLUDES FUNCTIONS <identifyInput>,
// <validateInputSize>, and <inputIdentification>.
//----------------------------------------------------------------------


const int maxInputSizeTask1 = 256;

// This function invokes task 1 with a thread created from the ThreadTest function.
void identifyInput(int thread) {

  char* inputString;
  char* result;
  char* inputRequestMessage = "Please give me an input.\n";
  char* task1InvalidInputMessage = "Your input was too long. It was discarded.\nPlease rerun and keep your input under 256 characters.";

  result = getInput(false, maxInputSizeTask1, 0, 0, inputRequestMessage, task1InvalidInputMessage);

  printf("\nYour input was of type: %s.\n\n", result);
}


// This function validates the input size.
// If the input is empty, it asks for it again.
// If the input is too long, it returns -1, otherwise it returns 1
int validateInputSize(char* input, int size) {
  bool empty = false;
  bool tooLong;
  if (input[0]=='\n'){
    empty = true;
  }
  while (empty) {
    printf("You didn't input anything. Try again!\n\n");
    printf("Please give me an input.\n");
    fgets(input, size, stdin);
    if (input[0]=='\n'){
      empty = true;
    } else {
      empty = false;
      return 1;
    }
  }
  if ((input[size-1]==0) && ((input[size-2]=='\n') || (input[size-2]==0))) {
    return 1;
  } else {
    return -1;
  }
}

// This function receives a character array and identifies the input type.
// It returns a character string stating that type.
char* inputIdentification(char* input, int size) {
  int numDecimals = 0;

  // check for single characters, it could be an integer!
  if (input[1]=='\n'){
    if ((input[0]>='0') && (input[0]<='9')){
      return "integer";
    }
    return "character";
  }
  // begin looping through character array
  for(int i=0;i<=size;i++){
    // if null terminator, return
    if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || input[i]==0 || numDecimals>1) {
      return "character string";
    }
    // keep track of # of decimals and check placement
    if (input[i]=='.') {
      if ((i>0) && (input[i+1]!='\n') && (input[i-1]!='-')){
        numDecimals++;
      } else return "character string";
    }
    // special characters mean this is a character string
    else if (!((input[i]>='0') && (input[i]<='9')) && !((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) && !(input[i]=='-')) {
      return "character string";
    }
    // check for negative numbers
    else if ((input[i]=='-') && i>0) {
      return "character string";
    }
    else if (input[0]=='-') {
      if ((input[i]>='0') && (input[i]<='9')) {
        if (input[i+1]=='\n') {
          if ((numDecimals==0)){
            return "negative integer";
          } else if (numDecimals==1) {
            return "negative decimal";
          }
          return "negative integer";
        }
      }
    }
    // this is a number
    else if ((input[i]>='0') && (input[i]<='9')) {
      if (input[i+1]=='\n') {
        if (numDecimals==0){
          return "integer";
        } else if (numDecimals==1){
          return "decimal";
        }
      }
      else continue;
    }
    // if it includes any letter, this is a character string
    else if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) {
      return "character string";
    }
    else return "character string";
  }
}




//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 2
// IT INCLUDES FUNCTIONS <shoutingTask>,
// <forkShoutingThreads>, <randomInteger>, and <shouter>.
//----------------------------------------------------------------------


int globalShoutCount;
int globalThreadCount;
const int maxIntegerSizeTask2 = 1001;
const int minIntegerSizeTask2 = 1;
const int maxInputSizeTask2 = 6;
const char* shout1 = "I should not have said that.";
const char* shout2 = "My father will hear about this.";
const char* shout3 = "But I AM the chosen one.";
const char* shout4 = "Follow the spiders? Why couldn't it be 'follow the butterflies'?";
const char* shout5 = "You've got the emotional range of a teaspoon.";
const char* shout6 = "I suspect Nargles are behind it.";


// Get input from user and calls getInput.
// Create threads and call the shouter function.
void shoutingTask(){
  char* threadCount;
  char* shoutCount;
  char* threadInputType;
  char* shoutInputType;
  char* threadInput;
  char* shoutInput;
  char* invalidInputMessage = "\nYour input was either not a positive integer, or it was too large. Try again.\n\n";
  char* threadRequstMessage = "Enter number of threads (1-1000): ";
  char* shoutRequestMessage = "\nEnter number of shouts (1-1000): ";

  threadCount = getInput(true, maxInputSizeTask2, maxIntegerSizeTask2, minIntegerSizeTask2, threadRequstMessage, invalidInputMessage);
  globalThreadCount = atoi(threadCount);

  shoutCount = getInput(true, maxInputSizeTask2, maxIntegerSizeTask2, minIntegerSizeTask2, shoutRequestMessage, invalidInputMessage);
  globalShoutCount = atoi(shoutCount);

  forkShoutingThreads();
}


// Forks specified number of threads to do the shouting
void forkShoutingThreads() {
  for(int i=0;i<globalThreadCount;i++){
    Thread *t = new Thread("Shouting Thread");
    t->Fork(shouter, i+1);
  }
}


// Will decide thread phrase to shout at random and keep track of # of shouts.
void shouter(int thread){
  int waitsDone = 0;
  int shoutsDone = 0;
  int upperBusyLoops = 7;
  int lowerBusyLoops = 3;
  int waitGoal = randomInteger(upperBusyLoops, lowerBusyLoops);
  int upperShoutStrings = 7;
  int lowerShoutStrings = 1;
  int randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);

  while(shoutsDone<globalShoutCount) {
    while((waitsDone>=0) && (waitsDone<waitGoal)){
      waitsDone++;
      currentThread->Yield();
    }
    switch (randomShout) {
      case(1): {
        printf("Shouter %d: %s\n", thread, shout1);
        randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(2): {
        printf("Shouter %d: %s\n", thread, shout2);
        randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(3): {
        printf("Shouter %d: %s\n", thread, shout3);
        randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(4): {
        printf("Shouter %d: %s\n", thread, shout4);
        randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(5): {
        printf("Shouter %d: %s\n", thread, shout5);
        randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(6): {
        printf("Shouter %d: %s\n", thread, shout6);
        randomShout = randomInteger(upperShoutStrings, lowerShoutStrings);
        break;
      }
    }
    shoutsDone++;
    waitsDone = 0;
    waitGoal = randomInteger(upperBusyLoops, lowerBusyLoops);
  }
}



//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 3
// IT INCLUDES FUNCTIONS <diningPhilosophersBusyWaiting>,
// <forkPhilosophersBWL>, and <philosopherBWL>.
//----------------------------------------------------------------------


int philosophers, meals;
int mealsEaten = 0;
int maxInputSizeProject2 = 16;
int maxIntegerSizeProject2 = 10001;
int minPhilosophers = 2;
int minMeals = 1;
int randomInt;
int sitting;
int waitingToLeave;
bool* chopstick;
char* philospherRequestMessage = "\nPlease input the number of philosophers you would like.\n";
char* mealsRequestMessage = "\nPlease input the number of meals you would like.\n";
char* invalidPhilosopherInputMessage = "\nYou didn't input properly. You need more than 1 philosopher, and at most 10,000 philosophers.\n\n";
char* invalidInputMessage = "\nYou didn't input properly. Please input an integer greater than zero and at most 10,000.\n\n";

// Main function for busy waiting Dining Philosophers
void diningPhilosophersBusyWaiting() {
  printf("Dining Philosophers problem using busy waiting loops.\n");
  sitting = 0;
  waitingToLeave=0;
  char* inputString;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, minPhilosophers, philospherRequestMessage, invalidPhilosopherInputMessage);
  philosophers = atoi(inputString);
  chopstick = new bool[philosophers];
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, minMeals, mealsRequestMessage, invalidInputMessage);
  meals = atoi(inputString);

  forkPhilosophersBWL(philosophers);
  currentThread->Yield();
  if (waitingToLeave==(philosophers)) {
    printf("\n\n***All philosophers are done eating, and they all leave the table together.***\n\n\n");
  }
  currentThread->Finish();
}

// Have them all sit and then for threads
void forkPhilosophersBWL(int numPhilosophers) {
  for (int i=0; i<numPhilosophers; i++) {
    chopstick[i] = true;
    sitting++;
    printf("\nPhilosopher %d has entered the room.\n-Philosopher %d is waiting to sit.", i, i);
    if (sitting==numPhilosophers) {
      printf("\n\n***All philosophers have sat down.***\n\n");
      break;
    }
  }
  for (int i=0; i<numPhilosophers; i++) {
    Thread *t = new Thread("Philosopher Thread");
    t->Fork(philosopherBWL, i);
  }
}

// Each philosopher goes through this process until all meals are eaten and
// all philosophers are ready to leave
void philosopherBWL(int thread) {
  if (mealsEaten==meals) {
    printf("All meals have been eaten!\n");
    printf("--------Philosopher %d is waiting to leave.\n", thread);
    waitingToLeave++;
    if (waitingToLeave==(philosophers)) {
      printf("\n\n***All philosophers are done eating, and they all leave the table together.***\n\n\n");
    }
    currentThread->Finish();
  }
  while (mealsEaten<meals) {
    currentThread->Yield();
    printf("--Philosopher %d is trying to pick up chopstick %d.\n", thread, thread);
    while (!chopstick[thread]) {
      currentThread->Yield();
    }
    chopstick[thread] = false;
    printf("---Philosopher %d picked up chopstick [%d].\n", thread, thread);
    printf("--Philosopher %d is trying to pick up chopstick [%d].\n", thread, ((thread+1)%philosophers));
    while (!chopstick[((thread+1)%philosophers)]) {
      currentThread->Yield();
    }
    chopstick[((thread+1)%philosophers)] = false;
    printf("---Philosopher %d picked up chopstick [%d].\n", thread, ((thread+1)%philosophers));
    if (mealsEaten<meals) {
      mealsEaten++;
      printf("----Philosopher %d begins to eat (%d meals have been eaten so far).\n", thread, mealsEaten);
      randomInt = randomInteger(3,7);
      busyWaitingLoop(randomInt);
    }
    chopstick[thread] = true;
    printf("------Philosopher %d drops chopstick [%d].\n", thread, thread);
    chopstick[((thread+1)%philosophers)] = true;
    printf("------Philosopher %d drops chopstick [%d].\n", thread, ((thread+1)%philosophers));
    printf("-------Philosopher %d is thinking.\n", thread);
    randomInt = randomInteger(3,7);
    busyWaitingLoop(randomInt);
    if (mealsEaten==meals) {
      printf("All meals have been eaten!\n");
      printf("--------Philosopher %d is waiting to leave.\n", thread);
      waitingToLeave++;
      if (waitingToLeave==(philosophers)) {
        printf("\n\n***All philosophers are done eating, and they all leave the table together.***\n\n\n");
      }
      currentThread->Finish();
    }
  }
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 4
// IT INCLUDES FUNCTIONS <diningPhilosophersSemaphores>
//----------------------------------------------------------------------

Semaphore **chopsticks;

// Main function for Dining Philosophers using Semaphores
void diningPhilosophersSemaphores() {
  printf("Dining Philosophers problem using Semaphores.\n");
  sitting = 0;
  waitingToLeave=0;
  char* inputString;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, minPhilosophers, philospherRequestMessage, invalidPhilosopherInputMessage);
  philosophers = atoi(inputString);
  chopsticks = new Semaphore*[philosophers];
  for (int i=0; i<philosophers; i++) {
    chopsticks[i] = new Semaphore("Chopstick", 1);
  }
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, minMeals, mealsRequestMessage, invalidInputMessage);
  meals = atoi(inputString);

  forkPhilosophersSem(philosophers);
  currentThread->Yield();
  if (waitingToLeave==(philosophers)) {
    printf("\n\n***All philosophers are done eating, and they all leave the table together.***\n\n\n");
  }
  currentThread->Finish();
}

// All philosophers enter and then sit down, then fork threads
void forkPhilosophersSem(int numPhilosophers) {
  for (int i=0; i<numPhilosophers; i++) {
    sitting++;
    printf("\nPhilosopher %d has entered the room.\n-Philosopher %d is waiting to sit.", i, i);
    if (sitting==numPhilosophers) {
      printf("\n\n***All philosophers have sat down.***\n\n");
      break;
    }
  }
  for (int i=0; i<numPhilosophers; i++) {
    Thread *t = new Thread("Philosopher Thread");
    t->Fork(philosopherSem, i);
  }
}

// Each philosopher goes through this process until all meals are eaten and
// all philosophers are ready to leave
void philosopherSem(int thread) {
  if (mealsEaten==meals) {
    printf("All meals have been eaten!\n");
    printf("--------Philosopher %d is waiting to leave.\n", thread);
    waitingToLeave++;
    if (waitingToLeave==(philosophers)) {
      printf("\n\n***All philosophers are done eating, and they all leave the table together.***\n\n\n");
    }
    currentThread->Finish();
  }
  while (mealsEaten<meals) {
    printf("--Philosopher %d is trying to pick up chopstick %d.\n", thread, thread);
    chopsticks[thread]->P();
    printf("---Philosopher %d picked up chopstick [%d].\n", thread, thread);
    printf("--Philosopher %d is trying to pick up chopstick [%d].\n", thread, ((thread+1)%philosophers));
    chopsticks[(thread + 1)%philosophers]->P();
    printf("---Philosopher %d picked up chopstick [%d].\n", thread, ((thread+1)%philosophers));
    if (mealsEaten<meals) {
      mealsEaten++;
      printf("----Philosopher %d begins to eat (%d meals have been eaten so far).\n", thread, mealsEaten);
      randomInt = randomInteger(3,7);
      busyWaitingLoop(randomInt);
    }
    chopsticks[thread]->V();
    printf("------Philosopher %d drops chopstick [%d].\n", thread, thread);
    chopsticks[(thread + 1)%philosophers]->V();
    printf("------Philosopher %d drops chopstick [%d].\n", thread, ((thread+1)%philosophers));
    printf("-------Philosopher %d is thinking.\n", thread);
    randomInt = randomInteger(3,7);
    busyWaitingLoop(randomInt);
    if (mealsEaten==meals) {
      printf("All meals have been eaten!\n");
      printf("--------Philosopher %d is waiting to leave.\n", thread);
      waitingToLeave++;
      if (waitingToLeave==(philosophers)) {
        printf("\n\n***All philosophers are done eating, and they all leave the table together.***\n\n\n");
        printf("\n***%d meals have been eaten.***\n\n\n", mealsEaten);
      }
      currentThread->Finish();
    }
  }
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 5
// IT INCLUDES FUNCTIONS <postOfficeSimulator>
//----------------------------------------------------------------------

#define N 5

Semaphore *mutex = new Semaphore ("mutex to guard the buffer", 1);
Semaphore *fullSpaces = new Semaphore ("semaphore to ensure there is at least one item in the buffer", 0);
Semaphore *emptySpaces = new Semaphore ("semaphore to ensure there is at least free space in the buffer", N);

void postOfficeSimulator() {
  printf("Post Office Simulator.\n");

  char* inputString;
  char* peopleRequestMessage = "\nPlease input the number of people: \n";
  char* storageRequestMessage = "\nPlease input the amount of storage each person has: \n";
  char* numMessagesRequestMessage = "\nPlease input the number of total messages: \n";

  int people, storage, numMessages;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, 1, peopleRequestMessage, invalidInputMessage);
  people = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, 1, storageRequestMessage, invalidInputMessage);
  storage = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, 1, numMessagesRequestMessage, invalidInputMessage);
  numMessages = atoi(inputString);


}

void Producers (int i) {
  printf ("Producer %d is trying to enter the buffer area\n", i);
  emptySpaces -> P();
  printf ("Producer %d found an empty space\n", i);
  currentThread->Yield();
  mutex-> P();
  printf ("Producer %d deposited an item into the buffer\n", i);
  mutex-> V();
  fullSpaces -> V();
  printf ("Producer %d did a V operation on fullSpaces \n", i);
}

void Consumers (int i) {
  printf ("Consumer %d is trying to enter the buffer area\n", i);
  fullSpaces -> P();
  printf ("Consumer %d found a full space space\n", i);
  // currentThread->Yield();
  mutex-> P();
  printf ("Consumer %d consumed an item from the buffer\n", i);
  mutex-> V();
  emptySpaces -> V();
  printf ("Producer %d did a V operation on emptySpaces \n", i);
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 6
// IT INCLUDES FUNCTIONS <readersWritersProblem>
//----------------------------------------------------------------------

Semaphore *area = new Semaphore ("rw shared semaphore", 1);
Semaphore *mutexRW;
int readCount = 0;

void readersWritersProblem() {
  printf("Readers writers problem.\n");

  char* inputString;
  char* readersRequestMessage = "\nPlease input the number of readers: \n";
  char* writersRequestMessage = "\nPlease input the number of writers: \n";
  char* maxReadersRequestMessage = "\nPlease input the maximum number of readers at a time: \n";

  int readers, writers, maxReaders;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, 1, readersRequestMessage, invalidInputMessage);
  readers = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, 1, writersRequestMessage, invalidInputMessage);
  writers = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, 1, maxReadersRequestMessage, invalidInputMessage);
  maxReaders = atoi(inputString);
  mutexRW = new Semaphore ("mutexRW for readers", maxReaders);
  for (int i=0; i<readers; i++) {
    Thread *t = new Thread("Reader");
    t->Fork(Readers, i);
  }
  for (int i=0; i<writers; i++) {
    Thread *t = new Thread("Writer");
    t->Fork(Writers, i);
  }
}

void Readers(int i) {
  printf ("Reader %d is trying to enter the buffer area\n", i);
  mutexRW-> P();
  readCount++;
  if (readCount == 1) area-> P();
  mutexRW-> V();
  printf ("Reader %d is READING in the buffer area\n", i);
  currentThread->Yield();
  printf ("Reader %d is done reading and leaving the buffer area\n", i);
  mutexRW-> P();
  readCount--;
  if (readCount == 0) area-> V();
  mutexRW-> V();
}

void Writers(int i) {
  printf ("Writer %d is trying to enter the buffer area\n", i);
  area-> P();
  printf ("Writer %d is writing in the buffer area\n", i);
  printf ("Writer %d is done writing and leaving area\n", i);
  area-> V();
}
