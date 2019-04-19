#!/bin/python

import fnmatch
import os
import shutil
import subprocess
import sys


line_nb = False

for arg in sys.argv[1:]:
    if (arg == "--with-line-nb"):
        print("Enabling line numbers in the context locations.")
        line_nb = True
    else:
        os.sys.exit("Non supported argument '" + arg + "'. Aborting.")


if (not os.path.exists("src")):
    os.sys.exit("ERROR: This script should be started from the root of the git repo.")


matches = []
for root, dirnames, filenames in os.walk('src'):
    for filename in fnmatch.filter(filenames, '*.cpp'):
        matches.append(os.path.join(root, filename))
    for filename in fnmatch.filter(filenames, '*.h'):
        matches.append(os.path.join(root, filename))
matches.sort()

unique_str = []
unique_loc = {}
main_po = """
# LANGUAGE translation of the GDProcMesh
#
msgid ""
msgstr ""
"Project-Id-Version: GDProcMesh\\n"
"Content-Type: text/plain; charset=UTF-8\\n"
"Content-Transfer-Encoding: 8-bit\\n"
"""

def process_file(f, fname):

    global main_po, unique_str, unique_loc

    l = f.readline()
    lc = 1
    while (l):

        patterns = ['RTR_LOCAL(\"', 'TTR(\"','TTRC(\"']
        idx = 0
        pos = 0
        while (pos >= 0):
            pos = l.find(patterns[idx], pos)
            if (pos == -1):
                if (idx < len(patterns) - 1):
                    idx += 1
                    pos = 0
                continue
            pos += 5

            msg = ""
            while (pos < len(l) and (l[pos] != '"' or l[pos - 1] == '\\')):
                msg += l[pos]
                pos += 1

            location = os.path.relpath(fname).replace('\\', '/')
            if (line_nb):
                location += ":" + str(lc)

            if (not msg in unique_str):
                main_po += "\n#: " + location + "\n"
                main_po += 'msgid "' + msg + '"\n'
                main_po += 'msgstr ""\n'
                unique_str.append(msg)
                unique_loc[msg] = [location]
            elif (not location in unique_loc[msg]):
                # Add additional location to previous occurrence too
                msg_pos = main_po.find('\nmsgid "' + msg + '"')
                if (msg_pos == -1):
                    print("Someone apparently thought writing Python was as easy as GDScript. Ping Akien.")
                main_po = main_po[:msg_pos] + ' ' + location + main_po[msg_pos:]
                unique_loc[msg].append(location)

        l = f.readline()
        lc += 1

print("Updating the gdprocmesh.pot template...")

for fname in matches:
    with open(fname, "rb") as f:
        process_file(f, fname)

with open("gdprocmesh.pot", "wb") as f:
    f.write(main_po)

if (os.name == "posix"):
    print("Wrapping template at 79 characters for compatibility with Weblate.")
    os.system("msgmerge -w79 gdprocmesh.pot gdprocmesh.pot > gdprocmesh.pot.wrap")
    shutil.move("gdprocmesh.pot.wrap", "gdprocmesh.pot")

shutil.move("gdprocmesh.pot", "translations/gdprocmesh.pot")

# TODO: Make that in a portable way, if we care; if not, kudos to Unix users
if (os.name == "posix"):
    added = subprocess.check_output("git diff translations/gdprocmesh.pot | grep \+msgid | wc -l", shell=True)
    removed = subprocess.check_output("git diff translations/gdprocmesh.pot | grep \\\-msgid | wc -l", shell=True)
    print("\n# Template changes compared to the staged status:")
    print("#   Additions: %s msgids.\n#   Deletions: %s msgids." % (int(added), int(removed)))
