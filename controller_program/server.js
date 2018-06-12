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

console.log("Usage: node server.js [<THSSTA> <THSSNR> <THSPKC>]");

var THSSTA = (process.argv[2] != undefined) ? process.argv[2] : 1000;
var THSSNR = (process.argv[3] != undefined) ? process.argv[3] : 0x43;
var THSPKC = (process.argv[4] != undefined) ? process.argv[4] : 50;

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
let socket_set = [];

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

function clean_ap_condition() {
  ap_condition.SCHRES = "";
  ap_condition.AVGSNR = 0;
  ap_condition.PKTCNT = 0;
  ap_condition.NUMSTA = 0;
}

function length_cal(type, input){

  var payload_len = 0;
  if (input[0]) payload_len++;
  if (input[1]) {
    if (type == 3) payload_len += 4; //THSSNR needs 4 bytes
    else payload_len ++; //PWR only needs 1 bytes
  }
  if (input[2]) payload_len++;
  if (type == 3) {
    if (input[3]) payload_len++;
    if (input[4]) payload_len++; 
  }

  return payload_len;
}

function encode_hello_back() {

  initial_parameter.THSSTA = 0;
  initial_parameter.THSSNR = 0;
  initial_parameter.THSPKC = 0;
  initial_parameter.CHN = 0;
  initial_parameter.PWR = 0;

  ap_condition.SCHRES = "";
  ap_condition.AVGSNR = 0;
  ap_condition.PKTCNT = 0;
  ap_condition.NUMSTA = 0;

  set_parameter.CHN = 0;
  set_parameter.PWR = 0;
  set_parameter.BEACON = 0;

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
  /* Initial set */
  recorder[AP_ID-1].initial_parameter.CHN = 
    channel_set.includes(1) ==  false ? 1 :
    channel_set.includes(6) ==  false ? 6 : 
    channel_set.includes(11) == false ? 11 :
    channel_set[Math.floor(Math.random() * channel_set.length)];

  recorder[AP_ID-1].initial_parameter.THSSTA = THSSTA; // argv
  recorder[AP_ID-1].initial_parameter.THSSNR = THSSNR; // argv
  recorder[AP_ID-1].initial_parameter.THSPKC = THSPKC; // argv
  if (channel_set.length > 0) recorder[AP_ID-1].initial_parameter.PWR = 3;
  else recorder[AP_ID-1].initial_parameter.PWR = 20;
  channel_set.push(recorder[AP_ID-1].initial_parameter.CHN);
  /* End of initial set */
  var init_prmt = recorder[AP_ID-1].initial_parameter;
  log.debug('================= Initial recorder ' + AP_ID + ' =================');
  log.debug(JSON.stringify(recorder[AP_ID-1]));
  /* message encoding */
  message.type = initial_set_t;
  message.length = length_cal(3, Object.values(init_prmt));
  message.bitmap = 2; //CHN must be set
  if (init_prmt.THSSTA) message.bitmap += 16;
  if (init_prmt.THSSNR) message.bitmap += 8;
  if (init_prmt.THSPKC) message.bitmap += 4;
  if (init_prmt.PWR) message.bitmap += 1;
  message.payload.push(init_prmt.THSSTA);
  message.payload.push(init_prmt.THSSNR);
  message.payload.push(0,0,0,init_prmt.THSPKC);
  message.payload.push(init_prmt.CHN);
  message.payload.push(init_prmt.PWR);
}

function occupied_channel_report(){
  var available = [1,2,3,4,5,6,7,8,9,10,11,12,13];
  for(var i = 0; i < 16; ){
    //Search bitmap from index i+1
    var index = ap_condition.SCHRES.indexOf('1',i+1);
    if(index == -1){
      i = 16;
    } else {
      i = index;
      var channel = 16 - index;
      //Remove array element from index i
      available.splice(channel-1,1);
    }
  }
  log.debug('Available channels: ' + Array.from(available).toString());
  return available;
}

function analyze_and_adjust_status(cond,id) {

  var parameter = {
    CHN: 0,
    PWR: 0,
    BEACON: 0
  };

  /* Channel reallocate */
  var available_channel = occupied_channel_report();
  var collision;
  var available_CHN1 = available_channel.indexOf(1)>=0 ? 1 : 0;
  var available_CHN6 = available_channel.indexOf(6)>=0 ? 1 : 0;
  var available_CHN11 = available_channel.indexOf(11)>=0 ? 1 : 0;
  var set_CHN = recorder[id].set_parameter.CHN;
  var init_CHN = recorder[id].initial_parameter.CHN;

  // Check if current channel is the same as surrounding AP
  // If the current channel can NOT be found in available channels, collision!
  if (set_CHN > 0) collision = available_channel.indexOf(set_CHN) < 0 ? 1 : 0;
  else collision = available_channel.indexOf(init_CHN) < 0 ? 1 : 0;

  // When the same, pick channel 1/6/11 which is not in use, else random pick one
  if (collision) {
    log.warn('Collision! Current channel is either ' + set_CHN + ' or (init) ' + init_CHN);
    parameter.CHN = available_CHN1 ? 1 :
                        available_CHN6 ? 6 :
                        available_CHN11? 11 :
                        available_channel[Math.floor(Math.random() * available_channel.length)];
    log.warn('New channel: ' + parameter.CHN);
  }

  /* For load balance and power saving, packet count is too much */
  if (cond.PKTCNT > 0){
    parameter.BEACON = 1; // TODO:Next should awake another AP
  }

  /* For power saving */
  // No body is using but power is too much => power down
  //if (cond.NUMSTA == 0) {
  //  parameter.PWR = 3;
  //}

  return parameter;
}

function encode_set(AP_ID) {
  clean_message();
  message.ID = AP_ID;
  var cond = recorder[AP_ID-1].condition;
  var parameter = analyze_and_adjust_status(cond, AP_ID-1);
  message.type = set_t;
  message.length = 0;
  message.bitmap = 0;
  if (parameter.CHN) {
    recorder[AP_ID-1].set_parameter.CHN = parameter.CHN;
    message.length += 1;
    message.bitmap += 4;
    message.payload.push(parameter.CHN);
  }
  if (parameter.PWR) {
    recorder[AP_ID-1].set_parameter.PWR = parameter.PWR;
    message.length += 1;
    message.bitmap += 2;
    message.payload.push(parameter.PWR);
  }
  //if (parameter.BEACON != recorder[AP_ID-1].set_parameter.BEACON) {
  if (parameter.BEACON) {
    recorder[AP_ID-1].set_parameter.BEACON = parameter.BEACON;
    message.length +=1;
    message.bitmap += 1;
    message.payload.push(parameter.BEACON);
  }
  log.view('********************* set parameter *******************');
  log.debug(JSON.stringify(parameter));
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

  if (recorder[header.ID-1]){
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
      clean_ap_condition();

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

        log.debug('----------------  ap_condition from AP '+ header.ID +'-----------------');
        log.debug(JSON.stringify(ap_condition));
        recorder[header.ID-1].condition = ap_condition;
        log.debug('================= Recorder ' + header.ID + ' BEFORE set =================');
        log.debug(JSON.stringify(recorder[header.ID-1]));
        encode_set(header.ID);
        log.debug('================= Recorder ' + header.ID + ' after set =================');
        log.debug(JSON.stringify(recorder[header.ID-1]));
        log.view("Go set: " + JSON.stringify(message));
      	send_msg = encode_msg();
        socket.write(send_msg);
        /* Power up anther AP to load balance */
        if (recorder[header.ID-1].set_parameter.BEACON == 1){
          if (recorder[header.ID]) {
            log.start("Wake up new AP!");
            recorder[header.ID-1].set_parameter.PWR = 20; // No need to change CHN & BEACON
            var msg = new Buffer([header.ID, set_t, 1, 2, 20]);
            var next_ap_socket = socket_set[header.ID];
            if (next_ap_socket) {
              log.start('Socket exist! Wake Up!!');
              next_ap_socket.write(msg);
            }
          }
        }
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
var host = '0.0.0.0';
var port = 8899;
var server = net.createServer();

server.on('connection', function(socket){
  log.watch('Connection from remote ' + socket.remoteAddress + ':' + socket.remotePort);
  socket.on('data', function(msg){
    var recv_data = Array.from(msg);
    log.info(recv_data.toString());
    decode_header(recv_data, socket);
    socket_set.push(socket);
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
