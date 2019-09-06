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
