// vim:ts=4:sw=4:sts=4:et:ai

var pgsql = require('pgsql');

// Create Connection
// (yeah, I know I shouldn't post passwords on github, but this is for internal testing)
var connection = pgsql.createConnection({
    host: "localhost",
    user: "mqtt",
    password: "yaituaC1bengie0r",
    database: "mqtt"
});

connection.connect(function(err) {
    if (err) throw err;
    console.log("Database Connected!");
});

//insert a row into the mqtt_messages table
connection.connect(function(err) {
    var clientID= "mqtt.karlsbakk.net";
    var topic = "test/kitchen";
    var message = "dev01,on";
    var sql = "INSERT INTO ?? (??,??,??) VALUES (?,?,?)";
    var params = ['mqtt_messages', 'clientid', 'topic', 'message', clientID, topic, message];
    sql = pgsql.format(sql, params);
    connection.query(sql, function (error, results) {
        if (error) throw error;
        console.log("1 record inserted");
    });
});
