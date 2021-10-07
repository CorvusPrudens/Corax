
import re

instr = re.compile(r'[ \t]*[a-z]+ ([a-z0-9\[\]]+,? *)*')
data = []
with open("conditionals.enc", "r") as file:
  for line in file:
    data.append(line)

from random import choice

conditions = ['zero', 'carry', 'negative', 'equal', 'greater', 'less']

with open("cond.enc", 'w') as file:
  for line in data:
    if instr.search(line) is not None:
      line = line[:-1] + f' -> {choice(conditions)}\n'
    file.write(line)
