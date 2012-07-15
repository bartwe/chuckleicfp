#!/usr/bin/python

import subprocess, time, os, sys

PACKAGES = [ ['../bartwe/visualizer/lifter', 'dasa', 'cd ../bartwe/visualizer; make'], ['../kyren/main', 'brute', ''] ]
MAPDIR = '../maps'

def mapname(map):
    return os.path.basename(map).replace('.map', '')

def DoTest(exe, shortname, mapfn):
    #print 'Testing with map %s' % mapfn
    with open(mapfn, 'r') as fd:
        mapdata = fd.read()
    s = time.time()
    p = subprocess.Popen((exe,), stdin=open(mapfn), stdout=subprocess.PIPE)
    killer = subprocess.Popen("sleep 150; kill -INT %d" % p.pid, shell=True)
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
        if c in "LRUDAW":
            solution += c
        
    print '%s-%s.value %d' % (mapname(mapfn), shortname, score)

    os.kill(killer.pid, 9)
    return score

def gitpull():
    p = subprocess.Popen('cd ../; git pull', shell=True, stdout=open('/dev/null', 'w'), stderr=subprocess.STDOUT)
    p.wait();
    
def main():
#    gitpull();
    maps = os.listdir(MAPDIR)
    config = 'config' in sys.argv
    if config:
        print 'graph_title Progress'
        print 'graph_vlabel score'
        print 'graph_category icfp'
    for main in PACKAGES:
        if main[2]:
            p = subprocess.Popen(main[2], shell=True, stdout=open('/dev/null', 'w'), stderr=subprocess.STDOUT)
            p.wait();
        sum = 0;
        for map in sorted(maps):
            if config:
                print '%s-%s.label %s %s' % (mapname(map), main[1], mapname(map), main[1])
            else:
                sum += DoTest(main[0], main[1], os.path.join(MAPDIR, map))
        if config:
            print '%s.label %s' %(main[1], main[1])
        else:
            print '%s.value %d' %(main[1], sum)

if __name__ == '__main__':
    main()


