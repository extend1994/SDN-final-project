const {Signale} = require('signale');
var bitset = require('bitset');
var hexconverter = require('convert-hex');
var pad = require('pad');

const options = {
  types: {
    debug: {
      badge: '☢',
      color: 'yellowBright',
      label: 'debug'
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
  message.bitmap = 0x01;
  message.payload.push(new_AP_ID);
  console.log(message);
}

function encode_initial_set(AP_ID) {
  message.ID = AP_ID;
  recorder[AP_ID-1].set_parameter.CHN = channel_set.includes(1) == false ? 1 :
                                      channel_set.includes(6) == false ? 6 : 11;

  channel_set.push(recorder[AP_ID-1].set_parameter.CHN);
  recorder[AP_ID-1].set_parameter.THSSTA = 3;
  recorder[AP_ID-1].set_parameter.THSSNR = 0x43;
  recorder[AP_ID-1].set_parameter.PWR = 0x11;
  message.type = initial_set_t;
  message.length = 4;
  message.bitmap = 0x1B;
  message.payload = [3, 0x43, channel_set[channel_set.length-1], 0x11];
}

function analyze_and_adjust_status() {
  //DO nothing for temporarily
}

function encode_set(AP_ID) {
  message.ID = AP_ID;
  recorder[AP_ID-1].set_parameter.PWR = 20;
  recorder[AP_ID-1].set_parameter.BEACON = 1;
  message.type = set_t;
  message.length = 2;
  message.bitmap = 0x03;
  message.payload.push([20,1]);
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

function decode_header(recv_msg) {

  if(recv_msg.constructor.toString().includes('Uint8Array')){
    recv_msg = Array.from(recv_msg);
  }
  decode_msg = hexconverter.bytesToHex(recv_msg);
  header.ID = hexToInt(decode_msg,0,2);
  header.type = hexToInt(decode_msg,2,4);
  header.length = hexToInt(decode_msg,4,6);
  header.bitmap = pad(8, hexToBin(decode_msg,6,8), '0');

  for(var i = 0; i < header.length; i++){
    header.payload.push(hexToInt(decode_msg,8+i*2,10+i*2));
  }

  // Checking payload values
  for(var i = 0; i < header.payload.length; i++){
    log.debug("payload: " + header.payload[i]);
  }

  switch (header.type) {

    case hello_t:
      log.info("Decoded as a hello message");
      if (header.ID == 0) {
        encode_hello_back();
        // TCP send hello back msg
        //log.debug("Hello back message");
        //console.log(message);
        var AP_ID = message.payload[message.payload.length-1];
        clean_message();
        encode_initial_set(AP_ID);
        //log.debug("Init set message");
        //console.log(message);
        // TCP send initial set msg
        encode_keep_alive(AP_ID);
        //log.debug("Keep alive message");
        //console.log(message);
      } else {
        log.error("Hello message with wrong ID or type");
        log.error("Tell AP to hello again?");
      }

      break;

    case status_rpy_t:
      log.info("Decoded as a status reply message");
      if (header.ID != 0) {
        if (header.bitmap.charAt(4)  == "1") {
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
        console.log(ap_condition);
        recorder[header.ID-1].condition = ap_condition;
        analyze_and_adjust_status();
        encode_set(header.ID);
      } else {
        log.error("Status reply should NOT bring ID = 0");
      }
      break;

    case alive_rpy_t:
      log.info("Decoded as a keep alive reply message");
      if (header.ID != 0 && header.length == 0 && header.bitmap == "00000000") {
        log.success("AP is still alive!");
        //setTimeout(encode_keep_alive, 30000, header.ID);
        if(test_flag) encode_keep_alive(header.ID);

        test_flag = 0;
      } else {
        log.error("Alive rpy should NOT bring ID = 0 or non-zero length or bitmap");
      }
      break;

    default:
      log.error("Receive a message with unexpected type!");
  }
  clean_header();
}

module.exports = {
  decode_header: decode_header
}
