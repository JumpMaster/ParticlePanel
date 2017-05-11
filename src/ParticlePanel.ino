/*
* Project ParticlePanel
* Description:
* Author:
* Date:
*/

#include "display.h"
#include "dst.h"
#include "publishqueue.h"
#include "reminders.h"

Display display;
DST dst;
PublishQueue pq;
Reminders reminders;
unsigned long ReminderTimes[10];

ApplicationWatchdog wd(60000, System.reset);

void random_seed_from_cloud(unsigned int seed) {
  randomSeed(seed);
}

void eventHandler(const char *event, String data)
{
  if (data.startsWith("upd")) { // update
    updateReminder(data.substring(4));
    return;
  } else if (data.startsWith("add")) { // new reminder
    addReminder(data.substring(4));
    return;
  }

  pq.publish("ppc_error", String::format("unknown command:%s", data.c_str()));
}

int setText(String extra) {
  bool repeat = extra.charAt(0) == '1';
  String text = extra.substring(1)
    .toUpperCase()
    .replace("Ä", "&");
  display.setText(text, repeat);
  display.setMode(show_text);
  return 0;
}

int setOption(String extra) {
  extra = extra.toLowerCase();
  if (extra.startsWith("setcolor")) {
    int color = extra.substring(9).toInt();
    display.setColor(color);
  } else if (extra.startsWith("stop")) {
    //repeat = false;
  } else if (extra.startsWith("setbrightness")) {
    int brightness = extra.substring(14).toInt();
    display.setBrightness(brightness);
  } else if (extra.startsWith("setscrollspeed")) {
    int updateInterval = extra.substring(15).toInt();
    display.setUpdateTimer(updateInterval);
  } else if (extra.equals("rainbow")) {
    display.setMode(show_rainbow);
  }

  return 0;
}

int getReminders(String extra) {
  for (int i = 0; i < reminders.getTotalReminders(); i++) {
    String id = reminders.getReminderId(i);
    unsigned long time = reminders.getReminderTime(i);
    String message = reminders.getReminderMessage(i);
    bool enabled = reminders.getReminderEnabled(i);
    pq.publish(String::format("ppc_get:%d", i), String::format("%s:%d:%d:%s", id.c_str(), enabled, time, message.c_str()));
  }
  return 0;
}

// 1:123456:This is a message
int addReminder(String data) {
  bool enabled = data.charAt(0) == '1';
  int timeEndPosition = data.indexOf(":", 2);
  unsigned long time = data.substring(2, timeEndPosition).toInt();
  String message = data.substring(timeEndPosition+1);

  if (reminders.getTotalReminders() < 10) {
    String id = reminders.addReminder(enabled, time, message);
    pq.publish(String::format("ppc_add:%s", id.c_str()), "success");
  } else {
    pq.publish("ppc_add", "failed");
  }
}

// AB:1:123456:This is a message
void updateReminder(String data) {
  String id = data.substring(0, 2);
  bool enabled = data.charAt(3) == '1';
  int timeEndPosition = data.indexOf(":", 5);
  unsigned long time = data.substring(5, timeEndPosition).toInt();
  String message = data.substring(timeEndPosition+1);
  bool success = reminders.updateReminder(id, enabled, time, message);
  if (success)
    pq.publish("ppc_upd", "success");
  else
    pq.publish("ppc_upd", "failed");
}

int removeReminder(String id) {
  bool success = reminders.removeReminder(id);

  if (success) {
    pq.publish("ppc_rem", "success");
    return 0;
  } else {
    pq.publish("ppc_rem", "failed");
    return -1;
  }

  return -1;
}

void setup() {
  Particle.function("setText", setText);
  Particle.function("setOption", setOption);
  Particle.function("getReminders", getReminders);
  Particle.function("remReminder", removeReminder);
  Particle.variable("numReminders", reminders.getTotalReminders());
  Particle.subscribe("pph_", eventHandler, MY_DEVICES);

  dst.setIsDST();
  display.begin();
}
