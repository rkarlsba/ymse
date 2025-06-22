#!/usr/bin/env python3

import argparse
import requests

user_agent = (
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/114.0.0.0 Safari/537.36"
)

def main():
    parser = argparse.ArgumentParser(description="Download a web page over HTTPS and save it to a file.")
    parser.add_argument("url", help="The URL of the web page to download (must start with http:// or https://)")  # [2][4][5][6][7][8]
    parser.add_argument("-o", "--output", default="webpage.html", help="Output file name (default: webpage.html)")
    args = parser.parse_args()

    try:
        response = requests.get(args.url)
        response.raise_for_status()
        with open(args.output, "w", encoding="utf-8") as file:
            file.write(response.text)
        print(f"Web page downloaded and saved as {args.output}")
    except Exception as e:
        print(f"Error downloading {args.url}: {e}")

if __name__ == "__main__":
    main()

