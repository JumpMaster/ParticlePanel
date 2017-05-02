#include "application.h"
#include "dst.h"

int DST::getLastSunday(int day, int dow, int dim) {
    if (dow != 1)
        day += (8-dow);
        
    while (day <= (dim-7))
        day += 7;
    
    return day;
}

void DST::setIsDST() {
    bool isDST = this->checkDST(Time.hour(), Time.day(), Time.weekday(), Time.month());
    
    if (isDST && !Time.isDST())
        Time.beginDST();
    else if (!isDST && Time.isDST())
        Time.endDST();
}

bool DST::checkDST(int hour, int day, int dow, int month) {
    if ((month > 3 && month < 10) || (month == 10 && day < 24))
        return true;
    
    if ((month < 3 || month > 10) || (month == 3 && day < 24))
        return false;

    int lastSunday = this->getLastSunday(day, dow, 31);

    if (month == 3) {
        if (day < lastSunday)
            return false;
        else if (day > lastSunday)
            return true;
            
        if (hour >= 1)
            return true;
        else
            return false;
    }
    
    if (month == 10) {
        if (day < lastSunday)
            return true;
        else if (day > lastSunday)
            return false;
            
        if ((hour >= 1 && !Time.isDST()) || (hour >= 2 && Time.isDST()))
            return false;
        else
            return true;
    }
    
    return false;
}