#include <Servo.h>
/*********************************************************************
  This is an example based on nRF51822 based Bluefruit LE modules
********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif



/*=========================================================================
       -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/


Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/

Servo servo;


#define OPEN         0
#define CLOSE       90
#define MOTOR_PIN    2
int openTime = -1;
void setup(void)
{
  servo.attach(MOTOR_PIN);
  servo.write(CLOSE);
  delay(2000);
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();



  ble.verbose(false);  // debug info is a little annoying after this point!


  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  //Give module a new name
  ble.println("AT+GAPDEVNAME=MobilLås"); // named TLONE

  // Check response status
  ble.waitForOK();

  /* Wait for connection */
  while (! ble.isConnected()) {

  }
  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  // Sætter c til 0.
  int c = 0;
  /*
    // Check for user input
    char n, inputs[BUFSIZE + 1];

    if (Serial.available())
    {
      Serial.println("D4_S");
      n = Serial.readBytes(inputs, BUFSIZE);
      inputs[n] = 0;
      Serial.print("D4_E");
      // Send characters to Bluefruit
      Serial.print("Sending: ");
      Serial.println("D3_S");
      Serial.println(inputs);
      Serial.print("D3_E");

      // Send input data to host via Bluefruit
      Serial.println("D2_S");
      ble.print(inputs);
      Serial.println("D2_E");
    }


    if (ble.available()) {
    Serial.print("D1_S");
    Serial.print("* "); Serial.print(ble.available()); Serial.println(F(" bytes available from BTLE\n"));
    Serial.print("D1_E");

    //ble.flush();

    }

    //Serial.print(ble.available());
    // Echo received data
  */
  // Så længe der er bytes i bufferen, skal ble.read køres og gemmes I c
  while ( ble.available() )
  {
    c = ble.read();
    /*
      Serial.print("D5_S");
      Serial.println(c);
      Serial.println("char: ");
      Serial.println((char)c);
      Serial.print("D5_E");
      delay(1000);
    */
  }

  // Lås op - Lås automatisk efter xx antal sekunder
  if (c == '1')
  {
    //Dør åbner
    servo.write(OPEN);
    //Tager nuværende millis og gemmer I openTime.
    openTime = millis();
  }
  // Hvis c er '0' ELLER openTime er over 0 og millis - openTime er over 5 sekunder, så lås dør
  if ((c == '0') || ((openTime > 0) && ((millis() - openTime) > 5000 )))
  {
    //Låser dør
    servo.write(CLOSE);
    openTime = -1;
  }

}
