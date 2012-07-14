#!/usr/bin/python

import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), '../jeroen'))

import libloader

libicfp = libloader.LoadLibrary()

def doverify(mapfn, commands):
    d = libicfp.Init(mapfn)
    for c in commands:
        score = libicfp.DoMove(d, c)

    print score

def main():
    mapfn, commands = sys.argv[1:]
    doverify(mapfn, commands)

if __name__ == '__main__':
    main()
