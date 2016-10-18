import threading
# import Queue as queue
import queue
import os
from time import sleep
import logging
from logging.handlers import RotatingFileHandler
from serial.tools import list_ports
from serial import Serial
from serial.serialutil import SerialException
from six import int2byte, byte2int
from binascii import hexlify
import pudb

from sireader import SIReaderControl

#logger
logger = logging.getLogger('ob')
hdlr = logging.StreamHandler(sys.stdout) 
#logging.FileHandler('/tmp/ob.log')
#hdlr = RotatingFileHandler('/tmp/ob.log', maxBytes=2048, backupCount=1)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)


#devices = {'radio':r'/dev/xbee', 'kontrola':r'/dev/ttyUSB0'}
#[ port for port in list_ports.grep("0403:6015") ] #xbee
#[ port for port in list_ports.grep("sportident.*") ]

class SerialMixin(object):
    def __init__(self):
        self._serial = Serial()
        super(SerialMixin, self).__init__()
        
    def disconnect(self):
        """Close the serial port an disconnect from the station."""
        try:
            self._serial.close()
        except:
            pass

    def reconnect(self,port):
        """Close the serial port and reopen again."""
        self.disconnect()
        self._connect_serial(port)

    def _connect_serial(self, port, timeout=None):
        """Connect to SI Reader.
        @param port: serial port
        """        
        self._serial = Serial(port,baudrate=38400,timeout=timeout)
        if not self._serial.isOpen():
            self._serial.open()
        
        # flush possibly available input        
        self._serial.reset_input_buffer()
        self._serial.reset_output_buffer()

        self.port = port
        self.baudrate = self._serial.baudrate

class SendPunchQueue(SerialMixin, threading.Thread):
    """
        sending punches from sendQueue
    """
   
    def __init__ (self, q, port):        
        self.q = q
        self.port = port
        super(SendPunchQueue,self).__init__()
       
        
    def run(self):        
        while True:
            sleep(0.1)
            try:
                if not self._serial.isOpen():
                    raise SerialException("Port not open")                    
                if not self.q.empty():
                    q = self.q.get()                    
                    self._serial.write(q)
                    # logger.info("Sending %s" % q.encode('hex'))
            except SerialException as e:
                try:                    
                    self.reconnect(self.port)
                    logger.info("Reconnected radio %s (%s)" % (self.port,e))
                except Exception as r:
                    return

class ReadPunchQueue(SerialMixin, threading.Thread):
    """
        reading punches from serial to sendQueue
    """
    
    # Protocol characters
    STX              = b'\x02'
    ETX              = b'\x03'
    ACK              = b'\x06' # when sent to BSx3..6 with a card inserted, causes beep until SI-card taken out
    NAK              = b'\x15'
    DLE              = b'\x10'
    WAKEUP           = b'\xFF'
    
   
    def __init__ (self, q, port):
        #self.sr = SIReaderControl(port)
        self.q = q
        self.port = port
        super(ReadPunchQueue,self).__init__()    
    
    def _connect_serial(self, port, timeout=None):
        sr = SIReaderControl(port)
        self._serial = sr._serial
        sr = None
        if not self._serial.isOpen():
            self._serial.open()
        
        # flush possibly available input        
        self._serial.reset_input_buffer()
        self._serial.reset_output_buffer()

        self.port = port
        self.baudrate = self._serial.baudrate
        
    def run(self):
        while True:
            sleep(0.1)
            try:                
                if not self._serial.isOpen():
                    raise SerialException("Port not open")
                
                char = self._serial.read()                
                if char == ReadPunchQueue.STX:
                    cmd = self._serial.read()
                    length = self._serial.read()
                    station = self._serial.read(2)            
                    data = self._serial.read(byte2int(length)-2)
                    crc = self._serial.read(2)
                    etx = self._serial.read()                    
                    
                    punch_bytes = char + cmd + length + station + data + crc + etx
                    self.q.put(punch_bytes)
                    logmessage = ' '.join([hexlify(int2byte(c)).decode('ascii') for c in punch_bytes])
                    logger.info("%s" % logmessage)
                self._serial.reset_input_buffer()
                
                # if self._serial.in_waiting > 0:
                #     punch_bytes = self._serial.read(self._serial.in_waiting)
                #     logger.info("%s" % punch_bytes.encode('hex'))
                #     self.q.put(punch_bytes)
                # self.q.put(punch_bytes)
                                
            except SerialException as e:
                try:                    
                    self.reconnect(self.port)
                    # sr = SIReaderControl(self.port)
                    # self._serial = sr._serial
                    logger.info("Reconnected controll %s: (%s)" % (self.port,e))
                except Exception as r:
                    return #terminate thread            
                

# def devices_present(devices):
#     if len(devices) == 0: return False
#     for i in devices.values():
#         if not os.path.exists(i):
#             return False
#     return True

# def _connect_kontrola(kontrola):
#     sr = SIReaderControl(kontrola)
#     return sr._serial


def main():    
    sendQueue = queue.Queue()
    senderThread = SendPunchQueue(sendQueue, None)
    senderThread.daemon = True    
    senderThread.start()
    
    
    #reader_pool = []    
    readerThread = ReadPunchQueue(sendQueue, None)
    readerThread.daemon = True #exit mozny jen kdyz zbyvaji pouze daemon threads
    readerThread.start()
    
    #second control - TODO: prepsat
    readerThread2 = ReadPunchQueue(sendQueue, None)
    readerThread2.daemon = True #exit mozny jen kdyz zbyvaji pouze daemon threads
    readerThread2.start()
    
    try:            
        while True:
            sleep(0.1) #interrupt
            if not senderThread.isAlive():
                try:
                    port = [ port for port in list_ports.grep("0403:6015") ].pop()
                    senderThread = SendPunchQueue(sendQueue,port.device)
                    senderThread.daemon = True #exit mozny jen kdyz zbyvaji pouze daemon threads
                    senderThread.start()
                except:
                    pass
            
            ports = [ port for port in list_ports.grep("sportident.*") ]
            if not readerThread.isAlive():
                try:
                    try:
                        port = ports[0]
                    except:
                        port = None
                    readerThread = ReadPunchQueue(sendQueue,port.device)
                    readerThread.daemon = True #exit mozny jen kdyz zbyvaji pouze daemon threads
                    readerThread.start()
                except:
                    pass
            
            if not readerThread2.isAlive():
                try:
                    try:
                        port = ports[1]
                    except:
                        port = None
                    readerThread2 = ReadPunchQueue(sendQueue,port.device)
                    readerThread2.daemon = True #exit mozny jen kdyz zbyvaji pouze daemon threads
                    readerThread2.start()
                except:
                    pass
            senderThread.join(1)
    except KeyboardInterrupt:
        exit()
        

if __name__ == "__main__":
    main()

