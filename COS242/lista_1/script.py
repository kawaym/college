import subprocess
import os

PATH = "permuta/target/release/"
PROGRAM = "permuta"
PROGRAM_NAME = os.path.join(PATH, PROGRAM)
RUN_COMMAND = ["time", PROGRAM_NAME, "grep total"]
OUTPUT_PATH = "./output.txt"

for size in range(0, 8, 2):
    with open(OUTPUT_PATH, "w") as f:
        subprocess.run(RUN_COMMAND + [f"{size}"], stdout=f, stderr=subprocess.STDOUT)
