#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import http.client
import ssl
import certifi

def get_response(host, path, context, max_redirects=5):
    redirects = 0
    while redirects <= max_redirects:
        conn = http.client.HTTPSConnection(host, context=context)
        conn.request("GET", path)
        response = conn.getresponse()
        # Handle 301/302 redirects
        if response.status in (301, 302):
            location = response.getheader('Location')
            if not location:
                raise RuntimeError("Redirect status but no Location header")
            # Parse new host and path
            if location.startswith('https://'):
                location = location[8:]
                split = location.find('/')
                if split == -1:
                    host = location
                    path = '/'
                else:
                    host = location[:split]
                    path = location[split:]
            elif location.startswith('/'):
                path = location
            else:
                # Relative path
                path = '/' + location
            redirects += 1
            conn.close()
            continue
        # Not a redirect, return response
        body = response.read().decode()
        conn.close()
        return response.status, response.reason, body
    raise RuntimeError("Too many redirects")

def main():
    parser = argparse.ArgumentParser(description='HTTPS client with redirect support')
    parser.add_argument('host', help='The target HTTPS server (e.g., www.example.com)')
    parser.add_argument('--path', default='/', help='Path to request (default: /)')
    args = parser.parse_args()

    context = ssl.create_default_context(cafile=certifi.where())
    status, reason, body = get_response(args.host, args.path, context)
    print("Status:", status)
    print("Reason:", reason)
    #print("Body:", body)

if __name__ == '__main__':
    main()

