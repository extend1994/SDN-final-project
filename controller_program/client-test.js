const net = require('net');
const log = require('signale');
var host = process.argv[2] || '127.0.0.1';
var port = process.argv[3] || 8888;
var connection = net.Socket();
connection.connect(port,host);
connection.write(,function(){
  connection.end();
});
