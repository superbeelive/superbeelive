from http.server import BaseHTTPRequestHandler, HTTPServer
import os

HOST = "0.0.0.0"
PORT = 8080

class MyHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/":
            # Page d’accueil avec auto-reload toutes les 10 minutes
            self.send_response(200)
            self.send_header("Content-type", "text/html; charset=utf-8")
            self.end_headers()
            html = """
            <html>
              <head>
                <meta http-equiv="refresh" content="60"> <!-- Reload toutes les 60s -->
              </head>
              <body>
                <h1>Serveur d'images (.png)</h1>
                <img src="M01C01" width=500>
                <img src="M01C02" width=500>
                <img src="M02C01" width=500>
                <img src="M02C02" width=500>
              </body>
            </html>
            """
            self.wfile.write(html.encode("utf-8"))
        else:
            # Récupère le nom demandé sans le "/"
            name = self.path.lstrip("/")      # ex: "chat"
            filename = f"/tmp/{name}.png"     # ajoute .png automatiquement

            if os.path.exists(filename):
                self.send_response(200)
                self.send_header("Content-type", "image/png")
                self.end_headers()
                with open(filename, "rb") as f:
                    self.wfile.write(f.read())
            else:
                self.send_error(404, f"Image {filename} introuvable")

if __name__ == "__main__":
    httpd = HTTPServer((HOST, PORT), MyHandler)
    print(f"Serveur démarré sur http://{HOST}:{PORT}")
    httpd.serve_forever()
