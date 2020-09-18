#include <Arduino.h>
#include <Wire.h>
#include <FT6X36.h>

FT6X36 ts(&Wire, 15);


void printTouchInfo(TPoint p, TEvent e)
{
  if (e != TEvent::Tap && e != TEvent::DragStart && e != TEvent::DragMove && e != TEvent::DragEnd)
    return;

  Serial.print("X: ");
  Serial.print(p.x);
  Serial.print(", Y: ");
  Serial.print(p.y);
  Serial.print(", E: ");

  switch (e)
  {
  case TEvent::Tap:
    Serial.println("Tap");
    break;
  case TEvent::DragStart:
    Serial.println("DragStart");
    break;
  case TEvent::DragMove:
    Serial.println("DragMove");
    break;
  case TEvent::DragEnd:
    Serial.println("DragEnd");
    break;
  default:
    Serial.println("UNKNOWN");
    break;
  }
}

void touch(TPoint p, TEvent e)
{
  printTouchInfo(p, e);
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));

  Wire.begin();
  
  ts.begin();
  ts.registerTouchHandler(touch);
}

void loop()
{
  ts.loop();
}
