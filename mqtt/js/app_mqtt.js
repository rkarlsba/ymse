// vim:ts=4:sw=4:sts=4:et:ai

var mysql = require('mysql');

//Create Connection
var connection = mysql.createConnection({
    host: "localhost",
    user: "mqtt",
    password: "yaituaC1bengie0r",
    database: "mqtt"
});


connection.connect(function(err) {
    if (err) throw err;
    console.log("Database Connected!");
});


//insert a row into the tbl_messages table
connection.connect(function(err) {
    var clientID= "client001";
    var topic = "test/kitchen";
    var message = "dev01,on";
    var sql = "INSERT INTO ?? (??,??,??) VALUES (?,?,?)";
    var params = ['mqtt_messages', 'clientid', 'topic', 'message', clientID, topic, message];
    sql = mysql.format(sql, params);
    connection.query(sql, function (error, results) {
        if (error) throw error;
        console.log("1 record inserted");
    });
});
