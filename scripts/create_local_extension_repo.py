###
# This script copies all extensions in a build folder from their cmake-produced structure into the extension repository
# structure of ./<goose_version>/<build_archictecture>/<extension_name>.goose_extension
# Note that it requires goose_platofrom_out file to be populated with the platform

import os
import sys
import subprocess
import glob
import shutil

if len(sys.argv) != 6:
    print(
        "Usage: scripts/create_local_extension_repo.py <goose_version> <goose_platform_out> <path/to/goose/build> <path/to/local_repo> <postfix>"
    )
    exit(1)

goose_version = sys.argv[1]
goose_platform_out = sys.argv[2]
extension_path = sys.argv[3]
dst_path = sys.argv[4]
postfix = sys.argv[5]

if os.name == 'nt':
    goose_platform_out = goose_platform_out.replace("/", "\\")
    extension_path = extension_path.replace("/", "\\")
    dst_path = dst_path.replace("/", "\\")

with open(goose_platform_out, 'r') as f:
    lines = f.readlines()
    goose_platform = lines[0]

# Create destination path
dest_path = os.path.join(dst_path, goose_version, goose_platform)
if not os.path.exists(dest_path):
    os.makedirs(dest_path)

# Now copy over the extensions to the correct path
glob_string = os.path.join(extension_path, 'extension', '*', '*.' + postfix)

for file in glob.glob(glob_string):
    dest_file = os.path.join(dest_path, os.path.basename(file))
    shutil.copy(file, dest_file)
