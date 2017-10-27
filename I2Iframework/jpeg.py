#!user/bin/python
# -*-coding:utf-8-*-

#FileName: jpeg.py
#Version: 1.0
#Author: Jingsheng Tang
#Date: 2017/11/22
#Email: mrtang@nudt.edu.cn
#Github: trzp

from ctypes import *
import numpy as np
import os
rootdir = os.path.split(__file__)[0]
dlldir = rootdir + '\\im_compress.dll'

class FormatErr(Exception):
    def __str__(self):
        return "unsupported iamge format!"

class JPEG:
    def __init__(self):
        self.__imc = CDLL(dlldir)
        self.__jc = self.__imc.compress
        self.__jdc = self.__imc.decompress
    
    def jcompress(self,raw_pixels_obj,quality=60):
        if raw_pixels_obj['format'].upper()!='RGB': raise FormatErr
        
        len = c_int(0)
        w = raw_pixels_obj['size'][0]
        h = raw_pixels_obj['size'][1]
        pixels = raw_pixels_obj['data']
        
        r = np.zeros(w*h*3,dtype=np.uint8)
        self.__jc(pixels.ctypes.data,w,h,r.ctypes.data,byref(len),quality)
        res = {'data':r[:len.value],'size':raw_pixels_obj['size'],'format':'JPG'}
        return res
        
    def jdecompress(self,jpgstream):
        if jpgstream['format']!='JPG':  raise FormatErr
        w = jpgstream['size'][0]
        h = jpgstream['size'][1]
        r = np.zeros(w*h*3,dtype=np.uint8)
        self.__jdc(jpgstream['data'].ctypes.data,jpgstream['data'].size,r.ctypes.data,w,h)
        return {'data':r,'size':jpgstream['size'],'format':'RGB'}


