#!/usr/bin/python
import BaseHTTPServer, SimpleHTTPServer
import ssl
    
def run(args):
    httpd = BaseHTTPServer.HTTPServer((args.hostname, args.port), SimpleHTTPServer.SimpleHTTPRequestHandler)
    httpd.socket = ssl.wrap_socket(httpd.socket, certfile=args.cert, server_side=True)
    name, addr = httpd.socket.getsockname()
    print("Serving HTTPS on {} port {}...".format(name, addr))
    httpd.serve_forever()

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--hostname", default="localhost")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--cert", default="certificates.pem")

    #print("Serving HTTPS on {} port {}...".format(name, addr))
    args = parser.parse_args()
    
    run(args)