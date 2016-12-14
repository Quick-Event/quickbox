#/usr/bin/env python

#import threading
# import Queue as queue
#import queue
import os
import sys
import time
import logging
import select
#from logging.handlers import RotatingFileHandler
from serial.tools import list_ports
from serial import Serial
from serial.serialutil import SerialException
from six import int2byte, byte2int
from binascii import hexlify
#import pudb

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

_debug = False

class SerialPort(object):
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
		self._serial.flushInput()
		self._serial.flushOutput()
		#self._serial.reset_input_buffer() #PY3
		#self._serial.reset_output_buffer() #PY3

class SIPunchReader(SerialPort):
	#def __init__(self, *args, **kwargs):
	#    super(type(self), self).__init__(*args, **kwargs)

	@staticmethod
	def readCommand_static(serial_port):

		try:
			char = serial_port.read()
			# logger.info("start: " + hex(byte2int(char)))
			#while True:
			#	char = serial_port.read()
			#	logger.info(hex(byte2int(char)))

			if char == b'':
				raise SIReaderTimeout('No data available')
			#elif char == '0':
			#	data = char + serial_port.read(15)
			#	return (char, data)
			elif char == SIReader.NAK:
				raise SIReaderException('Invalid command or parameter.')
			elif char != SIReader.STX:
				serial_port.flushInput()
				raise SIReaderException('Invalid start byte %s' % hex(byte2int(char)))

			# Read command, length, data, crc, ETX 02 ef 01 08 ea 09 03
			cmd = serial_port.read()
			# logger.info("cmd: " + hex(byte2int(cmd)))
			length = serial_port.read()
			# logger.info("length: " + hex(byte2int(length)))
			#logger.info("length: %d" % (byte2int(length)-2))
			station = serial_port.read(2)
			# logger.info("station: " + hexlify(station).decode('ascii'))
			station_code = SIReader._to_int(station)
			data = serial_port.read(byte2int(length)-2)
			# logger.info("data: " + hexlify(data).decode('ascii'))
			crc = serial_port.read(2)
			# logger.info("crc: " + hexlify(crc).decode('ascii'))
			etx = serial_port.read()
			# logger.info("etx: " + hex(byte2int(etx)))

			if etx != SIReader.ETX:
				logger.info("wrong ETX: " + hex(byte2int(etx)))
				raise SIReaderException('No ETX byte received.')
			if not SIReader._crc_check(cmd + length + station + data, crc):
				logger.info("wrong CRC: " + hexlify(crc) + " should be: " + hexlify(SIReader._crc(cmd + length + station + data)).decode('ascii'))
				#print hex(SIReader._to_int())
				raise SIReaderException('CRC check failed')
				
		except (SerialException, OSError) as msg:
			raise SIReaderException('Error reading command: %s' % msg)

		raw_data = char + cmd + length + station + data + crc + etx
		return (cmd, raw_data)

	def readCommand(self):
		return SIPunchReader.readCommand_static(self._serial)


class XBeeWriter(SerialPort):
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
		self._serial.flushInput()
		self._serial.flushOutput()
		#self._serial.reset_input_buffer() #PY3
		#self._serial.reset_output_buffer() #PY3

def main(): 
#2067939 [ d3 0d 00 7b 00 1f 8d e3 05 86 76 6d 00 01 00 ]
#4634    [ d3 0d 00 7b 00 00 12 1a 05 86 85 32 00 01 08 ]

	SportIdentVID = '10c4'
	ArduinoVID = '2341'
	XBeeVID='0403'
	si_devices = [port[0] if isinstance(port, tuple) else port.device for port in list_ports.grep(SportIdentVID + ':.*')]
	si_devices += [port[0] if isinstance(port, tuple) else port.device for port in list_ports.grep(ArduinoVID + ':.*')]
	logger.info("SI readers: %s" % si_devices)
	if len(si_devices) == 0:
		logger.info([port.hwid for port in list_ports.grep('.*')])
		raise Exception("No SI devices found!")

	xbee_devices = [port[0] if isinstance(port, tuple) else port.device for port in list_ports.grep(XBeeVID + ':.*')]
	if len(xbee_devices) == 0:
		raise Exception("No XBee device found!")
	xbee_device = xbee_devices.pop()
	logger.info("XBee writer: %s" % xbee_device)

	si_readers = [SIPunchReader(device) for device in si_devices]
	xbee_writer = XBeeWriter(xbee_device)

	rdfs = [dev._serial for dev in si_readers]
	try:
		while True:
			logger.info("select ...")
			readable, writable, exceptional = select.select(rdfs, [], [], 5)
			if not readable:
				logger.info("timeout")
			else:	
				for r in readable:
					for si in si_readers:
						#pudb.set_trace()
						if si._serial == r:
							cmd, data = si.readCommand()
							logger.info("<<== Data read %s" % (hexlify(data)))
							if cmd == SIReader.C_TRANS_REC:
								logger.info("==>> Sending punch %s" % (hexlify(data)))
								xbee_writer.send(data)
	except Exception as e:
		for rd in si_readers:
			rd.disconnect()
		xbee_writer.disconnect()
		raise e


if __name__ == "__main__":

	while True:	
		try:
			main()
		except Exception as e:
			#pudb.set_trace()
			logger.error(str(type(e)) + ": " + str(e))
		time.sleep(5)


