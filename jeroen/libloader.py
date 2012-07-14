from ctypes import *
import os

def LoadLibrary():
    kyren = os.path.exists('libkyren.so')
    if kyren:
        libicfp = CDLL('./libicfp.so')
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
        libicfp.GetSafeZone.argtypes = data_t,
        libicfp.GetSafeZone.restype = POINTER(c_char)

    return libicfp

