// Following code is using ESP8266 (also known as NodeMCU) and DTH22 sensor. Code has been tested and it's working, wohoo! :)

// This code has been commented by Aarni Pavlidi, if you have any questions or suggestions with the code,
// then please send me email at me@aarnipavlidi.fi!

#include <DHT.h> // Importing "DHT.h" library, so we are able to use it on this code!
#include <ESP8266WiFi.h> // Importing "ESP8266WiFi.h" library, so we are able to use it on this code!

// Formatting variables and also the api key can be found from => Channels => My Channels => Select your channel => API Keys and copy the "Write API Key" here.
String apiKey = "your_api_key_here"; // In the future development (for demo this is okay!), I don't recommend having api key "visible". One way is to hide, for example putting variable to own .ino file and import string from there to here.
const char *wlanNetwork =  "your_network_name_here"; // In the future development (for demo this is okay!), I don't recommend having wlan network name "visible". One way is to hide, for example putting variable to own .ino file and import string from there to here.
const char *wlanPasword =  "your_network_password_here"; // In the future development (for demo this is okay!), I don't recommend having wlan password "visible". One way is to hide, for example putting variable to own .ino file and import string from there to here.
const char *server = "api.thingspeak.com"; // Formatting variable, so we are able to connect to the ThingSpeak. Don't change this! :)

#define DHTPIN 4 // Formatting "DHTPIN" variable and sensor is connected to the pin 4, which is same thing as GPI04, which means its same thing as D2 (which you can see physically on the board!).
#define DHTTYPE DHT22 // Formatting "DHTTYPE" variable and we are using DHT22 sensor on this project. If you are using for example "DHT11" sensor, then change this variable value.

// Formatting "dht" variable, which uses previously made variables inside dht(). Not sure what does "11" do on this scenario (found this while doing researching with the code) and previously tried using only => "DHT dht(DHTPIN, DHTTYPE)" and that didn't worked for me at all! :(
DHT dht(DHTPIN, DHTTYPE, 11); // Don't change these variables! :)

WiFiClient client; // Initialize the client library, which is related to the <ESP8266WiFi.h> library. This enables us to connect to the wlan!

void setup() { // Formatting "setup()" function, this is called when a sketch starts. This will only run once or after each powerup or reset on the ESP8266 (NodeMCU) board.

  Serial.begin(115200); // Sets the data rate "115200" (bits per second = baud) for serial data transmission. Make sure your boards setting "Upload Speed" is equal and once you go to the "Serial Monitor" it has same value there, otherwise you wouldn't be able to see what program is printing for you!
  delay(1500); // Pauses the program for 1,5 seconds, before moving on the next line => dht.begin(). We are pausing program, because once we power the board, we don't want to take values straight from the sensor. I did some googling and found out that sometimes it can take up "250 milliseconds" to get value out of sensor.
  dht.begin(); // Make a connection between board and with the DHT sensor.

  Serial.println("Connecting to the following network:"); // Printing following string for the user in the serial monitor.
  Serial.println(wlanNetwork); // Printing following variable (value) for the user in the serial monitor.

  WiFi.begin(wlanNetwork, wlanPasword); // Board ESP8266 (NodeMCU) will start trying connect to the network with the variables you put on the code.

  while (WiFi.status() != WL_CONNECTED) { // We are using while() function, so as long board is trying connect to the network it will do everything inside => {} brackets. Also "!=" means "Not equal to".
    Serial.print("Trying connect to the network..."); // Printing following string for the user in the serial monitor.
    delay(1500); // Pauses the program for 1,5 seconds, before moving on the next line => Serial.print().
    Serial.print("."); // Printing following string for the user in the serial monitor.
  }

  Serial.println(""); // Printing following string for the user in the serial monitor.
  Serial.println("Connected to network successfully!"); // Printing following string for the user in the serial monitor.

} // End of "setup()" function.

void loop() { // Formatting "loop()" function. ESP8266 (NodeMCU) will run this multiple times untill user takes power off from the board.0

  float humidity = dht.readHumidity(); // Formatting "humidity" variable and we are using "float" function, because when sensor gets values by using "readHumidity()" function it has decimal points.
  float temperature = dht.readTemperature(); // Formatting "temperature" variable and we are using "float" function, because when sensor gets values by using "readTemperature()" function it has decimal points.

  // Following line means that if either variables (|| is equal to "or") "humidity" or "temperature" returns value "NaN" (Not-A-Number), then it will print following text for the user in the serial monitor and keep up doing untill both values return different value other than "NaN"-
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!"); // Printing following string for the user in the serial monitor.
    delay(1500); // Pauses the program for 1,5 seconds, before moving on the next line => return.
    return; // Terminates the function => if (...) {...}.
  }

  // if ESP8266 (NodeMCU) connection to the server is successful it will get values from the sensor and post them .json file (ThingSpeak).
  if (client.connect(server,80)) {
    String postJSON = apiKey; // Formatting variable "postStr" to be equal as "apiKey" variable.
    // Operator "+=" means => "Add AND", so for example => postStr +="&field1="; is equal to => postStr = postStr + "&field1"; and remember that "postStr" is equal to "apiKey" variable!
    postJSON +="&field1="; // Post value on the following JSON object. Keep in mind that this can't be changed I tried multiple times, because I would rather have following => Temperature = "value".
    postJSON += String(temperature); // Fetch current value from the "temperature" variable by using "readTemperature()" function.
    postJSON +="&field2="; // Post value on the following JSON object. Keep in mind that this can't be changed I tried multiple times, because I would rather have following => Humidity = "value".
    postJSON += String(humidity); // Fetch current value from the "humidity" variable by using "readHumidity()" function.

    // Printing following strings to the server (ThingSpeak). These are related to the ThingSpeak documentation, so that we are able post sensor values successfully on the server.
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postJSON.length()); // Remember that "postJSON" is equal to "apiKey" variable.
    client.print("\n\n"); // "\n\n" is equal to 2x new lines, so for example it would be same thing as <br><br> in HTML!
    client.print(postJSON); // Remember that "postJSON" is equal to "apiKey" variable.

    Serial.print("Temperature is currently "); // Printing following string for the user in the serial monitor.
    Serial.print(temperature); // Printing following variable (value) for the user in the serial monitor.
    Serial.print(" °C and Humidity is currently "); // Printing following string for the user in the serial monitor.
    Serial.print(humidity); // Printing following variable (value) for the user in the serial monitor.
    Serial.println(" %. Sending these values to the cloud."); // Printing following string for the user in the serial monitor.
  }

  client.stop(); // Disconnect from the server.
  Serial.println("Waiting for 15 seconds to get next value from the sensor and post it to the cloud..."); // Printing following string for the user in the serial monitor.

  // Pauses the program for 15 seconds, before ending the "loop()" function and starting the loop again. You might be wondering why in earth we are delaying the program for so long? Is because the ThingSpeak free version only provides "Message update interval limit" for every 15 seconds. Which means we can't update .json file faster than that! :(
  delay(15000);

} // End of "loop()" function.
