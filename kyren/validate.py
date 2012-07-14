#!/usr/bin/python

from __future__ import division

import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), '../jeroen'))
sys.path.append(os.path.join(os.path.dirname(__file__), '../tester'))

import libloader
import dotest

libicfp = libloader.LoadLibrary()

def doverify(mapfn, commands):
    d = libicfp.Init(mapfn)
    score = 0
    for c in commands:
        score = libicfp.DoMove(d, c)

    return score

def main():
    args = sys.argv[1:]
    if len(args) == 2:
        mapfn, commands = sys.argv[1:]
        print doverify(mapfn, commands)
    else:
        # assume this is dotest output
        outfile, = args

        contestscores = []
        with open(outfile) as fd:
            for line in fd:
                elems = line.strip().split()
                if 'SCORE' in elems: continue

                #print elems
                mapname, score, runtime, exitcode, solution = elems
                assert solution[0] == solution[-1]
                assert solution[0] in "'\""
                solution = solution[1:-1]
                score = int(score)
                verifiedscore = doverify(os.path.join(dotest.MAPDIR, mapname), solution)
                if verifiedscore != score:
                    print "Map %s: reported score %r, verified %r" % (mapname, score, verifiedscore)
                if mapname.startswith('cont'):
                    contestscores.append(verifiedscore)

        cs = contestscores
        print "%d contest scores, sum=%d, avg=%.1f" % (len(cs), sum(cs), sum(cs)/len(cs))

if __name__ == '__main__':
    main()
