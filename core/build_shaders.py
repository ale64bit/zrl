import glob
import os
import sys
import subprocess

def main():
  shader_compiler = sys.argv[1]
  input_dir = sys.argv[2]
  output_dir = sys.argv[3]

  if not os.path.exists(output_dir):
    os.makedirs(output_dir)

  for input_file in glob.glob(os.path.join(input_dir, "*.glsl")):
    cmd = [
        shader_compiler, 
        "-V", 
        input_file,
        "--vn", 
        "k" + os.path.basename(input_file).replace('.', '_') + "_",
        "-o",
        os.path.join(output_dir, os.path.basename(input_file) + ".h"),
    ]
    code = subprocess.call(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if code != 0:
      sys.exit(code)

if __name__ == "__main__":
  main()
