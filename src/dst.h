#ifndef DST_h
#define DST_h

#include "application.h"

class DST
{
  public:
    void setIsDST();
  private:
    bool checkDST(int hour, int day, int dow, int month);
    int getLastSunday(int day, int dow, int dim);
};
#endif