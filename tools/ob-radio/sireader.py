#/usr/bin/env python
#
#    Copyright (C) 2008-2014  Gaudenz Steinlin <gaudenz@durcheinandertal.ch>
#                       2014  Simon Harston <simon@harston.de>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""
sireader.py - Classes to read out si card data from BSM-7/8 stations.
"""

from __future__ import print_function
from six import int2byte, byte2int, iterbytes, PY3
if PY3:
    # Make byte2int on Python 3.x compatible with
    # the fact that indexing into a byte variable
    # already returns an integer. With this byte2int(b[0])
    # works on 2.x and 3.x
    def byte2int(x):
        try:
            return x[0]
        except TypeError:
            return x

from serial import Serial
from serial.serialutil import SerialException
from datetime import datetime, timedelta, time
from binascii import hexlify
import os, re

class SIReader(object):
    """Base protocol functions and constants to interact with SI Stations.
       This class has a lot of Constants defined that are not (yet) used.
       This is mainly for documentation purpose as most of this is not
       documented by SportIdent."""

    CRC_POLYNOM      = 0x8005
    CRC_BITF         = 0x8000

    # Protocol characters
    STX              = b'\x02'
    ETX              = b'\x03'
    ACK              = b'\x06' # when sent to BSx3..6 with a card inserted, causes beep until SI-card taken out
    NAK              = b'\x15'
    DLE              = b'\x10'
    WAKEUP           = b'\xFF'

    # Basic protocol commands, currently unused
    BC_SET_CARDNO    = b'\x30'
    BC_GET_SI5       = b'\x31' # read out SI-card 5 data
    BC_TRANS_REC     = b'\x33' # autosend timestamp (online control) in very old stations (BSF3)
    BC_SI5_WRITE     = b'\x43' # write SI-card 5 data page: 02 43 (page: 0x30 to 0x37) (16 bytes) 03
    BC_SI5_DET       = b'\x46' # SI-card 5 inserted (46 49) or removed (46 4F)
    BC_TRANS_REC2    = b'\x53' # autosend timestamp (online control)
    BC_TRANS_TIME    = b'\x54' # autosend timestamp (lightbeam trigger)
    BC_GET_SI6       = b'\x61' # read out SI-card 6 data (and in compatibility mode: model SI-card 8/9/10/11/SIAC/pCard/tCard as SI-card 6)
    BC_SI6_WRITEPAGE = b'\x62'# write SI-card 6 data page: 02 62 (block: 0x00 to 0x07) (page: 0x00 to 0x07) (16 bytes) 03
    BC_SI6_READWORD  = b'\x63'# read SI-card 6 data word: 02 63 (block: 0x00 to 0x07) (page: 0x00 to 0x07) (word: 0x00 to 0x03) 03
    BC_SI6_WRITEWORD = b'\x64'# write SI-card 6 data word: 02 64 (block: 0x00 to 0x07) (page: 0x00 to 0x07) (word: 0x00 to 0x03) (4 bytes) 03
    BC_SI6_DET       = b'\x66' # SI-card 6 inserted
    BC_SET_MS        = b'\x70' # \x4D="M"aster, \x53="S"lave
    BC_GET_MS        = b'\x71'
    BC_SET_SYS_VAL   = b'\x72'
    BC_GET_SYS_VAL   = b'\x73'
    BC_GET_BACKUP    = b'\x74' # Note: response carries b'\xC4'!
    BC_ERASE_BACKUP  = b'\x75'
    BC_SET_TIME      = b'\x76'
    BC_GET_TIME      = b'\x77'
    BC_OFF           = b'\x78'
    BC_RESET         = b'\x79'
    BC_GET_BACKUP2   = b'\x7A' # (for extended start and extended finish only) Note: response carries b'\xCA'!
    BC_SET_BAUD      = b'\x7E' # \x00=4800 baud, \x01=38400 baud

    # Extended protocol commands
    C_GET_BACKUP     = b'\x81'
    C_SET_SYS_VAL    = b'\x82'
    C_GET_SYS_VAL    = b'\x83'
    C_SRR_WRITE      = b'\xA2' # ShortRangeRadio - SysData write
    C_SRR_READ       = b'\xA3' # ShortRangeRadio - SysData read
    C_SRR_QUERY      = b'\xA6' # ShortRangeRadio - network device query
    C_SRR_PING       = b'\xA7' # ShortRangeRadio - heartbeat from linked devices, every 50 seconds
    C_SRR_ADHOC      = b'\xA8' # ShortRangeRadio - ad-hoc message, f.ex. from SI-ActiveCard
    C_GET_SI5        = b'\xB1' # read out SI-card 5 data
    C_SI5_WRITE      = b'\xC3' # write SI-card 5 data page: 02 C3 11 (page: 0x00 to 0x07) (16 bytes) (CRC) 03
    C_TRANS_REC      = b'\xD3' # autosend timestamp (online control)
    C_CLEAR_CARD     = b'\xE0' # found on SI-dev-forum: 02 E0 00 E0 00 03 (http://www.sportident.com/index.php?option=com_kunena&view=topic&catid=8&id=56#59)
    C_GET_SI6        = b'\xE1' # read out SI-card 6 data block
    C_SI5_DET        = b'\xE5' # SI-card 5 inserted
    C_SI6_DET        = b'\xE6' # SI-card 6 inserted
    C_SI_REM         = b'\xE7' # SI-card removed
    C_SI9_DET        = b'\xE8' # SI-card 8/9/10/11/p/t inserted
    C_SI9_WRITE      = b'\xEA' # write data page (double-word)
    C_GET_SI9        = b'\xEF' # read out SI-card 8/9/10/11/p/t data block
    C_SET_MS         = b'\xF0' # \x4D="M"aster, \x53="S"lave
    C_GET_MS         = b'\xF1'
    C_ERASE_BACKUP   = b'\xF5'
    C_SET_TIME       = b'\xF6'
    C_GET_TIME       = b'\xF7'
    C_OFF            = b'\xF8'
    C_BEEP           = b'\xF9' # 02 F9 01 (number of beeps) (CRC16) 03
    C_SET_BAUD       = b'\xFE' # \x00=4800 baud, \x01=38400 baud

    # Protocol Parameters
    P_MS_DIRECT      = b'\x4D' # "M"aster
    P_MS_INDIRECT    = b'\x53' # "S"lave
    P_SI6_CB         = b'\x08' # CardBlocks (see also O_SI6_CB)

    # offsets in system data
    # Thanks to Simon Harston <simon@harston.de> for most of this information
    # currently only O_MODE, O_STATION_CODE and O_PROTO are used
    O_OLD_SERIAL     = b'\x00' # 2 bytes - only up to BSx6, numbers < 65.536
    O_OLD_CPU_ID     = b'\x02' # 2 bytes - only up to BSx6, numbers < 65.536
    O_SERIAL_NO      = b'\x00' # 4 bytes - only after BSx7, numbers > 70.000
                               #   (if byte 0x00 > 0, better use OLD offsets)
    O_SRR_CFG        = b'\x04' # 1 byte - SRR-dongle configuration, bit mask value:
                               #   xxxxxx1xb Auto send SIAC data
                               #   xxxxx1xxb Sync time via radio
    O_FIRMWARE       = b'\x05' # 3 bytes
    O_BUILD_DATE     = b'\x08' # 3 bytes - YYMMDD
    O_MODEL_ID       = b'\x0B' # 2 bytes:
                               #   6F21: SIMSRR1-AP (ShortRangeRadio AccessPoint = SRR-dongle)
                               #   8003: BSF3 (serial numbers > 1.000)
                               #   8004: BSF4 (serial numbers > 10.000)
                               #   8084: BSM4-RS232
                               #   8086: BSM6-RS232 / BSM6-USB
                               #   8115: BSF5 (serial numbers > 50.000)
                               #   8117 / 8118: BSF7 / BSF8 (serial no. 70.000...70.521, 72.002...72.009)
                               #   8146: BSF6 (serial numbers > 30.000)
                               #   8187 / 8188: BS7-SI-Master / BS8-SI-Master
                               #   8197: BSF7 (serial numbers > 71.000, apart from 72.002...72.009)
                               #   8198: BSF8 (serial numbers > 80.000)
                               #   9197 / 9198: BSM7-RS232, BSM7-USB / BSM8-USB, BSM8-SRR
                               #   9199: unknown
                               #   9597: BS7-S (Sprinter)
                               #   9D9A: BS11-BL (SIAC / Air+)
                               #   B197 / B198: BS7-P / BS8-P (Printer)
                               #   B897: BS7-GSM
                               #   CD9B: BS11-BS-red / BS11-BS-blue (SIAC / Air+)

    O_MEM_SIZE       = b'\x0D' # 1 byte - in KB
    O_BAT_DATE       = b'\x15' # 3 bytes - YYMMDD
    O_BAT_CAP        = b'\x19' # 2 bytes - battery capacity in mAh (as multiples of 14.0625?!)
    O_BACKUP_PTR     = b'\x1C' # 4 bytes - at positions 1C,1D,21,22
    O_SI6_CB         = b'\x33' # 1 byte - bitfield defining which SI Card 6 blocks to read:
                               #   \x00=\xC1=read block0,6,7; \x08=\xFF=read all 8 blocks
    O_SRR_CHANNEL    = b'\x34' # 1 byte - SRR-dongle frequency band: 0x00="red", 0x01="blue"
    O_MEM_OVERFLOW   = b'\x3D' # 1 byte - memory overflow if != 0x00
    O_PROGRAM        = b'\x70' # 1 byte - station program: xx0xxxxxb competition, xx1xxxxxb training
    O_MODE           = b'\x71' # 1 byte - see SI station modes below
    O_STATION_CODE   = b'\x72' # 1 byte
    O_FEEDBACK       = b'\x73' # 1 byte - feedback on punch (and other unknown bits), bit mask value:
                               #   xxxxxxx1b optical feedback
                               #   xxxxx1xxb audible feedback
    O_PROTO          = b'\x74' # 1 byte - protocol configuration, bit mask value:
                               #   xxxxxxx1b extended protocol
                               #   xxxxxx1xb auto send out
                               #   xxxxx1xxb handshake (only valid for card readout)
                               #   xxx1xxxxb access with password only
                               #   1xxxxxxxb read out SI-card after punch (only for punch modes;
                               #             depends on bit 2: auto send out or handshake)
    O_WAKEUP_DATE    = b'\x75' # 3 bytes - YYMMDD
    O_WAKEUP_TIME    = b'\x78' # 3 bytes - 1 byte day (see below), 2 bytes seconds after midnight/midday
    O_SLEEP_TIME     = b'\x7B' # 3 bytes - 1 byte day (see below), 2 bytes seconds after midnight/midday
                               #   xxxxxxx0b - seconds relative to midnight/midday: 0 = am, 1 = pm
                               #   xxxx000xb - day of week: 000 = Sunday, 110 = Saturday
                               #   xx00xxxxb - week counter 0..3, relative to programming date

    # SI station modes
    M_SIAC_SPECIAL     = 0x01 # SI Air+ special register set (ON, OFF, Radio_ReadOut, etc.)
    M_CONTROL          = 0x02
    M_START            = 0x03
    M_FINISH           = 0x04
    M_READOUT          = 0x05
    M_CLEAR_OLD        = 0x06 # without start-number (not used anymore)
    M_CLEAR            = 0x07 # with start-number = standard
    M_CHECK            = 0x0A
    M_PRINTOUT         = 0x0B # BS7-P Printer-station (Note: also used by SRR-Receiver-module)
    M_START_TRIG       = 0x0C # BS7-S (Sprinter) with external trigger
    M_FINISH_TRIG      = 0x0D # BS7-S (Sprinter) with external trigger
    M_BC_CONTROL       = 0x12 # SI Air+ / SIAC Beacon mode
    M_BC_START         = 0x13 # SI Air+ / SIAC Beacon mode
    M_BC_FINISH        = 0x14 # SI Air+ / SIAC Beacon mode
    M_BC_READOUT       = 0x15 # SI Air+ / SIAC Beacon mode
    SUPPORTED_MODES    = (M_CONTROL, M_START, M_FINISH, M_READOUT, M_CLEAR, M_CHECK)

    # Weekday encoding (only for reference, currently unused)
    D_SUNDAY           = 0b000
    D_MONDAY           = 0b001
    D_TUESDAY          = 0b010
    D_WEDNESDAY        = 0b011
    D_THURSDAY         = 0b100
    D_FRIDAY           = 0b101
    D_SATURDAY         = 0b110
    D_UNKNOWN          = 0b111  # in D3-message from SIAC-beacon where no weekday-info is transmitted

    # Backup memory record length
    REC_LEN            = 8 # Only in extended protocol, otherwise 6!

    # General card data structure values
    TIME_RESET         = b'\xEE\xEE'

    # SI Card data structures
    CARD               = {'SI5':{'CN2': 6,   # card number byte 2
                                 'CN1': 4,   # card number byte 1
                                 'CN0': 5,   # card number byte 0
                                 'STD': None,# start time day
                                 'ST' : 19,  # start time
                                 'FTD': None,# finish time day
                                 'FT' : 21,  # finish time
                                 'CTD': None,# check time day
                                 'CT' : 25,  # check time
                                 'LTD': None,# clear time day
                                 'LT' : None,# clear time
                                 'RC' : 23,  # punch counter
                                 'P1' : 32,  # first punch
                                 'PL' : 3,   # punch data length in bytes
                                 'PM' : 30,  # punch maximum (punches 31-36 have no time)
                                 'CN' : 0,   # control number offset in punch record
                                 'PTD': None,# punchtime day byte offset in punch record
                                 'PTH': 1,   # punchtime high byte offset in punch record
                                 'PTL': 2,   # punchtime low byte offset in punch record
                             },
                          'SI6':{'CN2': 11,
                                 'CN1': 12,
                                 'CN0': 13,
                                 'STD': 24,
                                 'ST' : 26,
                                 'FTD': 20,
                                 'FT' : 22,
                                 'CTD': 28,
                                 'CT' : 30,
                                 'LTD': 32,
                                 'LT' : 34,
                                 'RC' : 18,
                                 'P1' : 128,
                                 'PL' : 4,
                                 'PM' : 64,
                                 'PTD': 0, # Day of week byte, SI6 and newer
                                 'CN' : 1,
                                 'PTH': 2,
                                 'PTL': 3,
                             },
                          'SI8':{'CN2': 25,
                                 'CN1': 26,
                                 'CN0': 27,
                                 'STD': 12,
                                 'ST' : 14,
                                 'FTD': 16,
                                 'FT' : 18,
                                 'CTD': 8,
                                 'CT' : 10,
                                 'LTD': None,
                                 'LT' : None,
                                 'RC' : 22,
                                 'P1' : 136,
                                 'PL' : 4,
                                 'PM' : 50,
                                 'PTD': 0,
                                 'CN' : 1,
                                 'PTH': 2,
                                 'PTL': 3,
                                 'BC' : 2,   # number of blocks on card (only relevant for SI8 and above = those read with C_GET_SI9)
                             },
                          'SI9':{'CN2': 25,
                                 'CN1': 26,
                                 'CN0': 27,
                                 'STD': 12,
                                 'ST' : 14,
                                 'FTD': 16,
                                 'FT' : 18,
                                 'CTD': 8,
                                 'CT' : 10,
                                 'LTD': None,
                                 'LT' : None,
                                 'RC' : 22,
                                 'P1' : 56,
                                 'PL' : 4,
                                 'PM' : 50,
                                 'PTD': 0, # Day of week byte, SI6 and newer
                                 'CN' : 1,
                                 'PTH': 2,
                                 'PTL': 3,
                                 'BC' : 2,
                             },
                          'SI10':{'CN2': 25,     # Same data structure for SI11
                                  'CN1': 26,
                                  'CN0': 27,
                                  'STD': 12,
                                  'ST' : 14,
                                  'FTD': 16,
                                  'FT' : 18,
                                  'CTD': 8,
                                  'CT' : 10,
                                  'LTD': None,
                                  'LT' : None,
                                  'RC' : 22,
                                  'P1' : 128, # would be 512 if all blocks were read, but blocks 1-3 are skipped on readout
                                  'PL' : 4,
                                  'PM' : 64,
                                  'PTD': 0, # Day of week byte, SI6 and newer
                                  'CN' : 1,
                                  'PTH': 2,
                                  'PTL': 3,
                                  'BC' : 8,
                              },
                      }

    # punch trigger in control mode data structure
    T_OFFSET           = 8
    T_CN               = 0
    T_TIME             = 5

    # backup memory in control mode 
    BC_CN              = 3
    BC_TIME            = 8

    def __init__(self, port = None, debug = False, logfile = None):
        """Initializes communication with si station at port.
        @param port: Serial device for the connection if port is None it
                     scans all available ports and connects to the first
                     reader found
        """
        self._serial = None
        self._debug = debug
        self._proto_config = None
        if logfile is not None:
            self._logfile = open(logfile, 'ab')
        else:
            self._logfile = None
            
        errors = ''
        if port is not None:
            self._connect_reader(port)
            return
        else:
            scan_ports = [ os.path.join('/dev', f) for f in os.listdir('/dev') if
                           re.match('ttyS.*|ttyUSB.*', f) ]
            if len(scan_ports) == 0:
                errors = 'no serial ports found'
                
            for port in scan_ports:
                try:
                    self._connect_reader(port)
                    return
                except (SIReaderException, SIReaderTimeout) as msg:
                    errors = '%sport: %s: %s\n' % (errors, port, msg)
                    pass

        raise SIReaderException('No SI Reader found. Possible reasons: %s' % errors)

    def set_extended_protocol(self, extended = True):
        """Configure extended protocol mode of si station.
        @param extended: Set exetended protocol if True, basic protocol if
                         False.
        """
        config = self.proto_config.copy()
        config['ext_proto'] = extended
        self._set_proto_config(config)
        self.beep()

    def set_autosend(self, autosend = True):
        """Set si station into autosend mode.
        @param autosend: Set autosend mode if True, unset otherwise.
        """
        config = self.proto_config.copy()
        config['auto_send'] = autosend
        config['handshake'] = not autosend
        self._set_proto_config(config)
        self.beep()

    def set_operating_mode(self, mode):
        """Set si station operating mode.
        @param mode: operating mode, supported modes: M_CONTROL, M_START, M_FINISH, M_READOUT, M_CLEAR, M_CHECK
        """
        if not mode in SIReader.SUPPORTED_MODES:
            raise SIReaderException("Unsupported mode '%i'!" % mode)
        try:
            self._send_command(SIReader.C_SET_SYS_VAL, SIReader.O_MODE + int2byte(mode))
        finally:
            self._update_proto_config()
        self.beep()

    def set_station_code(self, code):
        """Set si station control code.
        @param code: control code (1-1023)
        """
        if code < 1 or code > 1023:
            raise SIReaderException("Invalid control code: '%i'! Supported code range: 1-1023." % code)
        # lower byte of control code
        code_low = int2byte(code & 0xFF)
        # high byte of control code, only first 2 bits are used, the rest is set to 1
        code_high = int2byte((code >> 2) | 0b00111111)
        try:
            self._send_command(SIReader.C_SET_SYS_VAL, SIReader.O_STATION_CODE + code_low + code_high)
        finally:
            self._update_proto_config()
        self.beep()

    def get_time(self):
        """Read out stations internal time.
        @return: datetime
        """
        bintime = self._send_command(SIReader.C_GET_TIME, b'')[1]
        year = byte2int(bintime[0])
        month = byte2int(bintime[1])
        day = byte2int(bintime[2])
        am_pm = byte2int(bintime[3]) & 0b1
        second = SIReader._to_int(bintime[4:6])
        hour = am_pm * 12 + second // 3600
        second %= 3600
        minute = second // 60
        second %= 60
        ms = int(round(byte2int(bintime[6]) / 256.0 * 1000000))
        self.beep()
        try:
            return datetime(year, month, day, hour, minute, second, ms)
        except ValueError:
            # return None if the time reported by the station is impossible
            return None

    def set_time(self, time):
        """Set si station internal time.
        @param time: time as a python datetime object.
        """
        bintime = (SIReader._to_str(int(time.strftime('%y')), 1)
                   + SIReader._to_str(time.month, 1)
                   + SIReader._to_str(time.day, 1)
                   + SIReader._to_str(((time.isoweekday() % 7) << 1) + time.hour//12, 1)
                   + SIReader._to_str((time.hour % 12)*3600 + time.minute*60 + time.second, 2)
                   + SIReader._to_str(int(round(time.microsecond / 1000000.0 * 256)), 1)
                   )

        self._send_command(SIReader.C_SET_TIME, bintime)
        self.beep()

    def beep(self, count = 1):
        """Beep and blink control station. This even works if now sicard is
        inserted into the station.
        @param count: Count of beeps
        """
        self._send_command(SIReader.C_BEEP, int2byte(count))

    def poweroff(self):
        """Switch off the control station and disconnect."""
        self._send_command(SIReader.C_OFF, b'')
        self.disconnect()

    def disconnect(self):
        """Close the serial port an disconnect from the station."""
        self._serial.close()

    def reconnect(self):
        """Close the serial port and reopen again."""
        self.disconnect()
        self._connect_reader(self._serial.port)

    def _connect_reader(self, port):
        """Connect to SI Reader.
        @param port: serial port
        """
        
        try:
            self._serial = Serial(port, baudrate = 38400, timeout = 5)
        except (SerialException, OSError):
            raise SIReaderException("Could not open port '%s'" % port)
        
        # flush possibly available input
        try:
            self._serial.flushInput()
        except (SerialException, OSError):
            # This happens if the serial port is not ready for
            # whatever reason (eg. there is no real device behind this device node). 
            raise SIReaderException("Could not flush port '%s'" % port)
        
        self.station_code = None

        try:
            # try at 38400 baud, extended protocol
            self._send_command(SIReader.C_SET_MS, SIReader.P_MS_DIRECT)
        except (SIReaderException, SIReaderTimeout):
            try:
                self._serial.baudrate = 4800
            except (SerialException, OSError) as msg:
                raise SIReaderException('Could not set port speed to 4800: %s' % msg)
            try:
                self._send_command(SIReader.C_SET_MS, SIReader.P_MS_DIRECT)
            except SIReaderException as msg:
                raise SIReaderException('This module only works with BSM7/8 stations: %s' % msg)

        self.port = port
        self.baudrate = self._serial.baudrate
        self._update_proto_config()

    def _update_proto_config(self):
        # Read protocol configuration
        ret = self._send_command(SIReader.C_GET_SYS_VAL, SIReader.O_PROTO+b'\x01')
        config_byte = byte2int(ret[1][1])
        self.proto_config = {}
        self.proto_config['ext_proto']  = config_byte & (1 << 0) != 0
        self.proto_config['auto_send']  = config_byte & (1 << 1) != 0
        self.proto_config['handshake']  = config_byte & (1 << 2) != 0
        self.proto_config['pw_access']  = config_byte & (1 << 4) != 0
        self.proto_config['punch_read'] = config_byte & (1 << 7) != 0

        # Read operating mode
        ret = self._send_command(SIReader.C_GET_SYS_VAL, SIReader.O_MODE+b'\x01')
        self.proto_config['mode'] = byte2int(ret[1][1])

        return self.proto_config
        
    def _set_proto_config(self, config):
        try:
            config_byte = int2byte((config['ext_proto'] << 0) |
                                   (config['auto_send'] << 1) |
                                   (config['handshake'] << 2) |
                                   (config['pw_access'] << 4) |
                                   (config['punch_read'] << 7))
            self._send_command(SIReader.C_SET_SYS_VAL, SIReader.O_PROTO + config_byte)
        finally:
            self._update_proto_config()

    def __del__(self):
        if self._serial is not None:
            self._serial.close()
        
    @staticmethod
    def _to_int(s):
        """Computes the integer value of a raw byte string."""
        value = 0
        for offset, c in enumerate(iterbytes(s[::-1])):
            value += c << offset*8
        return value

    @staticmethod
    def _to_str(i, len):
        """
        @param i:   Integer to convert into str
        @param len: Length of the return value. If i does not fit OverflowError is raised.
        @return:    string representation of i (MSB first)
        """
        if PY3:
            return i.to_bytes(len, 'big')
        if i >> len*8 != 0:
            raise OverflowError('%i too big to convert to %i bytes' % (i, len))
        string = ''
        for offset in range(len-1, -1, -1):
            string += int2byte((i >> offset*8) & 0xFF)
        return string

    @staticmethod
    def _crc(s):
        """Compute the crc checksum of value. This implementation is
        a reimplementation of the Java function in the SI Programmers
        manual examples."""

        def twochars(s):
            """generator that split a string into parts of two chars"""
            if len(s) == 0:
                # immediately stop on empty string
                raise StopIteration
            
            # add 0 to the string and make it even length
            if len(s)%2 == 0:
                s += b'\x00\x00'
            else:
                s += b'\x00'
            for i in range(0, len(s), 2):
                yield s[i:i+2]

        if len(s) < 1:
            # return value for 1 or no data byte is 0
            return b'\x00\x00'
        
        crc = SIReader._to_int(s[0:2])
        
        for c in twochars(s[2:]):
            val = SIReader._to_int(c)
            
            for j in range(16):
                if (crc & SIReader.CRC_BITF) != 0:
                    crc <<= 1
                
                    if (val & SIReader.CRC_BITF) != 0:
                        crc += 1 # rotate carry
                            
                    crc ^= SIReader.CRC_POLYNOM
                else:
                    crc <<= 1

                    if (val & SIReader.CRC_BITF) != 0:
                        crc += 1 # rotate carry
                        
                val <<= 1

        # truncate to 16 bit and convert to char
        crc &= 0xFFFF
        return int2byte(crc >> 8) + int2byte(crc & 0xFF)

    @staticmethod
    def _crc_check(s, crc):
        return SIReader._crc(s) == crc

    @staticmethod
    def _decode_cardnr(number):
        """Decodes a 4 byte cardnr to an int. SI-Card numbering is a bit odd:

           SI-Card 5:
              - byte 0:   always 0 (not stored on the card)
              - byte 1:   card series (stored on the card as CNS)
              - byte 2,3: card number
              - printed:  100'000*CNS + card number
              - nr range: 1-65'000 + 200'001-265'000 + 300'001-365'000 + 400'001-465'000

           SI-Card 6/6*/8/9/10/11/pCard/tCard/fCard/SIAC1:
              - byte 0:   card series (SI6:00, SI9:01, SI8:02, pCard:04, tCard:06, fCard:0E, SI10+SI11+SIAC1:0F)
              - byte 1-3: card number
              - printed:  only card number
              - nr range:
                  - SI6: 500'000-999'999 + 2'003'000-2'003'400 (WM2003) + 16'711'680-16'777'215 (SI6*)
                  - SI9: 1'000'000-1'999'999, SI8: 2'000'000-2'999'999
                  - pCard: 4'000'000-4'999'999, tCard: 6'000'000-6'999'999
                  - SI10: 7'000'000-7'999'999, SIAC1: 8'000'000-8'999'999
                  - SI11: 9'000'000-9'999'999, fCard: 14'000'000-14'999'999

           The card nr ranges guarantee that no ambigous values are possible
           (500'000 = 0x07A120 > 0x04FFFF = 465'535 = highest technically possible value on a SI5)
        """
        
        if number[0:1] != b'\x00':
            raise SIReaderException('Unknown card series')
        
        nr = SIReader._to_int(number[1:4])
        if nr < 500000:
            # SI5 card
            ret = SIReader._to_int(number[2:4])
            if byte2int(number[1]) < 2:
	        # Card series 0 and 1 do not have the 0/1 printed on the card
                return ret
            else:
                return byte2int(number[1])*100000 + ret
        else:
            # SI6/8/9
            return nr

    @staticmethod
    def _decode_time(raw_time, raw_ptd = None, reftime = None):
        """Decodes a raw time value read from an si card into a datetime object.
        The returned time is the nearest time matching the data before reftime."""

        if raw_time == SIReader.TIME_RESET:
            return None

        if reftime is None:
            # add two hours as a safety marging for cases where the
            # machine time runs a bit behind the stations time.
            reftime = datetime.now() + timedelta(hours=2)

        # get 12h punchtime
        punchtime = timedelta(seconds = SIReader._to_int(raw_time))

        # Documentation of the PTD byte from SportIdent
        # bit 0 - am/pm
        # bit 3...1 - day of week, 000 = Sunday, 110 = Saturday
        # bit 5...4 - week counter 0...3, relative
        # bit 7...6 - control station code number high
        # (...511)
        # week counter is not used!

        if raw_ptd is not None:
            ptd = byte2int(raw_ptd)

            # get info about AM(0) or PM(1)
            # and adjust punchtime in case of PM
            if (ptd & 0b00000001) == 0b1:
                punchtime = punchtime + timedelta(hours=12)

            # extract day of week and convert to Mon = 0, Tue = 1 ... as
            # datetime.weekday has Mon = 0, Sun = 6 the modulo operation
            # takes care of underflows (Sun = -1)
            dow = (((ptd & 0b00001110) >> 1) - 1) % 7

            # subtract a whole week if the dow week is the same but the punchtime
            # is later on the same day
            if (reftime.weekday() == dow
                and punchtime > timedelta(hours=reftime.hour, minutes=reftime.minute, seconds=reftime.second)):
                reftime -= timedelta(days=7)
            else:
                # adjust reftime according to weekday information
                reftime -= timedelta(days=(reftime.weekday()-dow) % 7)

            ref_day = reftime.replace(hour=0, minute=0, second=0, microsecond=0, tzinfo=None)
            return ref_day + punchtime

        # No PTD byte available, we have to rely on guessing the closest 12h time

        ref_day = reftime.replace(hour=0, minute=0, second=0, microsecond=0, tzinfo=None)
        ref_hour = reftime - ref_day
        t_noon = timedelta(hours=12)

        if ref_hour < t_noon:
            # reference time is before noon
            if punchtime < ref_hour:
                # t is between 00:00 and t_ref
                return ref_day + punchtime
            else:
                # t is afternoon the day before
                return ref_day - t_noon + punchtime
        else:
            # reference is after noon
            if punchtime < ref_hour - t_noon:
                # t is between noon and t_ref
                return ref_day + t_noon + punchtime
            else:
                # t is in the late morning
                return ref_day + punchtime

    @staticmethod
    def _append_punch(list, station,  timedata, ptd, reftime):
        time = SIReader._decode_time(timedata, ptd, reftime)
        if time is not None:
            list.append((station, time))

    @staticmethod
    def _decode_carddata(data, card_type, reftime = None):
        """Decodes a data record read from an SI Card."""

        ret = {}
        card = SIReader.CARD[card_type]
        
        # the slicing of data is necessary for Python 3 to get a bytes object instead
        # of an int
        ret['card_number'] = SIReader._decode_cardnr(b'\x00'
                                                     + data[card['CN2']:card['CN2']+1]
                                                     + data[card['CN1']:card['CN1']+1]
                                                     + data[card['CN0']:card['CN0']+1])

        ret['start'] = SIReader._decode_time(data[card['ST']:card['ST']+2],
                                             data[card['STD']] if card['STD'] else None,
                                             reftime)
        ret['finish'] = SIReader._decode_time(data[card['FT']:card['FT']+2],
                                              data[card['FTD']] if card['FTD'] is not None else None,
                                             reftime)
        ret['check'] = SIReader._decode_time(data[card['CT']:card['CT']+2],
                                             data[card['CTD']] if card['CTD'] is not None else None,
                                             reftime)
        if card['LT'] is not None:
            ret['clear'] = SIReader._decode_time(data[card['LT']:card['LT']+2],
                                                 data[card['LTD']] if card['LTD'] is not None else None,
                                                 reftime)
        else:
            ret['clear'] = None # SI 5 and 9 cards don't store the clear time

        punch_count = byte2int(data[card['RC']:card['RC']+1])
        if card_type == 'SI5':
            # RC is the index of the next punch on SI5
            punch_count -= 1
            
        if punch_count > card['PM']:
            punch_count = card['PM']
            
        ret['punches'] = []
        p = 0
        i = card['P1']
        while p < punch_count:
            if card_type == 'SI5' and i % 16 == 0:
                # first byte of each block is reserved for punches 31-36
                i += 1

            ptd = data[i + card['PTD']] if card['PTD'] is not None else None
            cn  = byte2int(data[i + card['CN']])
            pt  = data[i + card['PTH']:i + card['PTL']+1]

            SIReader._append_punch(ret['punches'], cn, pt, ptd, reftime)

            i += card['PL']
            p += 1
            
        return ret

    def _send_command(self, command, parameters):
        try:
            if self._serial.inWaiting() != 0:
                raise SIReaderException('Input buffer must be empty before sending command. Currently %s bytes in the input buffer.' % self._serial.inWaiting())
            command_string = command + int2byte(len(parameters)) + parameters
            crc = SIReader._crc(command_string)
            cmd = SIReader.STX + command_string + crc + SIReader.ETX
            if self._debug:
                print("==>> command '%s', parameters %s, crc %s" % (hexlify(command).decode('ascii'),
                                                                    ' '.join([hexlify(int2byte(c)).decode('ascii') for c in parameters]),
                                                                    hexlify(crc).decode('ascii'),
                                                                    ))
            self._serial.write(cmd)
        except (SerialException, OSError) as  msg:
            raise SIReaderException('Could not send command: %s' % msg)

        if self._logfile:
            self._logfile.write('s %s %s\n' % (datetime.now(), cmd))
            self._logfile.flush()
            os.fsync(self._logfile)
        return self._read_command()

    def _read_command(self, timeout = None):

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

            # Read command, length, data, crc, ETX
            cmd = self._serial.read()
            length = self._serial.read()
            station = self._serial.read(2)
            self.station_code = SIReader._to_int(station)
            data = self._serial.read(byte2int(length)-2)
            crc = self._serial.read(2)
            etx = self._serial.read()

            if self._debug:
                print("<<== command '%s', len %i, station %s, data %s, crc %s, etx %s" % (hexlify(cmd).decode('ascii'),
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

            if self._logfile:
                self._logfile.write('r %s %s\n' % (datetime.now(), char + cmd + length + station + data + crc + etx))
                self._logfile.flush()
                os.fsync(self._logfile)
                
        except (SerialException, OSError) as msg:
            raise SIReaderException('Error reading command: %s' % msg)

        return (cmd, data)

class SIReaderReadout(SIReader):
    """Class for 'classic' SI card readout. Reads out the whole card. If you don't know
    about other readout modes (control mode) you probably want this class."""

    def __init__(self, *args, **kwargs):
        super(type(self), self).__init__(*args, **kwargs)

        self.sicard = None
        self.cardtype = None

    def poll_sicard(self):
        """Polls for an SI-Card inserted or removed into the SI Station.
        Returns true on state changes and false otherwise. If other commands
        are received an Exception is raised."""

        if not self.proto_config['ext_proto']:
            raise SIReaderException('This command only supports stations in "Extended Protocol" '
                                    'mode. Switch mode first')

        if not self.proto_config['mode'] == SIReader.M_READOUT:
            raise SIReaderException("Station must be in 'Read SI cards' operating mode! Change operating mode first.")

        if self._serial.inWaiting() == 0:
            return False

        oldcard = self.sicard
        while self._serial.inWaiting() > 0:
            # _read_command does the actual parsing of the command
            # if it's an insert or remove event
            try:
                self._read_command(timeout = 0)
            except SIReaderCardChanged:
                pass
                    
        return not oldcard == self.sicard

    def read_sicard(self, reftime=None):
        """Reads out the SI Card currently inserted into the station. The card must be
        detected with poll_sicard before."""
            
        if not self.proto_config['ext_proto']:
            raise SIReaderException('This command only supports stations in "Extended Protocol" '
                                    'mode. Switch mode first')

        if not self.proto_config['mode'] == SIReader.M_READOUT:
            raise SIReaderException("Station must be in 'Read SI cards' operating mode! Change operating mode first.")

        if self.cardtype == 'SI5':
            raw_data = self._send_command(SIReader.C_GET_SI5,
                                          b'')[1]
        elif self.cardtype == 'SI6':
            raw_data  = self._send_command(SIReader.C_GET_SI6,
                                           SIReader.P_SI6_CB)[1][1:]
            raw_data += self._read_command()[1][1:]
            raw_data += self._read_command()[1][1:]
        elif self.cardtype in ('SI8', 'SI9'):
            raw_data = b''
            for b in range(SIReader.CARD[self.cardtype]['BC']):
                raw_data += self._send_command(SIReader.C_GET_SI9,
                                               int2byte(b))[1][1:]

        elif self.cardtype == 'SI10':
            # Reading out SI10 cards block by block proved to be unreliable and slow
            # Thus reading with C_GET_SI9 and block number 8 = P_SI6_CB like SI6
            # cards
            raw_data =  self._send_command(SIReader.C_GET_SI9,
                                           SIReader.P_SI6_CB)[1][1:]
            raw_data += self._read_command()[1][1:]
            raw_data += self._read_command()[1][1:]
            raw_data += self._read_command()[1][1:]
            raw_data += self._read_command()[1][1:]
        else:
            raise SIReaderException('No card in the device.')

        return SIReader._decode_carddata(raw_data, self.cardtype, reftime)
    
    def ack_sicard(self):
        """Sends an ACK signal to the SI Station. After receiving an ACK signal
        the station blinks and beeps to signal correct card readout."""
        try:
            self._serial.write(SIReader.ACK)
        except (SerialException, OSError) as msg:
            raise SIReaderException('Could not send ACK: %s' % msg)

    def _read_command(self, timeout=None):
        """Reads commands from the station. As a station in readout mode can send a
        card inserted or card removed event at any time we have to intercept these events
        here."""
        cmd, data = super(type(self), self)._read_command(timeout)

        # check if a card was inserted or removed
        if cmd == SIReader.C_SI_REM:
            self.sicard = None
            self.cardtype = None
            raise SIReaderCardChanged("SI-Card removed during command.")
        elif cmd == SIReader.C_SI5_DET:
            self.sicard = self._decode_cardnr(data)
            self.cardtype = 'SI5'
            raise SIReaderCardChanged("SI-Card inserted during command.")
        elif cmd == SIReader.C_SI6_DET:
            self.sicard = self._to_int(data)
            self.cardtype = 'SI6'
            raise SIReaderCardChanged("SI-Card inserted during command.")
        elif cmd == SIReader.C_SI9_DET:
            # SI 9 sends corrupt first byte (insignificant)
            self.sicard = self._to_int(data[1:])
            if self.sicard >= 2000000 and self.sicard <= 2999999:
                self.cardtype = 'SI8'
            elif self.sicard >= 1000000 and self.sicard <= 1999999:
                self.cardtype = 'SI9'
            elif self.sicard >= 7000000 and self.sicard <= 9999999:
                self.cardtype = 'SI10'
            else:
                raise SIReaderException('Unknown cardtype!')
            raise SIReaderCardChanged("SI-Card inserted during command.")

        return (cmd, data)

class SIReaderControl(SIReader):
    """Class for reading an SI Station configured as control in autosend mode."""

    def __init__(self, *args, **kwargs):
        super(type(self), self).__init__(*args, **kwargs)
        self._next_offset = None
        
    def poll_punch(self):
        """Polls for new punches.
        @return: list of (cardnr, punchtime) tuples, empty list if no new punches are available
        """

        if not self.proto_config['ext_proto']:
            raise SIReaderException('This command only supports stations in "Extended Protocol" '
                                    'mode. Switch mode first')

        if not self.proto_config['auto_send']:
            raise SIReaderException('This command only supports stations in "Autosend" '
                                    'mode. Switch mode first')

        punches = []
        while True:
            try:
                c = self._read_command(timeout = 0)
            except SIReaderTimeout:
                break 
        
            if c[0] == SIReader.C_TRANS_REC:
                cur_offset = SIReader._to_int(c[1][SIReader.T_OFFSET:SIReader.T_OFFSET+3])
                if self._next_offset is not None:
                    while self._next_offset < cur_offset:
                        # recover lost punches
                        punches.append(self._read_punch(self._next_offset))
                        self._next_offset += SIReader.REC_LEN

                self._next_offset = cur_offset + SIReader.REC_LEN
            punches.append( (self._decode_cardnr(c[1][SIReader.T_CN:SIReader.T_CN+4]), 
                             self._decode_time(c[1][SIReader.T_TIME:SIReader.T_TIME+2])) )
        else:
            raise SIReaderException('Unexpected command %s received' % hex(byte2int(c[0])))
        
        return punches
        
    def _read_punch(self, offset):
        """Reads a punch from the SI Stations backup memory.
        @param offset: Position in the backup memory to read
        @warning:      Only supports firmwares 5.55+ older firmwares have an incompatible record format!
        """
        c = self._send_command(SIReader.C_GET_BACKUP,
                               SIReader._to_str(offset, 3)+int2byte(SIReader.REC_LEN))
        return (self._decode_cardnr(b'\x00'+c[1][SIReader.BC_CN:SIReader.BC_CN+3]),
                self._decode_time(c[1][SIReader.BC_TIME:SIReader.BC_TIME+2]))

class SIReaderException(Exception):
    pass

class SIReaderTimeout(Exception):
    pass
        
class SIReaderCardChanged(Exception):
    pass
