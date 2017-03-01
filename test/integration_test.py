from os import system
from threading import Thread
import socket
import time
import sys
import subprocess


def client_thread(args):
	TCP_IP = '127.0.0.1'
	TCP_PORT = 2020
	BUFFER_SIZE = 1024
	MESSAGE = "GET /echo HTTP/1.1\r\n\r\n"

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.connect(('localhost', TCP_PORT))
	s.send(MESSAGE)
	data = s.recv(BUFFER_SIZE)
	s.close()

	if (data != "HTTP/1.1 200 OK\r\nContent-Length: 18\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\n\r\n"):
	    	print "Failure response: ", repr(data)
                error_code[0] = 1
		return

	error_code[0] = 0

if __name__ == "__main__":
	print "\n-----Opening server-----"
	p = subprocess.Popen("./web-server simple_config", shell=True)

	time.sleep(4)

	error_code = [0]
	thread_client = Thread(target = client_thread, args=(error_code))
	thread_client.start()
	thread_client.join()

	print "\n\n-----Closing the server/Doing cleanup-----"
	system("fuser -k 2020/tcp")

	time.sleep(2)

	if (error_code[0] == 1):
		print "\n\nTEST FAILURE\n"
		exit(1)
	else:
		print "\n\nTEST SUCCESS\n"
		exit(0)

