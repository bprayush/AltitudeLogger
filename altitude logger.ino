#include <SFE_BMP180.h>
#include <Wire.h>
#include <EEPROM.h>



SFE_BMP180 pressure;
double getPressure();

double baseline; // baseline pressure
int raw;
int MAX;
int addr=0;
int address=0;

void setup()
{
  MAX = 2;
  Serial.begin(9600);
  Serial.println("REBOOT");
  pinMode(12, INPUT);

  // Initialize the sensor

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }

  // Get the baseline pressure:
  
  baseline = getPressure();
  
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  
}

void loop()
{
  double a,P;
  
  // Get a new pressure reading:

  P = getPressure();
  a = pressure.altitude(P,baseline);
 

  //get raw data from the sensor, convert to feets
  raw = a*3.28084,0;
  //Serial.println(raw);

  //write data to ROM if raw is greater than MAX
  if( raw > MAX )
  {
    EEPROM.write(addr++, raw);
    MAX = raw;
    EEPROM.write(addr++, 'e');
    Serial.print("jhg: ");
    Serial.println(MAX);
  }

  //read data from ROM
  if(digitalRead(12))
    Serial.println(EEPROM.read(address++), DEC);

  //Clear rom
  if( digitalRead(11) )
  {
    for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    addr = 0;
    address = 0;  
  }
 
  delay(500);
}
}


double getPressure()
{
  char status;
  double T,P,p0,a;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
