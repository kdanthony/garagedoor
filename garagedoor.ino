/* Garage Door Opener

Opens and closes garage doors via a relay and reports back status to Smart
Things via IR distance sensors. 

Copyright (c) 2015, Kevin Anthony (kevin@anthonynet.org)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "httpsclient-particle/httpsclient-particle.h"

// Assign your relay pins here. 
int relay1  = D5;
int relay2  = D4;
int sensor1 = A5;
int led1    = D7;

int relay1_state = 0;
int relay2_state = 0;

int nextTime;

bool laststate;
bool currentstate;

// Keep 'Bearer' on the front!
char smartthings_access_code[] = "Bearer ACCESS-CODE-HERE";
char smartthings_hostname[]    = "graph.api.smartthings.com";
char smartthings_url[]         = "/api/smartapps/installations/SMARTAPP-INSTALLATION-HERE/doorstate/";

static unsigned int freemem;
bool g_https_complete;
uint32 g_bytes_received;
int g_connected;
const bool g_https_trace = false; 

TCPClient client;

unsigned char httpRequestContent[] = "PUT %s HTTP/1.0\r\n"
  "User-Agent: MatrixSSL/" MATRIXSSL_VERSION "\r\n"
  "Host: graph.api.smartthings.com\r\n"
  "Accept: */*\r\n" 
  "Authorization: Bearer ACCESS-CODE-HERE\r\n\r\n";

void setup() 
{
    Particle.function("doorcontrol", doorcontrol);
    Particle.function("doorstate", doorstate);
    Particle.function("irdistance", irdistance);
    
    // set the pins to output mode
    pinMode(relay1, OUTPUT); 
    pinMode(relay2, OUTPUT);
    pinMode(led1, OUTPUT);
    digitalWrite(relay1, HIGH); 
    
    pinMode(sensor1, INPUT);
    
    currentstate = doorstate("");
    
    laststate = currentstate;
}

void loop()
{
    unsigned int t = millis();
    // Pause for a bit on updating to debounce
    if (nextTime > t) return;
    currentstate = doorstate("");
    
    if(currentstate != laststate)
    {
        updatesmartthings(currentstate);
    }
    
    laststate = currentstate;
    nextTime = millis() + 5000;
}

int doorcontrol(String command)
{
     digitalWrite(relay1, LOW); 
     delay(300);
     digitalWrite(relay1, HIGH); 
     return 1;
}

int irdistance(String command)
{
    // Debugging only to get the raw analog value of the IR distance sensor
    int irsensor = analogRead(sensor1);
    return irsensor;
}

bool doorstate(String command)
{
    int irsensor;
    bool doorstate = false;
    irsensor = analogRead(sensor1);
    if(irsensor > 900)
    {
        doorstate = true;
    }
    else
    {
        doorstate = false;
    }
    return doorstate;
}

int updatesmartthings(bool state)
{
    char smartthings_new_url[120];
    if (state == true)
    {
        sprintf(smartthings_new_url, "%s%s", smartthings_url, "open");
    }
    else 
    {
        sprintf(smartthings_new_url, "%s%s", smartthings_url, "closed");
    }
    httpsclientSetup(smartthings_hostname, smartthings_new_url);
    g_connected = client.connect(smartthings_hostname, 443);
    if (!g_connected) {
      client.stop();
      // If TCP Client can't connect to host, exit here.
      return -1;
    }
    g_https_complete = false;
    g_bytes_received = 0;
    int32 rc;
    httpsclientSetPath(smartthings_new_url);
    if ((rc = httpsClientConnection(httpRequestContent, 0, NULL) < 0)) {
        // Solid LED for failed send
        digitalWrite(led1,HIGH);
        delay(1000);
        digitalWrite(led1,LOW);
    } else {
        // Flash rapidly on successful send
        digitalWrite(led1,HIGH);
        delay(100);
        digitalWrite(led1,LOW);
        delay(100);
        digitalWrite(led1,HIGH);
        delay(100);
        digitalWrite(led1,LOW);
        delay(100);
        digitalWrite(led1,HIGH);
        delay(100);
        digitalWrite(led1,LOW);
        delay(100);
        digitalWrite(led1,HIGH);
        delay(100);
        digitalWrite(led1,LOW);
    }
    client.stop();

}

