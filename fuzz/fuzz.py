import os
import subprocess
import random
import sys

FUZZ_DRIVER = os.path.join(
    os.path.dirname(os.path.realpath(__file__)),
    'fuzz_driver',
)

def random_command(num_records):
    return random.choice([
        "L // flush",
        "u // push",
        "N {} // pushN ",
        "E {} // peek",
        "o // pop FIFO",
        "f {} // pop FIFO n",
        "r {} // remove n",
        "l // pop lifo",
        "I {} // pop lifo n",
    ]).format(random.randint(1, num_records))

def main():
    buf_size = 8
    record_size = 2
    num_records = buf_size / record_size

    while True:
        commandlist = [random_command(num_records) for _ in range(100)]
        commandlist.insert(0, "i {} {}".format(buf_size, record_size))
        command = '\n'.join(commandlist)
        try:
            subprocess.run(
                FUZZ_DRIVER,
                input=command.encode(),
                # stdout=subprocess.PIPE,
                # stderr=subprocess.STDOUT,
                check=True,
            )
        except subprocess.CalledProcessError:
            print(command)
            sys.exit()



if __name__ == '__main__':
    main()