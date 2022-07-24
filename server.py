import http.server
import argparse
import signal
import threading
import os
import sys
import time
import ssl
import subprocess


class BackgroundThread(threading.Thread):
    def __init__(self, service, cleanup):
        threading.Thread.__init__(self)
        self.running = False
        self.service = service
        self.cleanup = cleanup

    def run(self):
        while self.running:
            self.service()
        self.cleanup()

    def start(self):
        self.running = True
        return super().start()

    def stop(self):
        self.running = False


class IWPAServer:
    def __init__(self, PORT=8000, verbose=False, wasm_stream=True, secure=False):
        self.PORT = PORT if not secure else 4443
        self.verbose = verbose
        self.background_thread = None

        # setup handler
        class Handler(http.server.SimpleHTTPRequestHandler):
            def end_headers(self):
                self.send_header("Cross-Origin-Opener-Policy", "same-origin")
                self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
                http.server.SimpleHTTPRequestHandler.end_headers(self)
            def log_message(self, format, *args):
                if verbose:
                    super().log_message(format, *args)

        Handler.extensions_map[".js"] = "text/javascript"
        if wasm_stream:
            Handler.extensions_map[".wasm"] = "application/wasm"
        else:
            Handler.extensions_map[".wasm"] = "text/plain"

        self.httpd = http.server.ThreadingHTTPServer(("", self.PORT), Handler)
        self.httpd.timeout = 2

        if secure:
            certs = self.generate_ssl()
            self.httpd.socket = ssl.wrap_socket(
                self.httpd.socket,
                keyfile=certs["key"],
                certfile=certs["cert"],
                server_side=True,
                ssl_version=ssl.PROTOCOL_TLS,
            )

    def generate_ssl(self):
        cmd = [
            "openssl",
            "req",
            "-x509",
            "-newkey",
            "rsa:2048",
            "-keyout",
            "key.pem",
            "-out",
            "cert.pem",
            "-days",
            "365",
            "-nodes",
            "-subj",
            "/CN=localhost",
        ]
        args = (
            {"stdout": subprocess.PIPE, "stderr": subprocess.PIPE}
            if not self.verbose
            else {}
        )
        sub = subprocess.Popen(cmd, **args)
        code = sub.wait()
        if code != 0:
            sys.stderr.write(f"Failed to generate SSL certificates{os.linesep}")
            sys.exit(1)
        return {"key": "key.pem", "cert": "cert.pem"}

    def start(self):
        if self.verbose:
            print("serving at port", self.PORT)
        self.background_thread = BackgroundThread(
            service=self.httpd.handle_request, cleanup=self.httpd.server_close
        )
        self.background_thread.start()

    def stop(self):
        if self.verbose:
            print("shuting down port", self.PORT)
        if self.background_thread:
            self.background_thread.stop()
            self.background_thread.join()

    def isRunning(self):
        return self.background_thread.running


def main():
    parser = argparse.ArgumentParser(description="HTTP Server")
    parser.add_argument(
        "-p", "--port", type=int, default=8000, help="Port number to serve at"
    )
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Print debug information"
    )
    parser.add_argument(
        "-s",
        "--secure",
        action="store_true",
        default=False,
        help="Secure server using SSL",
    )
    parser.add_argument(
        "--wasm-stream",
        action="store_true",
        default=True,
        help="Enable streaming Web Assembly [DEFAULT]",
    )
    parser.add_argument(
        "--no-wasm-stream",
        action="store_true",
        default=False,
        help="Disable streaming Web Assembly",
    )
    parser.add_argument(
        "--directory", type=str, default=".", help="Run the server at directory"
    )

    args = parser.parse_args()

    # change to working directory
    current_dir = os.getcwd()
    if not os.path.isdir(args.directory):
        sys.stderr.write(f"Invalid directory: '{args.directory}'\n")
        exit(1)
    os.chdir(args.directory)

    server = IWPAServer(
        PORT=args.port,
        verbose=args.verbose,
        wasm_stream=(args.wasm_stream and not args.no_wasm_stream),
        secure=args.secure,
    )

    try:
        server.start()
        while server.isRunning():
            time.sleep(2)
    except KeyboardInterrupt:
        server.stop()

    # change back
    os.chdir(current_dir)


if __name__ == "__main__":
    main()
