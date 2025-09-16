import subprocess
import re
import serial
import time

# Serial port to ESP8266
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

# Exact ANSI sequences
SED_RED_YELLOW_PATTERN = re.compile(r'\x1b\[1;31;103m.*?\x1b\[0m')  # RED/YELLOW
SED_RED_PATTERN = re.compile(r'\x1b\[1;31m.*?\x1b\[0m')              # RED only

# Run linpeas.sh
process = subprocess.Popen(
    ["bash", "/home/buuf/Desktop/githubshit/linpeas/linpeas.sh"],
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True,
    bufsize=1
)

def send_to_esp(text, color_flag):
    """Send message to ESP8266 over serial (color_flag=1 for RED_YELLOW, 0 for RED)."""
    # truncate if too long (max 100)
    text = text.strip()
    if len(text) > 40:  # leave space for safety
        text = text[:40]
    length_byte = len(text)
    ser.write(bytes([length_byte, color_flag]))
    ser.write(text.encode())
    ser.flush()

# Process output in real-time
for line in process.stdout:
    if SED_RED_YELLOW_PATTERN.search(line) and 'RED/YELLOW' not in line:
        send_to_esp(line, 1)
        print("Sent RED/YELLOW:", line.strip())
        time.sleep(0.1)  # slight delay to avoid overwhelming the ESP
    elif SED_RED_PATTERN.search(line) and 'RED' not in line:
        send_to_esp(line, 0)
        print("Sent RED:", line.strip())
        time.sleep(0.1)  # slight delay to avoid overwhelming the ESP
