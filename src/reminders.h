#ifndef __REMINDERS_H__
#define __REMINDERS_H__

#include "application.h"

class Reminders
{
  public:
    Reminders();

    int getTotalReminders();

    String getReminderId(int location);
    bool getReminderEnabled(int location);
    unsigned long getReminderTime(int location);
    String getReminderMessage(int location);

    String addReminder(bool enabled, unsigned long time, String message);
    bool updateReminder(String id, bool enabled, unsigned long reminderTime, String message);
    bool removeReminder(String id);
  private:
    int mTotalReminders = 0;
    String createNewID();
    void generateID(char * id);
    int loadTotalReminders();
    int getReminderLocationFromID(String id);
    void updateTotalReminders(int value);
    void removeReminder(int location);
    void saveReminder(int location, String id, bool enabled, unsigned long time, String message);
};

#endif  // End of __REMINDERS_H__ definition check
