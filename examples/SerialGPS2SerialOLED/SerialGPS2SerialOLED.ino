#include <TinyGPS++.h>
#include <heltec_unofficial.h>

TinyGPSPlus gps; // Create a TinyGPS++ object
/*----------------------------------------------------------------------------
| GPS Function         | Description                                          |
|----------------------|------------------------------------------------------|
| location.lat()       | Latitude in degrees (double)                         |
| location.lng()       | Longitude in degrees (double)                        |
|                                                                             |
| date.year()          | Year (2000+) (u16)                                   |
| date.month()         | Month (1-12) (u8)                                    |
| date.day()           | Day (1-31) (u8)                                      |
|                                                                             |
| time.hour()          | Hour (0-23) (u8)                                     |
| time.minute()        | Minute (0-59) (u8)                                   |
| time.second()        | Second (0-59) (u8)                                   |
| time.centisecond()   | 100ths of a second (0-99) (u8)                       |
|                                                                             |
| speed.knots()        | Speed in knots (double)                              |
| speed.mph()          | Speed in miles per hour (double)                     |
| speed.mps()          | Speed in meters per second (double)                  |
| speed.kmph()         | Speed in kilometers per hour (double)                |
|                                                                             |
| course.deg()         | Course in degrees (double)                           |
|                                                                             |
| altitude.value()     | Raw altitude in centimeters (i32)                    |
| altitude.meters()    | Altitude in meters (double)                          |
| altitude.miles()     | Altitude in miles (double)                           |
| altitude.kilometers()| Altitude in kilometers (double)                      |
| altitude.feet()      | Altitude in feet (double)                            |
|                                                                             |
| satellites.value()   | Number of satellites in use (u32)                    |
|                                                                             |
| hdop.value()         | Horizontal Dilution of Precision (HDOP) (100ths-i32) |
|=============================================================================|
| (Not shown in example)                                                      |
|-----------------------------------------------------------------------------|
| location.rawLat().negative ? "-" : "+" | Latitude sign                      |
| location.rawLat().deg()                | Raw latitude in whole degrees (u16)|
| location.rawLat().billionths()         | Latitude billionths (u32)          |
| location.rawLng().negative ? "-" : "+" | Longitude sign                     |
| location.rawLng().deg()                | Raw long. in whole degrees (u16)   |
| location.rawLng().billionths()         | Longitude billionths (u32)         |
|                                                                             |
| date.value()         | Raw date in DDMMYY format (u32)                      |
| time.value()         | Raw time in HHMMSSCC format (u32)                    |
| speed.value()        | Raw speed in 100ths of a knot (i32)                  |
| course.value()       | Raw course in 100ths of a degree (i32)               |
-----------------------------------------------------------------------------*/



void setup(){
  heltec_setup();
  Serial.begin(115200);       // Begin Serial monitor communication
  Serial1.begin(9600, SERIAL_8N1, 33, 34);  // Begin Serial1 for GPS communication
  display.init(); // Initialising the UI will init the display too.
}

void displayGPSInfo() {
  // Check if valid location is available
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Location: INVALID");
  }

  // Check date and time information
  if (gps.date.isValid() && gps.time.isValid()) {
    Serial.print("Date: ");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());

    Serial.print("Time: ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.print(":");
    Serial.println(gps.time.centisecond());
  } else {
    Serial.println("Date/Time: INVALID");
  }

  // Speed in km/h
  if (gps.speed.isValid()) {
    Serial.print("Speed (km/h): ");
    Serial.println(gps.speed.kmph());
    Serial.print("Speed (knots): ");
    Serial.println(gps.speed.knots()); // Speed in knots (double)
    Serial.print("Speed (mph): ");
    Serial.println(gps.speed.mph()); // Speed in miles per hour (double)
    Serial.print("Speed (mps): ");
    Serial.println(gps.speed.mps()); // Speed in miles per hour (double)

  } else {
    Serial.println("Speed: INVALID");
  }
// Speed in km/h
  if (gps.course.isValid()) {
  Serial.print("Course (°): ");
  Serial.println(gps.course.deg()); // Course in degrees (double)
  } else {
    Serial.println("Course: INVALID");
  }
  // Altitude
  if (gps.altitude.isValid()) {
    Serial.print("Altitude (m): ");
    Serial.println(gps.altitude.meters());
    Serial.print("Altitude (miles): ");
    Serial.println(gps.altitude.miles()); // Altitude in miles (double)
    Serial.print("Altitude (km): ");
    Serial.println(gps.altitude.kilometers()); // Altitude in kilometers (double)
    Serial.print("Altitude (feet): ");
    Serial.println(gps.altitude.feet()); // Altitude in feet (double)
  } else {
    Serial.println("Altitude: INVALID");
  }

  // Satellites count
  if (gps.satellites.isValid()) {
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
  } else {
    Serial.println("Satellites: INVALID");
  }

  // Horizontal dilution of precision (HDOP)
  if (gps.hdop.isValid()) {
    Serial.print("HDOP: ");
    Serial.println(gps.hdop.value());
  } else {
    Serial.println("HDOP: INVALID");
  }

  Serial.println();
}

void ShowPosOLED()
{
  String speed, course, time_str, pos;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  if(gps.location.isValid())
				{
        display.clear();
        char buffer[16]; // Puffer für die formatierte Zeit, genügend Platz für HH:MM:SS:CC
        sprintf(buffer, "%02d:%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second(), gps.time.centisecond());

        time_str = String(buffer); // Umwandlung des C-Strings in einen Arduino String

				//String time_str = (String)gps.time.hour() + ":" + (String)gps.time.minute() + ":" + (String)gps.time.second()+ ":"+(String)gps.time.centisecond();
				display.drawString(0, 0, time_str);
				pos = "LAT: " + (String)gps.location.lat()+"   LON: "+  (String)gps.location.lng();;
				display.drawString(0, 15, pos);
        if (gps.speed.isValid())
        {
				   speed  ="Speed: " +  (String)gps.speed.kmph();
        }
        else 
        {
          speed  ="Speed: INVALID";
        }
				display.drawString(0, 30, speed);

        if (gps.course.isValid())
        {
            course  ="Course: " +  (String)gps.course.deg();
        }
        else 
        {
            course  ="Course: INVALID";
        }
				display.drawString(0, 45, course);
        }
  else
        {
        display.clear();
        display.drawString(0, 0, (String)"No valid Position");
        }// write the buffer to the display
  display.display();
}
void loop(){
  while (Serial1.available() > 0){
    char gpsData = Serial1.read();
    // Feed the GPS data to the TinyGPS++ object
    gps.encode(gpsData);
  }

  // Display the parsed GPS info once per second
  if (millis() % 1000 == 0) {
    displayGPSInfo(); //Show it on Serial Monitor 
    ShowPosOLED();    //Show it on OLED Display
  }
}

