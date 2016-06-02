import serial
import time
import os
import subprocess
import videofunctions
from kaleidoscope_db_client import KaleidoscopeDBClient, User
#from msvcrt import getch
import msvcrt

import signal,sys

def catch_ctrl_C(sig,frame):
    exit()#pass

signal.signal(signal.SIGINT, catch_ctrl_C)

# Create an object Serial with ttymxc3 port
ser = serial.Serial('COM7', 9600, timeout=20)
ser.flushInput()
inputs=""

videoFolderPath = "C:/Users/wshong/Desktop/deleteme/"
vlcPath = "C:/Program Files (x86)/VideoLAN/VLC/vlc.exe"
projectorOnDuration = 0 #0 is epson, 20 is crappy projector
projectorOffDuration = 2

uid = ''
name = ''
done1 = False
done2 = False
done3 = False
tries1 = 0
tries2 = 0
tries3 = 0
pvideo1 = 'intro.mov'
pvideo2 = 'blablabla.mov'
pvideo3 = 'denied.mp4'
pvideo4 = 'optional.mov'

playerPacketSize = 36
checkin_tries_byte = 20
speech_tries_byte = 21
light_tries_byte = 22

playerVideoPath = ""


'''
def sendSerialData(serialData):
    ser.write(serialData)
'''
def getName():
	ser.flushInput()
	sys.stdout.flush()
	# Try to flush the buffer
	while msvcrt.kbhit():
		msvcrt.getch()
	key = ''
	name = ''
	while (key != '\r'):	
		key = msvcrt.getch() #ord(getch())
		if key == '\b':
			name = name[:-1]
		else:
			name += key
		print(name)
		ser.write(key)
	return str(name)	

def queryEntry(uid):
    #if uid doesn't exist, return newplayer
	with KaleidoscopeDBClient('kaleidoscope_users.db') as client:
		user = client.find_by_uid(uid)
	  
		if user:
			return "Found"
		else:
			return "None"
		
def newPlayer(uid):
	# tell arduino to advance to Enter Name state
	ser.write(b"Y")
	
	with KaleidoscopeDBClient('kaleidoscope_users.db') as client:
		# getKey
		newUid = uid
		newPlayerName = getName()
		print(newPlayerName)

		# insertEntry(uid)
		user = User(uid=newUid, name=newPlayerName, done1=True, tries1=1,\
			video1=pvideo1, video2=pvideo2, video3=pvideo3)
		client.save(user)
		
		# queryEntry(uin) # get video to play
		video1 = str(videoFolderPath + user.video1)
		
		print('waiting for play video state')
		inPlayVideoState = ser.readline().rstrip() 
		if inPlayVideoState == 'PLAY_VIDEO':
			playVideo(video1)
	
def oldPlayer(uid):
	# tell arduino to advance to Play Video state
	ser.write(b"N")
	
	# updateEntry() # update database info
	with KaleidoscopeDBClient('kaleidoscope_users.db') as client:
		user = client.find_by_uid(uid)
		user.done1 = done1
		user.done2 = done2
		user.done3 = done3
		user.tries1 = tries1
		user.tries2 = tries2
		user.tries3 = tries3
		
	# queryEntry() # query database, set video to play
		video1 = str(videoFolderPath + user.video1)
		video2 = str(videoFolderPath + user.video2)
		video3 = str(videoFolderPath + user.video3)
		if user.video4:
			video4 = str(videoFolderPath + user.video4)
		else:
			video4 = ''
		
		print('waiting for play video state')
		inPlayVideoState = ser.readline().rstrip() 
		if inPlayVideoState == 'PLAY_VIDEO':

			# temp set video path	
			if user.alldone:
				if video4:
					playVideo(video4)
			elif user.tries3 and user.tries2 and user.tries1:
				if video4:
					playVideo(video3, ignoreProjOff=True)
					playVideo(video4, ignoreProjOn=True)
				else:
					playVideo(video3)
				user.alldone = True	
			  
			elif tries2 and tries1:
				playVideo(video2)
			else:
				pass
	
			# If user is Done, tell arduino to advance back to Check In state
			ser.write(b"D")
		
	
def playVideo(videoPath, ignoreProjOn=False, ignoreProjOff=False):
	if not projectorOnDuration:
		time.sleep(5) # For Epson, allow some time before start video
	if not ignoreProjOn:
		ser.write(b"O")
		time.sleep(projectorOnDuration)
	videoDuration = videofunctions.duration(videoPath)
	p = subprocess.Popen([vlcPath, "file:///"+videoPath])
	time.sleep(videoDuration)
	#p.terminate()
	#p.kill()
	if not ignoreProjOff:
		ser.write(b"F")
		time.sleep(projectorOffDuration)
	p.terminate()
	p.kill()	
		

while 1:

	# Wait for UID
	#uid = ser.readline().rstrip()
	print('waiting for packet')
	playerPacket = ser.readline().rstrip()
	#playerPacket = ser.read(playerPacketSize)
	if playerPacket:
		print(playerPacket)
		import binascii
		#binascii.hexlify(bytearray(playerPacket))
		
		#uid = playerPacket[:4]
		uid = binascii.b2a_hex(playerPacket[:4])
		name = binascii.b2a_uu(playerPacket[4:20])
		tries1 = ord(playerPacket[checkin_tries_byte])
		tries2 = ord(playerPacket[speech_tries_byte])
		tries3 = ord(playerPacket[light_tries_byte])
		
		print(uid)	
		print(name)
		print(tries1)
		
		# Check for new player
		if queryEntry(uid) == "None":
			print("new playerrrr") #debug
			newPlayer(uid)

			
		else:
			print("old playerrrr") #debug
			oldPlayer(uid)
		
		time.sleep(1)


ser.close()
