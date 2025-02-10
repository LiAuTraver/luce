import os
import shutil
import glob
import sys

def create_symlinks(src_dir, dest_dir, verbose: bool = False):
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)
    i = 0
    exists = 0
    try:
        for file in glob.glob(src_dir + "/*.dll"):
            dest_file = os.path.join(dest_dir, os.path.basename(file))
            if os.path.exists(dest_file):
                exists += 1
                continue
            os.symlink(file, dest_file)
            if verbose:
                print("Creating symlink from " + file + " to " + dest_file)
            i += 1
        for file in glob.glob(src_dir + "/*.pdb"):
            dest_file = os.path.join(dest_dir, os.path.basename(file))
            if os.path.exists(dest_file):
                exists += 1
                continue
            os.symlink(file, dest_file)
            if verbose:
                print("Creating symlink from " + file + " to " + dest_file)
            i += 1
        
        if i == 0 and exists == 0:
            print("Warning: No dll file found in " + src_dir, file=sys.stderr)
            return
        if exists > 0:
            print(str(exists) + " symlink(s) already exist in " + dest_dir + " and were skipped.")
        if i > 0:
            print("Created " + str(i) + " symlink(s) from " + src_dir + " to " + dest_dir)
    except OSError as e:
        print("OSError: " + str(e), file=sys.stderr)
        print("Ensure you are running as administrator or have the SeCreateSymbolicLinkPrivilege.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print("Error: " + str(e), file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    length = len(sys.argv)
    if length != 3 and length != 4:
        print("Usage: python create_symlinks.py <src_dir> <dest_dir> [--verbose]", file=sys.stderr)
        sys.exit(1)
    if length == 4:
        if sys.argv[3] == "--verbose":
            create_symlinks(sys.argv[1], sys.argv[2], True)
        else:
            print("Usage: python create_symlinks.py <src_dir> <dest_dir> [--verbose]", file=sys.stderr)
            sys.exit(1)
    else:
        create_symlinks(sys.argv[1], sys.argv[2])
else:
  pass