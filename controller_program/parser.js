'use strict';

const net = require('net');
const {Signale} = require('signale');
var bitset = require('bitset');
var hexconverter = require('convert-hex');
var pad = require('pad');
var emoji = require('node-emoji');

const options = {
  types: {
    debug: {
      badge: emoji.get('radioactive_sign'),
      color: 'yellowBright',
      label: 'debug'
    },
    info: {
      badge: emoji.get('envelope'),
      color: 'blue',
      label: 'recv_msg'
    },
    view: {
      badge: emoji.get('pencil2'),
      color: 'cyan',
      label: 'send_msg'
    }
  }
};

const log = new Signale(options);
var test_flag = 1;

/* Global AP information */
let header = {
  ID: 0,
  type: 0x00,
  length: 0,
  bitmap: "00000000",
  payload: []
}

let message = header;
let channel_set = [];
let recorder = []; //[{init_parameter, condition, set_parameter}]

/* type definitions */
let hello_t       = 0x01;
let hello_back_t  = 0x02;
let initial_set_t = 0x03;
let status_rpy_t  = 0x04;
let set_t         = 0x05;
let keep_alive_t  = 0x06;
let alive_rpy_t   = 0x07;

//THS: Threshold
let initial_parameter = {
  THSSTA: 0, //Station
  THSSNR: 0, //SNR
  THSPKC: 0, //Pakcet count
  CHN: 0,    //Channel
  PWR: 0     //Power
};

let ap_condition = {
  SCHRES: "", //Scanned channels
  AVGSNR: 0, //Average SNR
  PKTCNT: 0, //Packet Count, 32bits
  NUMSTA: 0  //Number of STA
};

let set_parameter = {
  CHN: 0,
  PWR: 0,
  BEACON: 0
};

function clean_header() {
  header.ID = 0;
  header.type = 0;
  header.length = 0;
  header.bitmap = "00000000";
  header.payload = [];
}

function clean_message() {
  message.ID = 0;
  message.type = 0x00;
  message.length = 0;
  message.bitmap = 0x00;
  message.payload = [];
}

function encode_hello_back() {

  var new_AP_ID = recorder.push({
                                  initial_parameter: initial_parameter,
                                  condition:         ap_condition,
                                  set_parameter:     set_parameter
                                });
  message.ID = 0;
  message.type = hello_back_t;
  message.length = 1;
  message.bitmap = 0x02;
  message.payload.push(new_AP_ID);
}

function encode_initial_set(AP_ID) {

  clean_message();
  message.ID = AP_ID;
  recorder[AP_ID-1].set_parameter.CHN = channel_set.includes(1) == false ? 1 :
                                      channel_set.includes(6) == false ? 6 : 11;

  channel_set.push(recorder[AP_ID-1].set_parameter.CHN);
  recorder[AP_ID-1].set_parameter.THSSTA = 3;
  recorder[AP_ID-1].set_parameter.THSSNR = 0x43;
  recorder[AP_ID-1].set_parameter.THSPKC = 10;
  recorder[AP_ID-1].set_parameter.PWR = 0x11;
  message.type = initial_set_t;
  message.length = 8;
  message.bitmap = 0x1F;
  message.payload = [3, 0x43, 0x00, 0x00, 0x00, 0x0A, channel_set[channel_set.length-1], 0x11];
}

function analyze_and_adjust_status() {
  //DO nothing for temporarily
}

function occupied_channel_report(){
  for(var i = 0; i < 16; ){
    var index = ap_condition.SCHRES.indexOf('1',i+1);
    if(index == -1){
      i = 16;
    } else {
      i = index;
      var channel = 16 - index;
      log.debug('channel ' + channel + ' is occupied!');
    }
  }
}

function encode_set(AP_ID) {
  clean_message();
  message.ID = AP_ID;
  recorder[AP_ID-1].set_parameter.PWR = 20;
  message.type = set_t;
  message.length = 2;
  message.bitmap = 0x06;
  message.payload = [6,20];
}

function encode_keep_alive(AP_ID) {
  clean_message();
  message.ID = AP_ID;
  message.type = keep_alive_t;
}

function hexToInt(hex, start, end) {
  return parseInt(hex.slice(start,end),16);
}

function hexToBin(hex, start, end) {
  return bitset.fromHexString(hex.slice(start,end)).toString();
}

function encode_msg(){
  var msg = [];
  msg.push(message.ID);
  msg.push(message.type);
  msg.push(message.length);
  msg.push(message.bitmap);
  for(var i=0;i<message.payload.length;i++){
    msg.push(message.payload[i]);
  }
  return new Buffer(msg);
}

function decode_header(recv_msg, socket) {

  var decode_msg = hexconverter.bytesToHex(recv_msg);
  header.ID = hexToInt(decode_msg,0,2);
  header.type = hexToInt(decode_msg,2,4);
  header.length = hexToInt(decode_msg,4,6);
  header.bitmap = pad(8, hexToBin(decode_msg,6,8), '0');

  for(var i = 0; i < header.length; i++){
    header.payload.push(hexToInt(decode_msg,8+i*2,10+i*2));
  }
  
  var send_msg;
  switch (header.type) {

    case hello_t:
      log.info("Decoded as a hello message");
      if (header.ID == 0) {
        encode_hello_back();
        send_msg = encode_msg();
        socket.write(send_msg);
        log.view("Hello back: " + JSON.stringify(message));

        var AP_ID = message.payload[message.payload.length-1];
        encode_initial_set(AP_ID);
        send_msg = encode_msg();
        socket.write(send_msg);
        log.view("Initial Set: " + JSON.stringify(message));

        encode_keep_alive(AP_ID);
	      send_msg = encode_msg();

      	setTimeout(function(){
          socket.write(send_msg);
          log.view("Keep alive: " + Array.from(send_msg));
        }, 2000);
      } else {
        log.error("Hello message with wrong ID or type");
        log.error("Tell AP to hello again?");
      }

      break;

    case status_rpy_t:

      log.info("Decoded as a status reply message");

      if (header.ID != 0) {
        if (header.bitmap.charAt(4)  == "1") {
          ap_condition.SCHRES = "";
          for(var i=0; i<2; i++){
            var extract_byte = pad(8,header.payload[0].toString(2),'0');
            ap_condition.SCHRES += extract_byte; //Becomes a bitmap
            header.payload.shift();
          }
        }

        if (header.bitmap.charAt(5)  == "1") {
          ap_condition.AVGSNR = header.payload[0];
          header.shift();
        }

        if (header.bitmap.charAt(6)  == "1") {
          var extract_byte = "";
          for(var i=0; i<4; i++){
            extract_byte += pad(8,header.payload[0].toString(2),'0');
            header.payload.shift();
          }
          ap_condition.PKTCNT = parseInt(extract_byte,2);
        }

        if (header.bitmap.charAt(7)  == "1") {
          ap_condition.NUMSTA = header.payload[0];
        }

      	log.debug("------ ap_condition -------");
        log.debug(JSON.stringify(ap_condition));
        occupied_channel_report();
        recorder[header.ID-1].condition = ap_condition;
        analyze_and_adjust_status();
        encode_set(header.ID);
      	send_msg = encode_msg();
        socket.write(send_msg);
      } else {
        log.error("Status reply should NOT bring ID = 0");
      }
      break;

    case alive_rpy_t:

      log.info("Decoded as a keep alive reply message");
      if (header.ID != 0 && header.length == 0 && header.bitmap == "00000000") {

        log.success("AP is still alive!");
        encode_keep_alive(header.ID);
        send_msg = encode_msg();

        setTimeout(function(){
          socket.write(send_msg);
          log.view("Keep alive: " + Array.from(send_msg));
        }, 30000);
      } else {
        log.error("Alive rpy should NOT bring ID = 0 or non-zero length or bitmap");
      }
      break;

    default:
      log.error("Receive a message with unexpected type!");
  }
  clean_header();
}


/* Start a server and accept incoming connections */
var host = process.argv[2] || '0.0.0.0';
var port = process.argv[3] || 8899;
var server = net.createServer();

server.on('connection', function(socket){
  log.watch('Connection from remote ' + socket.remoteAddress + ':' + socket.remotePort);
  socket.on('data', function(msg){
    var recv_data = Array.from(msg);
    log.info(recv_data.toString());
    decode_header(recv_data, socket);
  });

  socket.on('end', function(test){
    log.debug('Lose connection of remote ' + socket.remoteAddress + ':' + socket.remotePort);
  }) 
});


server.listen(port,host);
log.start("Controller is listening on " + host + ":" + port);

module.exports = {
  decode_header: decode_header
}
