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

// Functions for Project 1 -> Tasks 1 and 2
void project1task1(int);
char* task1GetInput();
int validateInputSize(char*, int);
char* inputIdentification(char*, int);
void project1task2();
int task2GetInput();
void forkThreads();
int randomNumber(int, int);
void shouter(int);


// global flag to execute a particular task
int taskFlag;

// Constant shouting strings
const char* shout1 = "I should not have said that.";
const char* shout2 = "My father will hear about this.";
const char* shout3 = "But I AM the chosen one.";
const char* shout4 = "Follow the spiders? Why couldn't it be 'follow the butterflies'?";
const char* shout5 = "You've got the emotional range of a teaspoon.";
const char* shout6 = "I suspect Nargles are behind it.";



//------------------------------------------------------------------------------
// ThreadTest (main function)
// Depending on the value of the taskFlag, thread is taken in
// in the terminal command when running nachOS, it will either execute
// project1task1, project1task2, or print an error if the task is not specified properly.
//------------------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    if (taskFlag==1){
      Thread *t = new Thread("forked thread");
      t->Fork(project1task1, 0);
    } else if (taskFlag==2) {
      project1task2();
    } else {
      printf("You didn't use the -A command to choose a thread task to execute.\nPlease insert '-A' followed by which task you'd like to execute (1/2).\nExiting ->->->->->->\n\n");
    }

	  currentThread->Finish();
}


//----------------------------------------------------------------------
// THIS IS THE START OF CODE FOR TASK 1
// IT INCLUDES FUNCTIONS <project1task1>, <task1GetInput>,
// <validateInputSize>, and <inputIdentification>.
//----------------------------------------------------------------------


const int maxInputSizeTask1 = 256;

// This function invokes task 1 with a thread created from the ThreadTest function.
void project1task1(int thread) {
  printf("Please give me an input.\n");

  char* result = task1GetInput();
  if (result=="Your input was too long. It was discarded.\nPlease rerun and keep your input under: ") {
    printf("%s %d.\n\n", result, (maxInputSizeTask1 - 1));
  }
  else {
    printf("\nYour input was of type: %s\n\n", result);
  }
}

// This function recieves input from the user.
// It calls validateInputSize to validate the input size.
// If the input size is fine, it calls inputIdentification
// thread returns the input type.
char* task1GetInput() {
	char* input;
  char* tooLongInputMessage = "Your input was too long. It was discarded.\nPlease rerun and keep your input under: ";
  char* inputType;
  input = new char[maxInputSizeTask1];

  fgets(input, maxInputSizeTask1, stdin);

  int validateResult = validateInputSize(input, maxInputSizeTask1);
  if (validateResult==1) {
    inputType = inputIdentification(input, maxInputSizeTask1);
  }
  else if (validateResult==-1){
    return tooLongInputMessage;
  }
  return inputType;
}

// This function validates the input size.
// If the input is empty, it asks for it again.
// If the input is too long, it discards it and tells the user to start over.
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
      return 1;;
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
// IT INCLUDES FUNCTIONS <project1task2>, <task2GetInput>,
// <forkThreads>, <randomNumber>, and <shouter>.
//----------------------------------------------------------------------


int globalShoutCount;
int globalThreadCount;
const int maxInputSizeTask2 = 6;


// Get input from user and calls task2GetInput.
// Create threads and call the shouter function.
void project1task2(){
  int threadCount;
  int shoutCount;
  char* threadInputType;
  char* shoutInputType;
  char* threadInput;
  char* shoutInput;

  printf("Enter number of threads (1-1000): ");
  threadCount = task2GetInput();
  if (threadCount==-1) {
    printf("\nYour input was either not a positive integer, or it was too large. Try again.\n\n");
    currentThread->Finish();
  } else {
    globalThreadCount = threadCount;
  }

  printf("\nEnter number of shouts (1-1000): ");
  shoutCount = task2GetInput();
  if (shoutCount==-1) {
    printf("\nYour input was either not a positive integer, or it was too large. Try again.\n\n");
    currentThread->Finish();
  } else {
    globalShoutCount = shoutCount;
  }

  forkThreads();
}

// Gets user input and check if it's an integer and in the correct range.
// Returns either: the integer value correctly input from the user OR -1
// if the value entered is not valid.
int task2GetInput() {
  char* input = new char[maxInputSizeTask2];
  char* inputType;
  int integerInput;

  fgets(input, maxInputSizeTask2, stdin);

  int validateResult = validateInputSize(input, maxInputSizeTask2);
  if (validateResult==1) {
    inputType = inputIdentification(input, maxInputSizeTask2);
    if (inputType=="integer") {
      integerInput = atoi(input);
      if ((integerInput>0) && (integerInput<1001)) {
        return integerInput;
      } else {
        return -1;
      }
    } else {
      return -1;
    }
  }
  else {
    return -1;
  }
}

// Forks specified number of threads to do the shouting
void forkThreads() {
  for(int i=0;i<globalThreadCount;i++){
    Thread *t = new Thread("Shouting Thread");
    t->Fork(shouter, i+1);
  }
}

// Generates a random number in a specific range
int randomNumber(int upperLimit, int lowerLimit) {
  int random = (Random()%((upperLimit)-lowerLimit))+lowerLimit;
}

// Will decide thread phrase to shout at random and keep track of # of shouts.
void shouter(int thread){
  int waitsDone = 0;
  int shoutsDone = 0;
  int upperBusyLoops = 7;
  int lowerBusyLoops = 3;
  int waitGoal = randomNumber(upperBusyLoops, lowerBusyLoops);
  int upperShoutStrings = 7;
  int lowerShoutStrings = 1;
  int randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);

  while(shoutsDone<globalShoutCount) {
    while((waitsDone>=0) && (waitsDone<waitGoal)){
      waitsDone++;
      currentThread->Yield();
    }
    switch (randomShout) {
      case(1): {
        printf("Shouter %d: %s\n", thread, shout1);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(2): {
        printf("Shouter %d: %s\n", thread, shout2);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(3): {
        printf("Shouter %d: %s\n", thread, shout3);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(4): {
        printf("Shouter %d: %s\n", thread, shout4);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(5): {
        printf("Shouter %d: %s\n", thread, shout5);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(6): {
        printf("Shouter %d: %s\n", thread, shout6);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
    }
    shoutsDone++;
    waitsDone = 0;
    waitGoal = randomNumber(upperBusyLoops, lowerBusyLoops);
  }
}
