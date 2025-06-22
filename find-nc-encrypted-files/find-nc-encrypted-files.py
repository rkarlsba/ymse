#!/usr/bin/env python3

import argparse
import magic
import os
import requests
import sys

def check_file_contents(file):
    mime = magic.Magic(mime=True)
    file_type = magic.Magic()

    with open(file, 'rb') as f:
        content = f.read(1024)  # Read the first 1KB; usually enough for magic detection

    # Get MIME type
    print(mime.from_buffer(content))

    # Get human-readable description (like the file command)
    print(file_type.from_buffer(content))

def find_nc_encrypted_files(path, oce):
    # HBEGIN:oc_encryption_module:OC_DEFAULT_MODULE:cipher:AES-256-CTR:signed:true:HEND
    signature = 'HBEGIN:oc_encryption_module'

    for dirpath, dirnames, filenames in os.walk(path):
        for filename in filenames:
            if filename.startswith(signature):
                print(os.path.join(dirpath, filename))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Search for files encrypted by nextcloud')
    parser.add_argument('path', type=str, help='Which path to start looking for files')
    parser.add_argument('oc-encrypted', type=str, help='Only find files encryped by Owncloud/Nextcloud')
    args = parser.parse_args()

    find_nc_encrypted_files(args.path)

