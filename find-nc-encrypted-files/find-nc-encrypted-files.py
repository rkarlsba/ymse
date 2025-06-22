#!/usr/bin/env python3

import argparse
import requests
import sys
import os

def find_nc_encrypted_files(path):
    # HBEGIN:oc_encryption_module:OC_DEFAULT_MODULE:cipher:AES-256-CTR:signed:true:HEND
    signature = 'HBEGIN:oc_encryption_module'

    for dirpath, dirnames, filenames in os.walk(path):
        for filename in filenames:
            if filename.startswith(signature):
                print(os.path.join(dirpath, filename))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Search for files encrypted by nextcloud')
    parser.add_argument('path', type=str, help='Which path to start looking for files')
    args = parser.parse_args()

    find_nc_encrypted_files(args.path)

