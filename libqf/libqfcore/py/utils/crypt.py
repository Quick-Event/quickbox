#!/usr/bin/python
# -*- coding: utf-8 -*-
'''doc string'''

import sys, re
import datetime
import random

class Crypt:

	@staticmethod # not inherited http://rapd.wordpress.com/2008/07/02/python-staticmethod-vs-classmethod/
	def _to_uint32(n):
		return n % 0x100000000

	@staticmethod
	def _generator_for_params(params):
		to_uint32 = Crypt._to_uint32
		def gen(val):
			n = to_uint32(to_uint32(params.a*val) + params.c) % params.max_rnd;
			return to_uint32(n);
		return gen

	@staticmethod # http://www.geocities.com/foetsch/python/new_style_classes.htm
	def _defaultGenerator(val):
		to_uint32 = Crypt._to_uint32
		a = 213244
		c = 1;
		max_rnd = 1048613;
		n = to_uint32(to_uint32(a*val) + c) % max_rnd;
		return to_uint32(n);

	@staticmethod
	def _codeByte(b):
		ret = ''
		# hodnoty, ktere nejsou pismena se ukladaji jako cislo
		# format cisla je 4 bity cislo % 10 [0-9] + 4 bity cislo / 10 [A-Z]
		c = chr(b)
		if (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z'):
			ret += c
		else:
			ret += chr((b % 10) + ord('0'))
			ret += chr((b / 10) + (ord('A') if (b % 2) else ord('a')))
		#sys.stderr.write("_codeByte: %s -> %s\n" % (b, ret))
		return ret;

	def __init__(self, generator = None, generator_params = None):
		if generator:
			self.generator = generator
		elif generator_params:
			self.generator = Crypt._generator_for_params(generator_params)
		else:
			self.generator = Crypt._defaultGenerator

	def crypt(self, s, min_length = 16):
		#sys.stderr.write("crypting: %s\n" % s)
		if type(s) == unicode: src = s.encode('utf-8')
		else: src = str(s)
		val = random.randint(0, 0xFFFF)
		val += datetime.datetime.now().time().microsecond
		val = val % 256
		if val == 0: val = 1
		b = val
		dest = Crypt._codeByte(b)
		for c in src:
			b = ord(c)
			if b == 0:
				break
			val = self.generator(val);
			b = b ^ (val % 256)
			dest += Crypt._codeByte(b)
		bb = 0
		while len(dest) < min_length:
			val = self.generator(val);
			b = bb ^ (val % 256)
			dest += Crypt._codeByte(b)
			bb = random.randint(0, 0xFFFF) % 256
		return dest

	@staticmethod
	def _getByte(arr, pos):
		c = arr[pos]
		pos += 1
		b = ord(c)
		if (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z'):
			pass
		else:
			b1 = b - ord('0')
			b = b1
			c = arr[pos]
			pos += 1
			b1 = 10 * (ord(c) - (ord('A') if (b1 % 2) else ord('a')))
			b += b1
		return (b % 256, pos)


	def decrypt(self, s):
		# remove all whitespaces
		s = re.sub(r'\s', '', s)
		#sys.stderr.write("decrypting: %s\n" % s)
		ret = ''
		pos = 0
		if len(s) > 0:
			(b, pos) = Crypt._getByte(s, pos)
			val = b;
			#print hex(b), b, chr(b)
			while pos < len(s):
				val = self.generator(val)
				(b, pos) = Crypt._getByte(s, pos)
				b = b ^ (val % 256)
				c = chr(b)
				#print hex(b), b, c
				ret += c
		#odstran \0 na konci, byly tam asi umele pridane
		pos = 0
		while pos < len(ret):
			if ord(ret[pos]) == 0:
				break
			pos += 1
		ret = ret[0:pos]
		return ret

#===========================================================
if __name__ == '__main__':
	help_msg = '''crypt.py.

USAGE: crypt.py [options] [string | - ]

- is for stdin

OPTIONS:
	-c crypt [default]
	-d decrypt
	--generator generator_name

EXAMPLES:
	./crypt.py -c - < ~/t/1.txt | fold -w40

'''
	o_direction = 'c'
	o_generator = ''
	src = ''
	args = sys.argv[1:]
	pos = 0;
	while pos < len(args):
		arg = args[pos]
		if arg[:2] == '--':
			arg = arg[2:]
			if arg == 'generator':
				pos += 1;
				o_generator = args[pos]
		elif arg[:1] == '-':
			arg = arg[1:]
			if arg == 'd':
				o_direction = 'd'
			elif not arg:
				src = '-'
		else: src = arg
		pos += 1;

	if not src:
		print help_msg
		sys.exit(0)

	generator_coefs = None
	if o_generator:
		coefs = o_generator.split(',')
		if not coefs:
			coefs = o_generator.split(';')
		if coefs:
			generator_coefs = {a:coefs[0], c:coefs[1], max_rnd:coefs[2]}

	if src == '-':
		src = sys.stdin.read()

	c = Crypt(generator_params=generator_coefs)
	if o_direction == 'd':
		print c.decrypt(src)
	else:
		print c.crypt(src)

