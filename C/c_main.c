

#include "utils.h"
#include "capture.h"
#include "devices.h"
#include "vendors.h"

void init(char* iface)
{
  printf("Starting capture in %s \n",iface);
  utils_init();
  vendors_init("./vendors.txt");											// Load vendors
  device_init();
  capture_init(iface);
}


int x(int y)
{
   return y + 1;
}
