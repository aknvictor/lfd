import os
import subprocess as sub

p = sub.Popen(['echo \"#0P750 #1P2000 <CR>\" > \\\\.\\COM9'], stdout=sub.PIPE,stderr=sub.PIPE)

output, errors = p.communicate()
print(output)
