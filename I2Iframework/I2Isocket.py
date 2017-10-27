#!user/bin/python
# -*-coding:utf-8-*-

#FileName: jpeg.py
#Version: 1.0
#Author: Jingsheng Tang
#Date: 2017/11/22
#Email: mrtang@nudt.edu.cn
#Github: trzp

import threading
from jpeg import JPEG
import socket
import numpy as np
import pygame
from struct import pack,unpack


#format of a socket package: (one package is less than 64k)
#   package flag ('send'/'over'), image formate ('JPG'/'RGB'), 
#   image buffer length (4 bytes int32),image width (4 bytes int32),
#   image height (4 bytes int32), start index in the image buffer (4 bytes int32)
#   num of data bytes in this package (4 bytes int32) 

LEN_PACKAGE = 64*1024-27


class IMsock(JPEG):
    def __init__(self):
        JPEG.__init__(self)
        self.__rawdata = np.zeros(1080*720*3,dtype=np.uint8)
        self.obj = None

    def generate_packages(self,obj,quality=60):   #将原始字节数据压缩后分解为网络数据包
        if obj['format']=='RGB':    obj = self.jcompress(obj,quality)
        len = obj['data'].size  #获取长度
        st = obj['data'].tostring()
        tem = np.hstack((len,obj['size'])).astype(np.int32)
        bufs = []
        lst = range(0,len,LEN_PACKAGE)
        for i in lst:
            header = 'oooo'+obj['format']+tem.tostring()+pack('i',i)
            if len>=i+LEN_PACKAGE:   endp = i+LEN_PACKAGE
            else:   endp = len
            buf = header+pack('i',endp)+st[i:endp]
            bufs.append(buf)
        bufs[-1][:4] = 'over'+bufs[-1][]
        return bufs

    def merge_packages(self,buf):   #将得到的网络数据包合并后解压缩为原始图像数据
        size,w,h,start,end = np.fromstring(buf[7:27],dtype=np.int32)
        data = np.fromstring(buf[27:27+end-start],dtype=np.uint8)
        flag = buf[:4]
        format = buf[4:7]
        self.__rawdata[start:end]=data
        if flag=='over':
            te = self.__rawdata[:end]
            self.obj = self.jdecompress({'data':te,'format':format,'size':np.array([w,h])})
            return 1
        else:
            return 0

class I2Isender(threading.Thread):
    def __init__(self):
        self.__s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.__buf = []
        self.__addrs = []
        self.__ims = IMsock()
        self.__lock = threading.Lock()
        self.__im_buffer = ''
        threading.Thread.__init__(self)
        self.start()
        
    def send(self,im,addr,consistance_check=False):
        act = True
        buf = pygame.image.tostring(im,'RGB')
        if consistance_check:   #一致性检查
            if buf == self.__im_buffer: act = False

        if act:
            self.__im_buffer = buf
            data = np.fromstring(buf,dtype=np.uint8)
            self.__lock.acquire()
            bufs = self.__ims.generate_packages({'data':data,'format':'RGB','size':np.array(im.get_size())})
            self.__buf += bufs
            self.__addrs += [addr]*len(bufs)
            self.__lock.release()

    def run(self):
        while True:
            try:
                buf = self.__buf.pop(0)
                addr = self.__addrs.pop(0)
                self.__s.sendto(buf,addr)
            except:
                pass           

class I2Ireciever(threading.Thread):
    def __init__(self,bindaddr):
        self.__s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.__s.bind(bindaddr)
        self.__ims = IMsock()
        self.__sur = pygame.surface.Surface((100,100))
        threading.Thread.__init__(self)
        self.start()
    
    @property
    def image(self):
        return self.__sur

    def run(self):
        while True:
            buf,addr = self.__s.recvfrom(64*1024)
            if self.__ims.merge_packages(buf):
                self.__sur = pygame.image.frombuffer(self.__ims.obj['data'],self.__ims.obj['size'],self.__ims.obj['format']).convert()
            
                
                