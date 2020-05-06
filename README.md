# IoTR - Robot Monitoring Device System

The project is developed by group of people that has interests in the field of Robotics and IoT.
This group is dedicated to serve on the robotics community in Bulgaria.

## **License**

GNU General Public License v3.0

Permissions of this strong copyleft license are conditioned on making available complete source code of licensed works and modifications, which include larger works using a licensed work, under the same license. Copyright and license notices must be preserved. Contributors provide an express grant of patent rights.

## **Content**

 - [Brief](https://github.com/orlin369/iotr)
 - [References](https://github.com/orlin369/iotr)
 - [Environment](https://github.com/orlin369/iotr)
 - [External Libraries](https://github.com/orlin369/iotr)
 - [Schematics](https://github.com/orlin369/iotr)

## **Brief**

- The complexity of the project is requires to have keelage in the field of: Arduino IDE, Adding external libraries to arduino environment, Visual Studio, ESP8266 Arduino SDK, Git. 
- This project is related to **IoTR - Robot Monitoring Device System**. The content of the repository is tha source code of **ESP8266 (WeMos-R1/2) MCU** on the board.
- IDE that is used to create this project is **Arduino IDE**, the code is written in **Visual Studio 2019** with **vMicro** extension to work with **Arduino SDK** and **ESP8266 SDK**.
- The project is create in **Windows 10** environment.

## **References**

 - [What is Arduino](https://www.arduino.cc/en/Main/Software)
 - [What is ESP8266](https://en.wikipedia.org/wiki/ESP8266)
 - [What is Visual Studio](https://visualstudio.microsoft.com/)
 - [What is vMicro](https://www.visualmicro.com/)
 
## **Environment**

**The environment should be prepared the wat it is given in the repo. First Arduino IDE and then all the same for all given bullets.**

**1. Arduino IDE**

First we download and install the Arduino IDE.

- Link - [**here**](https://www.arduino.cc/en/Main/Software)
- Version - 1.8.12 or greater

**2. ESP8266 SDK**

After the Arduino IDE is installed, it is time to add the ESP8266 SDK. Use link below to read the details.

- Description - It can be add by Arduino board manager with specified SDK version.
- Version - 2.6.3
- Repository - [**here**](https://github.com/esp8266/Arduino)
- License - [**GPL-V2.1**](https://github.com/esp8266/Arduino/blob/master/LICENSE)

**3. Upload files from "/data" to the MCU**

This Arduino IDE extension will help to upload files to SPIFFS via Serial port to the MCU.

- Link - Tutorial how to setup file upload [**here**](https://github.com/esp8266/arduino-esp8266fs-plugin)
- Version - 0.5.0
- License - [**GPL-2.0**](https://github.com/esp8266/arduino-esp8266fs-plugin/blob/master/LICENSE.txt)

**4. Visual Studio**

If we want to use better editor we recommend to proceed on this step, download and install Visual Studio.

- Link - [**here**](https://visualstudio.microsoft.com/downloads/)
- Version - 2019

**5. vMicro**

To use Visual Studio as development environment use this extension and do not forget to stop Visual Studio before installation.

- Link - [**here**](https://www.visualmicro.com/page/User-Guide.aspx?doc=Visual-Micro-Menu.html)
- Version - [**here**](https://www.visualmicro.com/page/Visual-Micro-Product-Version-History-Fixes-and-Additions.aspx)

**6. It is also possible from "/sport_apps" to use upload.py to upload the files from "/data".**

- Open terminal
- Call the application

        $ python upload.py --path <path\to\data> --ip <ip.address.of.the.module> --user <USER_NAME> --password <PASSWORD>
- The result should be displayed immediately after upload with status code 200.

## **External Libraries**

**All external libraries are placed: __C:/Users/(Your User Name)/Documents/Arduino/libraries__**

**1. Arduino JSON Library**

- Description - CRUD operation of JSON string no the file system and network.
- Installation - It can be add by Arduino library manager with specified library version.
- Version - 6.X
- Repository - [**here**](https://github.com/bblanchon/ArduinoJson)
- License - [**MIT**](https://github.com/bblanchon/ArduinoJson/blob/master/LICENSE.md)

**2. NTPClient**

- Description - It is responsible to have actual date and time.
- Installation - It can be add by Arduino library manager with specified library version.
- Version - 2.5.0
- Repository - [**here**](https://github.com/arduino-libraries/NTPClient)
- License - **No License**

**3. ESPAsyncTCP**

- Description - Async TCP socket fro ESP8266/32
- Installation - It can not be add by Arduino library manager. It should be add manually.
- Repository - [**here**](https://github.com/me-no-dev/ESPAsyncTCP)
- License - [**LGPLv3.0**](https://github.com/me-no-dev/ESPAsyncTCP/blob/master/LICENSE)

**4. ESPAsyncWebServer**

- Description - Async WEB services.
- Installation - It can not be add by Arduino library manager. It should be add manually.
- Repository - [**here**](https://github.com/me-no-dev/ESPAsyncWebServer)
- License - **No License**

**5. AsyncTCP**

- Description - Async TCP sockets.
- Installation - It can not be add by Arduino library manager. It should be add manually.
- Repository - [**here**](https://github.com/me-no-dev/AsyncTCP)
- License - [**LGPLv3.0**](https://github.com/me-no-dev/AsyncTCP/blob/master/LICENSE)


**6. Async MQTT**

- Description - Communicate with MQTT broker.
- Installation - It can not be add by Arduino library manager. It should be add manually.
- Repository - [**here**](https://github.com/marvinroger/async-mqtt-client)
- License - [**MIT**](https://github.com/marvinroger/async-mqtt-client/blob/master/LICENSE)

**7. IRremoteESP8266**

- Description - It is responsible to communicate with IR remotes.
- Installation - It can not be add by Arduino library manager. It should be add manually.
- Repository - [**here**](https://github.com/crankyoldgit/IRremoteESP8266)
- License - [**LGPL-2.1**](https://github.com/crankyoldgit/IRremoteESP8266/blob/master/LICENSE)

**8. Adafruit_NeoPixel**

- Description - Communicates with programmable RGB LED WS2812.
- Installation - It can be add by Arduino library manager with specified library version.
- Repository - [**here**](https://github.com/adafruit/Adafruit_NeoPixel)
- License - [**LGPL-3.0**](https://github.com/adafruit/Adafruit_NeoPixel/blob/master/LICENSE)

## **Schematics**

Coaming soon **:)**
