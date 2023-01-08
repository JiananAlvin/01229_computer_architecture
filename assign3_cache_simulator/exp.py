# script for running experiment.
import os

num_of_sets = ['1', '4', '8', '16', '64', '128', '256']
num_of_blocks = ['1', '2', '4', '8', '16', '64', '1024']
size_of_block = ['4', '16', '64', '128']
write_strategy = ['write-allocate write-back', 'no-write-allocate write-through', 'write-allocate write-through']
evict_strategy = ['lru', 'fifo']
cmd = ''

for s in num_of_sets:
  for b in num_of_blocks:
    for z in size_of_block:
      for w in write_strategy:
        for e in evict_strategy:
          cmd = './csim' + ' ' + s + ' ' + b + ' ' + z + ' ' + w + ' ' + e + ' ' + '< gcc.trace'
          os.system(cmd)





