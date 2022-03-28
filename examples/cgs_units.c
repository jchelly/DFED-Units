#include <stdlib.h>
#include <stdio.h>

#include "swift_units.h"

int main(int argc, char *argv[]) {

  /* Initialize a cgs unit system */
  struct unit_system us;
  units_init_cgs(&us);

  /* Print out the unit system */
  units_print(&us);

  return 0;
}
