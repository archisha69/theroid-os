import os
from contextlib import contextmanager

@contextmanager
def cd(dir):
    prevdir = os.getcwd()
    os.chdir(os.path.expanduser(dir))
    try:
        yield dir
    finally:
        os.chdir(prevdir)

argv = str()

for i in os.listdir("files"):
    if not os.path.isdir(i):
        argv += 2 * (i + " ")

with cd("files"):
    os.system(f"../initrd_generator {argv}")

with cd("src"):
    os.system("make build")