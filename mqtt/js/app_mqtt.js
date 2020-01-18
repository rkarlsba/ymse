// vim:ts=4:sw=4:sts=4:et:ai
//
var PgClient = require('pg');
var format = require('pg-format');
var mqtt = require('mqtt');

var Topic = '#'; //subscribe to all topics
var Broker_URL = 'mqtt://mqtt.karlsbakk.net';

var options = {
    clientId: 'MyMQTT',
    port: 1883,
    keepalive : 60
};

var pgclient = new PgClient({
    user: 'mqtt',
    host: 'localhost',
    database: 'mqtt',
    password: 'yaituaC1bengie0r',
    port: 5432,
});
pgclient.connect();

var client = mqtt.connect(Broker_URL, options);
client.on('connect', mqtt_connect);
client.on('reconnect', mqtt_reconnect);
client.on('error', mqtt_error);
client.on('message', mqtt_messsageReceived);
client.on('close', mqtt_close);

function mqtt_connect() {
    console.log("Connecting MQTT");
    client.subscribe(Topic, mqtt_subscribe);
}

function mqtt_subscribe(err, granted) {
    console.log("Subscribed to " + Topic);
    if (err) {
        console.log(err);
    }
}

function mqtt_reconnect(err) {
    console.log("Reconnect MQTT");
    if (err) {
        console.log(err);
    }
    client = mqtt.connect(Broker_URL, options);
}

function mqtt_error(err) {
    console.log("Error!");
    if (err) {
        console.log(err);
    }
}

function after_publish() {
//do nothing
}

function mqtt_messsageReceived(topic, message, packet) {
    console.log('Topic=' + topic + ' Message=' + message);
}

function mqtt_close() {
    console.log("Close MQTT");
}

var clientID= "mqtt.karlsbakk.net";
var topic = "test/kitchen";
var message = "dev01,on";

var sqltemplate = "INSERT INTO %s(%s,%s,%s) VALUES ('%s', '%s', '%s')";
var sql = format(sqltemplate, 'mqtt_messages', 'clientid', 'topic', 'message', clientID, topic, message);

console.log(sql);
pgclient.query(sql, function (error, results) {
    if (error) throw error;
    console.log("1 record inserted");
    pgclient.end();
});

