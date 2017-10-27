from VideoCapture import Device
import pygame,time
from pygame.locals import *
import socket

from I2Isocket import I2Isender,I2Ireciever


class Client():
    def __init__(self):
        pygame.init()
        self.scr = pygame.display.set_mode((640,480),0,24)
        self.rec = I2Ireciever(('192.168.43.208',9000))
        
    def start(self):
        tk = pygame.time.Clock()
        while True:
            pygame.event.clear()
            self.scr.blit(self.rec.image,(0,0))
            pygame.display.flip()
            tk.tick(50)

if __name__ == '__main__':
    ga = Client()
    ga.start()