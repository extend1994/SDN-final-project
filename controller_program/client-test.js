const net = require('net');
const log = require('signale');
var host = process.argv[2] || '127.0.0.1';
var port = process.argv[3] || 8888;
var connection = net.Socket();
connection.connect(port,host);
var send_data = new Buffer([0,1,0,0]);
connection.write(send_data,function(){
  connection.end();
});
