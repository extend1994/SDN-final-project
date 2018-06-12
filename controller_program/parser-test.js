const {Signale} = require('signale');
var debug_dump = require('buffer-hexdump'); //For debug
var hexconverter = require('convert-hex');
var parser = require('./parser');

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

log.start("-------------- hello from AP 1 --------------");
let hello_msg = [0,1,0,0];
parser.decode_header(hello_msg);
//log.start("-------------- hello from AP 2 --------------");
hello_msg = [1,1,0,0];
//parser.decode_header(hello_msg);
log.start("-------------- status reply from AP 1 --------------");
let status_rpy = [1,0x04,3,0x09,0x00,0x11,2];
parser.decode_header(status_rpy);
log.start("-------------- keep alive reply from AP 1--------------");
let alive_rpy = [1,0x07,0,0];
parser.decode_header(alive_rpy);
