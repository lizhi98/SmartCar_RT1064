import sensor
import time
from machine import UART
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
clock = time.clock()

# 初始化外设
uart = UART(2, baudrate=115200)     # 初始化串口 波特率设置为115200

while True:
    clock.tick()
    img = sensor.snapshot()


    #print(clock.fps())
