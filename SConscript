import os
from building import *

src = []
group = []

cwd = GetCurrentDir()
src = Split("""
rt_tlsf.c
src/tlsf.c
""")
CPPPATH = [cwd + '/src']

group = DefineGroup('tlsf', src, depend = [''], CPPPATH = CPPPATH)

Return('group')
