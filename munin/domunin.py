#!/usr/bin/python

import subprocess, time, os, sys

PACKAGES = [
        #['../bartwe/visualizer/', 'lifter', 'dasa', 'cd ../bartwe/visualizer; make'],
        ['../kyren/', 'main', 'kyren', 'cd ../kyren/; make'] ]

# ['../lightning/', 'lifter', 'lightning', 'cd ../lightning; ./install'], 
# , ['../kyren_chunk/', 'main', 'chunk', 'cd ../kyren_chunk/; make'] ]
MAPDIR = '../maps'

def mapname(map):
    return os.path.basename(map).replace('.map', '')

def DoTest(cwdpath, exe, shortname, mapfn):
    #print 'Testing with map %s' % mapfn
    with open(mapfn, 'r') as fd:
        mapdata = fd.read()
    s = time.time()
    exepath = os.path.abspath(cwdpath+exe)
    fullcwdpath = os.path.abspath(cwdpath)
    print >> sys.stderr, 'Running ' + exepath + ' map ' + mapfn
    p = subprocess.Popen((exepath,), cwd=fullcwdpath, stdin=open(mapfn), stdout=subprocess.PIPE)
    killer = subprocess.Popen("sleep 45; kill -INT %d; sleep 10; kill -9 %d" % (p.pid, p.pid), shell=True)
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
    if score < 0:
        score = -1
    solution =  ""
    print >> sys.stderr, out
    for c in out:
        if c in "LRUDAW":
            solution += c
        
    print '%s-%s.value %d' % (mapname(mapfn), shortname, score)
    

    os.kill(killer.pid, 9)
    if score < 0:
        score = 0;
    return score

def gitpull():
    p = subprocess.Popen('cd ../; git pull', shell=True, stdout=open('/dev/null', 'w'), stderr=subprocess.STDOUT)
    p.wait();
    
def main():
#    gitpull();
    maps = os.listdir(MAPDIR)
    config = 'config' in sys.argv
    test = 'test' in sys.argv
    if 'map' in sys.argv:
        maps = [ '../maps/horock3.map' ]
    if config:
        print 'graph_title Progress'
        print 'graph_vlabel score'
        print 'graph_category icfp'
    else:
        for main in PACKAGES:
            if main[3]:
                p = subprocess.Popen(main[3], shell=True, stderr=subprocess.STDOUT)
                p.wait();
                assert p.returncode == 0
    first = True;
    sums = {}
    for map in sorted(maps):
        if test and not first:
            break
        sum = 0;
        first = False
        for main in PACKAGES:
            if config:
                print '%s-%s.label %s %s' % (mapname(map), main[2], mapname(map), main[2])
            else:
                sum = sums.get(main[2], 0)
                sum += DoTest(main[0], main[1], main[2], os.path.join(MAPDIR, map))
                sums[main[2]] = sum;
    for main in PACKAGES:
        if config:
            print '%s.label %s' %(main[2], main[2])
        else:
            print '%s.value %d' %(main[2], sums[main[2]])

if __name__ == '__main__':
    main()


