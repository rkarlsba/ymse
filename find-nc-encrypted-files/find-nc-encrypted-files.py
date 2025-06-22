#!/usr/bin/env python3

import argparse
import magic
import os
import requests
import sys
from pathvalidate import sanitize_filepath

def is_valid_path(file_path):
    return file_path == sanitize_filepath(file_path)

def check_file_contents(file):
    mime = magic.Magic(mime=True)
    file_type = magic.Magic()

    with open(file, 'rb') as f:
        content = f.read(1024)  # Read the first 1KB; usually enough for magic detection

    # Get MIME type
    # print(mime.from_buffer(content))

    # Get human-readable description (like the file command)
    # print(file_type.from_buffer(content))
    return file_type.from_buffer(content)

def file_is_oc_encrypted(file):
    # HBEGIN:oc_encryption_module:OC_DEFAULT_MODULE:cipher:AES-256-CTR:signed:true:HEND
    os_enc_signature = b'HBEGIN:oc_encryption_module'
    with open(file, 'rb') as f:
        content = f.read(1024)  # Read the first 1KB; usually enough for magic detection
    return content.startswith(os_enc_signature)


def find_nc_encrypted_files(path, oc_encrypted=False):

    try:
        filetype = ''
        for dirpath, dirnames, filenames in os.walk(path):
            for filename in filenames:
                fullfilename = os.path.join(dirpath, filename)
                if (oc_encrypted):
                    if file_is_oc_encrypted(fullfilename):
                        filetype = 'Encrypted by Owncloud/Nextcloud'
                    else:
                        next
                else:
                    filetype = check_file_contents(fullfilename)
                print(f"{fullfilename}: {filetype}")
    except KeyboardInterrupt:
        print("Exiting cleanly.")
        sys.exit(0)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Search for files encrypted by nextcloud')
    parser.add_argument('path', type=str, help='Which path to start looking for files')
    parser.add_argument('-e', '--oc-encrypted', action='store_true', help='Only find files encryped by Owncloud/Nextcloud')

    args = parser.parse_args()

    if not os.path.isdir(args.path):
        print(f"ERROR: {args.path} is not a dir")
        sys.exit(1)

    find_nc_encrypted_files(args.path, args.oc_encrypted)

