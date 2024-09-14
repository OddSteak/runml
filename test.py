import os
import subprocess


def run_file(mlfile, expfile):
    passed = False
    command = ['builds/runml', mlfile]

    args = []
    if os.path.isfile(mlfile + '.err'):
        return True

    if os.path.isfile(mlfile + '.args'):
        with open(mlfile + '.args', 'r') as argfile:
            args = argfile.read().strip().split(' ')

    result = subprocess.run(command + args, capture_output=True,
                            text=True)
    lines = [line for line in result.stdout.splitlines()
             if not line.startswith('@')]

    out = '\n'.join(lines)

    with open(expfile, 'r') as expfd:
        expout = expfd.read().strip()

    if out.strip() != expout.strip():
        print(f"{mlfile}: {expfile}\n\tFailed\n")
        print("\tExpected:\n\t\t", expout.replace("\n", "\n\t\t"), sep="")
        print()
        print("\tGot:\n\t\t", result.stdout.strip().replace("\n", "\n\t\t"), sep="")
        print()
        passed = False
    else:
        print(f"{mlfile}: {expfile}\n\tPassed")
        print()
        passed = True

    if result.stderr:
        print("\tprogram printed to stderr:")
        print(f"\t\t{result.stderr}\n")

    return passed


def count_lines(filename):
    count = 0
    with open(filename, 'r') as fd:
        lines = fd.readlines()

    for i in lines:
        if i.strip() not in ["{", "}", ""] and i.strip()[:2] != "//":
            count += 1

    return count


passes = 0
total = 0

test_dir = "tests"
binary = "builds/runml"

if not os.path.isfile(binary) or not os.path.isdir(test_dir):
    print("TEST error: builds/runml or test dir missing")
    exit()

testfiles = [os.path.join(test_dir, f) for f in os.listdir(test_dir)
             if os.path.isfile(os.path.join(test_dir, f))
             and f.split(".")[-1] == "ml"]

testfiles.sort()

for i in range(len(testfiles)):
    if not os.path.isfile(testfiles[i] + ".exp"):
        print("TEST error: name mismatch")
        print("name samples as sample0{num}.ml and out files as \
            sample0{num}.ml.exp")
        exit()

    total += 1

    if run_file(testfiles[i], testfiles[i] + ".exp"):
        passes += 1

print('---------------------------------------------------')
print(f"SCORE: {passes}/{total}")
print("num of lines:", count_lines("runml.c"))
