from ctypes import *
import os

def LoadLibrary():
    kyren = os.path.exists('libkyren.so')
    if kyren:
        libicfp = CDLL('./libkyren.so')
    else:
        libicfp = CDLL('./libicfp.so')

    data_t = POINTER(c_int)
    libicfp.Init.argtypes = (c_char_p,)
    libicfp.Init.restype = data_t
    libicfp.GetData.argtypes = data_t, POINTER(c_int), POINTER(c_int)
    libicfp.GetData.restype = POINTER(c_char)
    libicfp.DoMove.argtypes = data_t, c_char
    libicfp.DoMove.restype = c_int
    if kyren:
        libicfp.GetInfo.argtypes = data_t, POINTER(c_char), c_int, POINTER(c_int)
        libicfp.GetInfo.restype = None
        libicfp.GetSafeZone.argtypes = data_t,
        libicfp.GetSafeZone.restype = POINTER(c_char)
        libicfp.GoForIt.argtypes = data_t,
        libicfp.GoForIt.restype = None
        libicfp.GetBest.argtypes = ()
        libicfp.GetBest.restype = c_char_p

    return libicfp

