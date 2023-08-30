# TinyTim
A tiny HTTP server just for fun

## Building
```
sudo cmake --build . --target install
```

## Usage

```
TinyTim  [-r -p -c]|[--port --root --config]
```

Default configuration file is stored in `/etc/TinyTim/tinytim.conf`

Configuration directives:

- `port` - bind port (default 80).
- `root` - server root.
- `try_files` - a space-separated list of files that the server will try to open if no explicit location is specified. For example `"index.html"`. TinyTim will try to open the request location first, then will try to access `index.html`, and as the last option, it will try to open `404.html` and send a 404 Not Found response with `404.html` as the body. If there is no `404.html` at `root` an empty body will be sent. 