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
    libicfp.GetData.restype = POINTER(c_ulong)
    libicfp.DoMove.argtypes = data_t, c_char
    libicfp.DoMove.restype = c_int

    return libicfp

