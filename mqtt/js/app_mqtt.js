#!/usr/bin/node
// vim:ts=4:sw=4:sts=4:et:ai
//
const { Pool, Client } = require('pg');
var format = require('pg-format');

const client = new Client({
    user: 'mqtt',
    host: 'localhost',
    database: 'mqtt',
    password: 'yaituaC1bengie0r',
    port: 5432,
});
client.connect();

client.query('SELECT NOW()', (err, res) => {
  console.log(err, res);
//  client.end();
})

var clientID= "mqtt.karlsbakk.net";
var topic = "test/kitchen";
var message = "dev01,on";

var sqltemplate = "INSERT INTO %s(%s,%s,%s) VALUES ('%s', '%s', '%s')";
var sql = format(sqltemplate, 'mqtt_messages', 'clientid', 'topic', 'message', clientID, topic, message);

console.log(sql);
client.query(sql, function (error, results) {
    if (error) throw error;
    console.log("1 record inserted");
    client.end();
});

