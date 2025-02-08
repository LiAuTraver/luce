import os
import shutil
import glob
import sys


def copy_dlls(src_dir, dest_dir, verbose: bool = False):
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)
    i = 0
    try:
        for file in glob.glob(src_dir + "/*.dll"):
            shutil.copy(file, dest_dir)
            if verbose:
                print("Copying " + file + " to " + dest_dir)
            i += 1
        for file in glob.glob(src_dir + "/*.pdb"):
            shutil.copy(file, dest_dir)
            if verbose:
                print("Copying " + file + " to " + dest_dir)
            i += 1
        print("Copied " + str(i) + " files from " + src_dir + " to " + dest_dir)
    except Exception as e:
        print("Error: " + str(e), file=sys.stderr)
        sys.exit(1)
    if i == 0:
        print("Warning: No dll files found in " + src_dir, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    length = len(sys.argv)
    if length != 3 and length != 4:
        print("Usage: python copy_dlls.py <src_dir> <dest_dir> [--verbose]",file=sys.stderr)
        sys.exit(1)
    if length == 4:
      if sys.argv[3] == "--verbose":
        copy_dlls(sys.argv[1], sys.argv[2], True)
      else:
        print("Usage: python copy_dlls.py <src_dir> <dest_dir> [--verbose]",file=sys.stderr)
        sys.exit(1)
    copy_dlls(sys.argv[1], sys.argv[2])

else:
    pass
