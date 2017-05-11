#include "reminders.h"
#include "application.h"

// EEPROM USAGE
// Messages - 1000 - 2010 (1000+(101*10))
// 100 Char + termination - 101 bytes x 10
//
// Total Reminders - 40 - 44
// int - 4 bytes x 1
//
// Time - 44 - 84
// long int - 4 bytes x 10
//
// Enabled - 85 - 95
// bool - 1 byte x 10
//
// Key 96 - 126
// 2 char + termination - 3 bytes x 10

Reminders::Reminders() {
  mTotalReminders = loadTotalReminders();
};

String Reminders::getReminderId(int location) {
  char value[3];
  EEPROM.get(96+(location*3), value);
  return String(value);
}

unsigned long Reminders::getReminderTime(int location) {
  unsigned long value;
  EEPROM.get(44+(location*4), value);
  return value;
}

bool Reminders::getReminderEnabled(int location) {
  bool value;
  EEPROM.get(85+location, value);
  return value;
}

String Reminders::getReminderMessage(int location) {
  char value[101];
  EEPROM.get(1000+(location*102), value);
  return String(value);
}

bool Reminders::updateReminder(String id, bool enabled, unsigned long reminderTime, String message) {
  int location = getReminderLocationFromID(id);

  if (location < 0 || location >= 10)
    return false;

  saveReminder(location, id, enabled, reminderTime, message);

  return true;
}

String Reminders::addReminder(bool enabled, unsigned long reminderTime, String message) {
  String id = createNewID();
  saveReminder(mTotalReminders, id, enabled, reminderTime, message);
  mTotalReminders++;
  updateTotalReminders(mTotalReminders);
  return id;
}

void Reminders::saveReminder(int location, String id, bool enabled, unsigned long reminderTime, String message) {
  EEPROM.put(85+location, enabled);
  EEPROM.put(44+(location*4), reminderTime);
  char chrId[3];
  id.toCharArray(chrId, 3);
  EEPROM.put(96+(location*3), chrId);
  char chrMessage[101];
  message.toCharArray(chrMessage, 101);
  EEPROM.put(1000+(location*102), chrMessage);
}

void Reminders::removeReminder(int location) {
  for (int i = location; i < 10; i++) {
    bool enabled = getReminderEnabled(i+1);
    long time = getReminderTime(i+1);
    String message = getReminderMessage(i+1);
    String id = getReminderId(i+1);
    saveReminder(i, id, enabled, time, message);
    if (time == 0)
      break;
  }
  mTotalReminders--;
  updateTotalReminders(mTotalReminders);
}

int Reminders::loadTotalReminders() {
  int value = 0;
  EEPROM.get(40, value);
  if (value < 0)
    value = 0;
  else if (value > 10)
    value = 10;
  return value;
}

int Reminders::getTotalReminders() {
  return mTotalReminders;
}

void Reminders::updateTotalReminders(int value) {
  mTotalReminders = value;
  EEPROM.put(40, mTotalReminders);
}

String Reminders::createNewID() {
  char id[3];
  while(true) {
    generateID(id);
    bool unique = true;
    for (int i = 0; i < mTotalReminders; i++) {
      String strId = getReminderId(i);
      if (id[0] == strId.charAt(0) && id[1] == strId.charAt(1))
        unique = false;
    }
    if (unique)
      break;
  }
  return String(id);
}

void Reminders::generateID(char * id) {
  id[0] = random(65,116);
  if (id[0] > 90)
    id[0] += 7;
  id[1] = random(65,116);
  if (id[1] > 90)
    id[1] += 7;
  id[3] = '\0';
}

bool Reminders::removeReminder(String id) {
  int location = getReminderLocationFromID(id);
  if (location >= 0 && location < 10) {
    removeReminder(location);
    return true;
  } else
    return false;
}

int Reminders::getReminderLocationFromID(String id) {
  for (int i = 0; i < mTotalReminders; i++) {
    if (id.equals(getReminderId(i)))
      return i;
  }
  return -1;
}
