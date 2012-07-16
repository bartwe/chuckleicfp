#!/usr/bin/python

import os

outfiles = sorted(fn for fn in os.listdir('data') if fn.endswith('.out'))

map2data = {}

alllabels = set()

for outfile in outfiles:
    with open('data/'+outfile) as fd:
        for line in fd:
            if '.value' not in line: continue

            name, score = line.strip().split()
            #score = int(score)
            label, _ = name.split('.')
            if '-' not in label: continue
            map, impl = label.split('-')
            #print impl, map, score

            data = map2data.setdefault(map, {})
            key = os.path.basename(outfile)[:-4] + '-' + impl
            alllabels.add(key)
            data[key] = score

labs = sorted(alllabels)
print ','.join(['map'] + labs)
for k, v in sorted(map2data.items()):
    print ','.join([k] + [v.get(lab, '') for lab in labs])




