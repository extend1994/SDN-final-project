const net = require('net');
const parser = require('./parser');
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
    var recv_data = Array.from(msg);
    log.info(recv_data.toString());
    parser.decode_header(recv_data);
  });
});

server.listen(port,host);
log.start("Controller is listening on " + host + ":" + port);
