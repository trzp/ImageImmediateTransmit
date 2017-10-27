from VideoCapture import Device
import pygame,time
from pygame.locals import *
import socket

from I2Isocket import I2Isender

class Server():
    def __init__(self):
        pygame.init()
        self.cam = Device(0)
        self.scr = pygame.display.set_mode((640,480),0,24)
        self.sender = I2Isender()
        
    def start(self):
        tk = pygame.time.Clock()
        while True:
            pygame.event.clear()
            buf = self.cam.getImage().tostring()
            sur = pygame.image.fromstring(buf,(640,480), "RGB")
            self.scr.blit(sur.copy(),(0,0))
            self.sender.send(sur,('127.0.0.1',9000))
            pygame.display.flip()
            tk.tick(50)

if __name__ == '__main__':
    ga = Server()
    ga.start()