# Copyright 2019 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
    code = subprocess.call(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if code != 0:
      sys.exit(code)

if __name__ == "__main__":
  main()
