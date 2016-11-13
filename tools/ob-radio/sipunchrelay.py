#/usr/bin/env python

#import threading
# import Queue as queue
#import queue
import os
import sys
from time import sleep
import logging
#from logging.handlers import RotatingFileHandler
from serial.tools import list_ports
from serial import Serial
from serial.serialutil import SerialException
from six import int2byte, byte2int
from binascii import hexlify
import pudb

from sireader import SIReader, SIReaderException, SIReaderTimeout

#logger
logger = logging.getLogger('ob')
hdlr = logging.StreamHandler(sys.stderr) 
#logging.FileHandler('/tmp/ob.log')
#hdlr = RotatingFileHandler('/tmp/ob.log', maxBytes=2048, backupCount=1)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)


#devices = {'radio':r'/dev/xbee', 'kontrola':r'/dev/ttyUSB0'}
#[ port for port in list_ports.grep("0403:6015") ] #xbee
#[ port for port in list_ports.grep("sportident.*") ]

class SIPunchReader(SIReader):
    #def __init__(self, *args, **kwargs):
    #    super(type(self), self).__init__(*args, **kwargs)

    def readCommand(self, timeout = None):

        try:
            if timeout != None:
                old_timeout = self._serial.timeout
                self._serial.timeout = timeout
            char = self._serial.read()
            if timeout != None:
                self._serial.timeout = old_timeout

            if char == b'':
                raise SIReaderTimeout('No data available')
            elif char == SIReader.NAK:
                raise SIReaderException('Invalid command or parameter.')
            elif char != SIReader.STX:
                self._serial.flushInput()
                raise SIReaderException('Invalid start byte %s' % hex(byte2int(char)))

            # Read command, length, data, crc, ETX 02 ef 01 08 ea 09 03
            cmd = self._serial.read()
            length = self._serial.read()
            station = self._serial.read(2)
            self.station_code = SIReader._to_int(station)
            data = self._serial.read(byte2int(length)-2)
            crc = self._serial.read(2)
            etx = self._serial.read()

            if self._debug:
                logger.info("<<== command '%s', len %i, station %s, data %s, crc %s, etx %s" % (hexlify(cmd).decode('ascii'),
                                                                                          byte2int(length),
                                                                                          hexlify(station).decode('ascii'),
                                                                                          ' '.join([hexlify(int2byte(c)).decode('ascii') for c in data]),
                                                                                          hexlify(crc).decode('ascii'),
                                                                                          hexlify(etx).decode('ascii'),
                                                                                          ))

            if etx != SIReader.ETX:
                raise SIReaderException('No ETX byte received.')
            if not SIReader._crc_check(cmd + length + station + data, crc):
                raise SIReaderException('CRC check failed')
                
        except (SerialException, OSError) as msg:
            raise SIReaderException('Error reading command: %s' % msg)

        raw_data = char + cmd + length + station + data + crc + etx
        return (cmd, raw_data)

class XBeeWriter(object):
    def __init__(self, port):
        self.port = port
        self._connect_serial(port)
        
    def disconnect(self):
        """Close the serial port an disconnect from the station."""
        try:
            self._serial.close()
        except:
            pass

    def reconnect(self):
        """Close the serial port and reopen again."""
        self.disconnect()
        self._connect_serial(self.port)

    def send(self, data):
        self._serial.write(data)
        self._serial.flush()

    def _connect_serial(self, port, timeout=None):
        """Connect to SI Reader.
        @param port: serial port
        """        
        self._serial = Serial(port, baudrate=38400, timeout=timeout)
        
        # flush possibly available input        
        self._serial.reset_input_buffer()
        self._serial.reset_output_buffer()

def main():  
    si_port = [ port for port in list_ports.grep("sportident.*") ].pop()
    logger.info("Opening SI reader on port %s" % (si_port))
    punch_reader = SIPunchReader(si_port.device)
    
    xbee_port = [ port for port in list_ports.grep("0403:6015") ].pop()
    logger.info("Opening writer on port %s" % (xbee_port))
    punch_writer = XBeeWriter(xbee_port.device)
    while True:
        try:
            cmd, raw_data = punch_reader.readCommand()
            if cmd == SIReader.C_TRANS_REC:
                #logger.info("==>> Sending punch %s" % (' '.join([hexlify(int2byte(c)).decode('ascii') for c in raw_data])))
                logger.info("==>> Sending punch %s" % (hexlify(raw_data)))
                punch_writer.send(raw_data)
        except SIReaderTimeout:
            pass


if __name__ == "__main__":
    
    while True:
        sleep(1)
        try:
            main()
        except Exception as e:
            #pudb.set_trace()
            logger.error(str(type(e)) + ": " + str(e))



