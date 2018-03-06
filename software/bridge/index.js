const SerialPort = require('serialport');
const mqttClient = require('mqtt');
const exec = require('child_process').exec;

// read configuration from env
const conf = {
    mqttBroker: "localhost",
    mqttPort: 1883,
    serialBaud: 57600
}

console.log("Trying to connect to serial port /dev/ttyS0 with baud: ", conf.serialBaud);
  

// configure serial port
const Readline = SerialPort.parsers.Readline;
const parser = new Readline();
const port = new SerialPort('/dev/cu.usbmodem3432041', { 
    baudRate: conf.serialBaud,
    dataBits : 8,
    parity : 'none',
    stopBits: 1,
    flowControl : true
});
port.pipe(parser);

console.log("Trying to connect to MQTT broker ", conf.mqttBroker, ":", conf.mqttPort);

// configure mqtt connection
const mqtt = mqttClient.connect(`mqtt://${conf.mqttBroker}:${conf.mqttPort}`, {
    clientId: conf.clientId,
    keepalive: 1,
    reconnectPeriod: 200
});

/*
 *      MQTT EVENTS
 * 
 */ 

mqtt.on('connect', () => {
    console.log('Connected/Reconnected to MQTT Broker');
    mqtt.subscribe('#'); // # = all messages
});

// mqtt receive message
mqtt.on('message', (topic, message) => {
    
    switch (topic) {
    
        case '#/query_info':
            return handleNodeInfo(message)

        case conf.clientId+'/set_id':
            return false

    }

    console.log('No handler for topic %s', topic)
})

function handleNodeInfo (message) {
    mqtt.publish('node_info', JSON.stringify(conf))
}


/*
 *      SERIAL EVENTS
 * 
 */ 

parser.on('data', function(buffer){

    // buffer is expected to be two numbers devided by ,
    let data = buffer.replace(/[\n\r]+/g, '').split(',')

    dataJson = JSON.stringify({poti: data[0]})

    mqtt.publish('/sensors', dataJson);
    
    console.log(dataJson);
});

