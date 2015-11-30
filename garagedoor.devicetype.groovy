/**

Particle Photon Garage Door
   
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

preferences {
  input("deviceId", "text", title: "Device ID")
  input("token", "text", title: "Access Token")
}
// for the UI
metadata {

  definition (name: "Particle Photon Garage Door", author: "Kevin Anthony") {
    capability "Garage Door Control"
    capability "Door Control"
    capability "Contact Sensor"
    capability "Refresh"
    capability "Polling"
    capability "Momentary"

    attribute "open", "string"
    attribute "closed", "string"
    
    //command "doorStatus", ["number"]
    command "setDoorState"

  }

  tiles(scale: 2) {
    multiAttributeTile(name:"status", type: "generic", width: 6, height: 4){
      tileAttribute ("device.status", key: "PRIMARY_CONTROL") {
        attributeState "open", label:'${name}', icon:"st.doors.garage.garage-open", backgroundColor:"#ffa81e"
        attributeState "closed", label:'${name}', icon:"st.doors.garage.garage-closed", backgroundColor:"#79b821"
        attributeState "garage-open", label:'Open', icon:"st.doors.garage.garage-open", backgroundColor:"#ffa81e"
        attributeState "garage-closed", label:'Closed', icon:"st.doors.garage.garage-closed", backgroundColor:"#79b821"
      }
    }
    standardTile("contact", "device.contact", width: 2, height: 2) {
      state("open", label:'${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
      state("closed", label:'${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
    }
    standardTile("door", "device.door", width: 2, height: 2, canChangeIcon: false) {
      state "closed", label:'closed', action:"door control.open", icon:"st.doors.garage.garage-closed", backgroundColor:"#79b821", nextState:"opening"
      state "open", label:'open', action:"door control.close", icon:"st.doors.garage.garage-open", backgroundColor:"#ffa81e", nextState:"closing"
      state "opening", label:'moving', icon:"st.doors.garage.garage-opening", backgroundColor:"#ffe71e"
      state "closing", label:'moving', icon:"st.doors.garage.garage-closing", backgroundColor:"#ffe71e"
    }
    standardTile("refresh", "device.switch", width: 2, height: 2, inactiveLabel: false, decoration: "flat") {
      state "default", label:"", action:"refresh.refresh", icon:"st.secondary.refresh"
    }
    main(["status", "door"])
    details(["status", "door", "refresh"])
  }
}

def parse(String description) {
  return null
}

def open() { 
  doorMove('doorcontrol');
}
def close() {
  doorMove('doorcontrol');
}

def push() {
  doorMove('doorcontrol');
}

def poll() { //TODO
  refresh()
}

def refresh() { //TODO
  doorState('doorstate');
}

private doorMove(command) {
  def doorState = device.currentState("door")?.value
  httpPost(
    uri: "https://api.spark.io/v1/devices/${deviceId}/${command}",
    body: [access_token: token, command: level],
  ) {response -> log.debug (response.data)}
}

private setDoorState(state) {
  sendEvent(name: 'contact', value: state )
  sendEvent(name: 'door', value: state )
  sendEvent(name: 'status', value: state )
}

private doorState(command) {
  def doorState = device.currentState("door")?.value
  httpPost(
    uri: "https://api.spark.io/v1/devices/${deviceId}/${command}",
    body: [access_token: token, command: level],
  ) {
    response ->
      log.debug(response.data.return_value)
      if (response.data.return_value == 1) {
        sendEvent(name: 'contact', value: 'open')
        sendEvent(name: 'door', value: 'open')
        sendEvent(name: 'status', value: 'open')
        log.debug("Door is open");
      }
      else {
        sendEvent(name: 'contact', value: 'closed')
        sendEvent(name: 'door', value: 'closed')
        sendEvent(name: 'status', value: 'closed')
        log.debug("Door is closed");
      }
      log.debug (response.data)}
}




