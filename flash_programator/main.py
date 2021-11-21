import serial
import time

from serial.serialutil import EIGHTBITS, PARITY_EVEN, STOPBITS_ONE

serial_port = serial.Serial('COM7', parity=PARITY_EVEN, baudrate=9600, bytesize=EIGHTBITS, stopbits=STOPBITS_ONE)

song_file = open("Police Siren Cut 44100 Mono.raw", "rb")

i = 1
song_data = song_file.read(528)
total_blocks = (song_file.__sizeof__()) / 528

while (song_data != b""):
    serial_port.write(song_data)
    answer = serial_port.read(1)

    if (answer == b'\xBD'):
        print("Blocks transfered: " + str(i) + "/" + str(total_blocks))
        song_data = song_file.read(528)
        i += 1
    else:
        print("ERROR")

serial_port.close()
song_file.close()