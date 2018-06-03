const net = require('net');
const {Signale} = require('signale');
const options = {
  types: {
    info: {
      badge: '✉ ',
      color: 'blueBright',
      label: 'message'
    }
  }
}
const log = new Signale(options);

var host = process.argv[2] || '127.0.0.1';
var port = process.argv[3] || 8888;
var server = net.createServer();

server.on('connection', function(socket){
  log.watch('Connection from remote ' + socket.remoteAddress + ':' + socket.remotePort);
  socket.on('data', function(msg){
    console.log(msg);
    log.info(msg.toString());
  });
});

server.listen(port,host);
log.start("Controller is listening on " + host + ":" + port);
