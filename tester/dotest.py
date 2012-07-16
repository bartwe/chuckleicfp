#!/usr/bin/python

import subprocess, time, os

MAIN = '../kyren/main'
#MAIN = '../bartwe/visualizer/lifter'
MAPDIR = '../maps'
TIME = 90
totalScore = 0

def DoTest(exe, mapfn):
    global totalScore
    #print 'Testing with map %s' % mapfn
    with open(mapfn, 'r') as fd:
        mapdata = fd.read()
    s = time.time()
    p = subprocess.Popen((exe,), stdin=open(mapfn), stdout=subprocess.PIPE)
    killer = subprocess.Popen("sleep %d; kill -INT %d" % (TIME, p.pid), shell=True)
    out = ""
    while True:
        r = p.stdout.read()
        if not r: break
        out += r
    p.wait()
    e = time.time()

    score = -99999
    for line in out.split('\n'):
        if 'score' in line:
            score = int(line.strip().split()[-1])
    solution =  ""
    for c in out:
        if c in "LRUDAWS":
            solution += c

    print '%30s %6d %3d %d %r' % (os.path.basename(mapfn), score, int(e-s), p.returncode, solution)
    totalScore += score;

    os.kill(killer.pid, 9)

def main():
    maps = os.listdir(MAPDIR)
    print 'MAPNAME SCORE RUNTIME EXITCODE SOLUTION'
    for map in sorted(maps):
        DoTest(MAIN, os.path.join(MAPDIR, map))
    
    print "Total: %s" % totalScore

if __name__ == '__main__':
    main()
