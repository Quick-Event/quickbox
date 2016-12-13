import subprocess
import time

if __name__ == "__main__":
	while True:
		ret = subprocess.call(['python', 'sipunchselect.py'])
		#print "Exit code: ", ret
		time.sleep(3)