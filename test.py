import os
import subprocess


def run_file(mlfile, expfile):
    passed = False
    result = subprocess.run(['builds/runml', mlfile], capture_output=True,
                            text=True)
    lines = [line for line in result.stdout.splitlines()
             if not line.startswith('@')]

    out = '\n'.join(lines)

    with open(expfile, 'r') as expfd:
        expout = expfd.read().strip()

    if out.strip() != expout.strip():
        print(f"{mlfile}: {expfile}\n\tFailed\n")
        print(f"\tExpected:\n\t{expout}\n")
        print(f"\tGot:\n\t{result.stdout.strip()}\n")
        passed = False
    else:
        print(f"{mlfile}: {expfile}\n\tPassed")
        print()
        passed = True

    if result.stderr:
        print("\tprogram printed to stderr:")
        print(f"\t\t{result.stderr}\n")

    return passed


passes = 0
total = 0

test_dir = "tests"
binary = "builds/runml"

if not os.path.isfile(binary) or not os.path.isdir(test_dir):
    print("TEST error: builds/runml or test dir missing")
    exit()

testfiles = [f for f in os.listdir(test_dir)
             if os.path.isfile(os.path.join(test_dir, f))
             and f.split(".")[-1] == "ml"]

expout = [f for f in os.listdir(test_dir)
          if os.path.isfile(os.path.join(test_dir, f))
          and f.split(".")[-1] == "exp"]

expout.sort()
testfiles.sort()

if len(testfiles) != len(expout):
    print("TEST error: number of test files doesn't match output files \
            in test directory")
    print("test files should end with .ml and exp outs should have \
            .exp appended to the file name")
    exit()

for i in range(len(expout)):
    if expout[i].split(".")[:-1] != testfiles[i].split(".") \
            or expout[i].split(".")[-1] != "exp":
        print("TEST error: name mismatch")
        print("name samples as sample0{num}.ml and out files as \
            sample0{num}.ml.exp")
        exit()

    total += 1

    if run_file(testfiles[i], expout[i]):
        passes += 1

print('---------------------------------------------------')
print(f"SCORE: {passes}/{total}")
