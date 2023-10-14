import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

PIN_DATA  = 22
PIN_LATCH = 27
PIN_CLOCK = 23

GPIO.setup(PIN_DATA,  GPIO.IN)
GPIO.setup(PIN_LATCH, GPIO.OUT)
GPIO.setup(PIN_CLOCK, GPIO.OUT)

# pload  = sh = 8 (GPIO27)
# clock  = clk = 12 (GPIO23)
# data   = qh  = 11 (GPIO22)
# clk en = gnd = 9

def shiftin(count=1):
  result = []

  GPIO.output(PIN_CLOCK, 0)
  GPIO.output(PIN_LATCH, 0)
  GPIO.output(PIN_LATCH, 1)

  for by in range(count):
    for bi in range(8):
      result.append(GPIO.input(PIN_DATA))

      GPIO.output(PIN_CLOCK, 1)
      GPIO.output(PIN_CLOCK, 0)

  return result

try:
  print "STARTED!"

  while(1):
    r = shiftin(3)
    #print(r)
    time.sleep(0.017)

except KeyboardInterrupt:
  GPIO.cleanup
GPIO.cleanup
