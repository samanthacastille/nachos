#include "syscall.h"

// code by Narathip Pimpa
int main() {
  Write("\nExec adder 1\n", 20, ConsoleOutput);
  Exec("../test/adder");

  Write("\nExec adder 2\n", 20, ConsoleOutput);
  Exec("../test/adder");

  Yield();

  Write("\nExec adder 3\n", 20, ConsoleOutput);
  Exec("../test/adder");
}
// end code by Narathip Pimpa
