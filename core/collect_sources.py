import glob
import os
import sys
from shutil import copyfile

_EXTENSIONS = ["h", "hh", "hpp", "c", "cc", "cpp"]

def main():
  output_dir = sys.argv[1]
  if not os.path.exists(output_dir):
    os.makedirs(output_dir)

  for input_dir in sys.argv[2:]:
    for ext in _EXTENSIONS:
      for f in glob.glob(os.path.join(input_dir, "*." + ext)):
        copyfile(f, os.path.join(output_dir, os.path.basename(f)))

if __name__ == "__main__":
  main()
