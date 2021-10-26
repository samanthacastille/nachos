#include "syscall.h"


int main() {
  Write("\nSort 1\n", 10, ConsoleOutput);
  Exec("../test/sort");
  Write("\nSort 2\n", 10, ConsoleOutput);
  Exec("../test/sort");
  Write("\nSort 3:\n", 10, ConsoleOutput);
  Exec("../test/sort");
}
