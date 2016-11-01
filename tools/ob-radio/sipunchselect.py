#/usr/bin/env python

#import threading
# import Queue as queue
#import queue
import os
import sys
from time import sleep
import logging
import select
#from logging.handlers import RotatingFileHandler
from serial.tools import list_ports
from serial import Serial
from serial.serialutil import SerialException
from six import int2byte, byte2int
from binascii import hexlify
import pudb

#logger
logger = logging.getLogger('ob')
hdlr = logging.StreamHandler(sys.stderr) 
#logging.FileHandler('/tmp/ob.log')
#hdlr = RotatingFileHandler('/tmp/ob.log', maxBytes=2048, backupCount=1)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)

'''
[['/dev/ttyS0', None, None],
 ['/dev/ttyUSB2', 'FTDI', 'DA01MHJF'],
 ['/dev/ttyUSB1', 'FTDI', 'DA011ZZ1'],
 ['/dev/ttyUSB0', 'FTDI', 'DA01MHJD']]
'''
arduino_params = type('obj', (object,), {"device": '/dev/ttyACM0'})
jenicek_params = type('obj', (object,), {"device": '/dev/ttyUSB0', "sn": 'DA01MHJD'})
marenka_params = type('obj', (object,), {"device": '/dev/ttyUSB2', "sn": 'DA01MHJF'})
vlk_params = type('obj', (object,), {"device": '/dev/ttyUSB1', "sn": 'DA011ZZ1'})

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
		self._serial.reset_input_buffer()
		self._serial.reset_output_buffer()

class SIPunchReader(SerialPort):
	#def __init__(self, *args, **kwargs):
	#    super(type(self), self).__init__(*args, **kwargs)

	@staticmethod
	def readCommand_static(serial_port):

		try:
			char = serial_port.read()

			if char == b'':
				raise SIReaderTimeout('No data available')
			elif char == '0':
				data = char + serial_port.read(15)
				return (char, data)
			elif char == SIReader.NAK:
				raise SIReaderException('Invalid command or parameter.')
			elif char != SIReader.STX:
				serial_port.flushInput()
				raise SIReaderException('Invalid start byte %s' % hex(byte2int(char)))

			# Read command, length, data, crc, ETX 02 ef 01 08 ea 09 03
			cmd = serial_port.read()
			length = serial_port.read()
			station = serial_port.read(2)
			self.station_code = SIReader._to_int(station)
			data = serial_port.read(byte2int(length)-2)
			crc = serial_port.read(2)
			etx = serial_port.read()

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
		self._serial.reset_input_buffer()
		self._serial.reset_output_buffer()

# hub == jenicek
# jenicek ~~~> marenka
# vlk ~~~> jenicek
# marenka ~~~> jenicek
# arduino + marenka ===> vlk
# jenicek ---> print

C_TRANS_REC      = b'\xD3' # autosend timestamp (online control)

def main(): 
	vlk = XBeeWriter(vlk_params.device)
	arduino = SIPunchReader(arduino_params.device)
	marenka = SIPunchReader(marenka_params.device)
	stations = [arduino, marenka]

	while True:
		logger.info("select ...")
		readable, writable, exceptional = select.select([arduino._serial, marenka._serial], [], [], 5)
		if not readable:
			logger.info("timeout")
		else:	
			for r in readable:
				for si in stations:
					if si._serial == r:
						cmd, data = si.readCommand()
						logger.info("<<== Data read %s" % (hexlify(data)))
						if cmd == C_TRANS_REC:
							logger.info("==>> Sending punch %s" % (hexlify(data)))
							vlk.send(data)
							punch_writer.send(raw_data)


if __name__ == "__main__":
	
	while True:
		sleep(1)
		try:
			main()
		except Exception as e:
			#pudb.set_trace()
			logger.error(str(type(e)) + ": " + str(e))



