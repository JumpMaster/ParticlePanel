/*
 * Project ParticlePanel
 * Description:
 * Author:
 * Date:
 */

 #include "display.h"
 #include "dst.h"

 Display display;
 DST dst;

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
     } else if (extra.startsWith("kevtest")) {
         int a = extra.substring(8).toInt();
         Particle.publish("LOG", String::format("%d", a), PRIVATE);
         display.kevtest(a);
     }

     return 0;
 }

 void setup() {
     Particle.function("setText", setText);
     Particle.function("setOption", setOption);

     dst.setIsDST();
     display.begin();
 }
