# Overview of A Client Library

A Client Library (ACL) is a sophisticated and versatile library for handling client-side network communication in C. Designed to integrate seamlessly with `libuv` for efficient asynchronous I/O operations, this library excels in managing HTTP client requests and responses. It harnesses the capabilities of the HTTP Parser Library (HPL) for parsing HTTP data and utilizes A Memory Library for effective memory management. ACL is equipped to handle various HTTP methods, support chunked transfers, and manage connection errors, making it ideal for robust client-server communication.

## Dependencies

* [libuv](https://github.com/libuv/libuv) for asynchronous I/O operations.
* [HTTP Parser Library](https://github.com/contactandyc/http-parser-library) for parsing HTTP requests and responses.
* [A memory library](https://github.com/contactandyc/a-memory-library) for memory management.

## Installation

### Clone the library and change to the directory

```bash
git clone https://github.com/contactandyc/a-client-library.git
cd a-client-library
```

### Build and install the library

```bash
mkdir -p build
cd build
cmake ..
make
make install
```

## An Example

```c
#include <stdio.h>
#include "a-client-library/aclient.h"
#include "http-parser-library/hpl.h"
#include "a-memory-library/aml_pool.h"

// Callback function for handling the response
int on_response(aclient_request_t *r) {
    hpl_t *http_parser = r->http;

    // Print the status code
    printf("Status Code: %d\n", http_parser->status_code);

    // Print the response body
    printf("Response Body: %.*s\n", (int)http_parser->body.len, http_parser->body.base);

    return 0;
}

int main() {
    uv_loop_t *loop = uv_default_loop();

    // Initialize a client request
    aclient_request_t *request = aclient_request_init(1024);
    request->on_response = on_response;

    // Connect to the server
    aclient_request_connect(request, loop, 80);

    // Make an HTTP GET request
    const char *http_get_request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    aclient_make_request(request, http_get_request, strlen(http_get_request));

    // Run the loop
    uv_run(loop, UV_RUN_DEFAULT);

    // Clean up
    aclient_request_destroy(request);
    return 0;
}
```

In this example:

- A client request is initialized and a response callback is set using `aclient_request_init` and `aclient_request_connect`.
- An HTTP GET request is made to a server using `aclient_make_request`.
- The `on_response` callback function processes the HTTP response, prints the status code and the response body.

## Core Functions

- **aclient_request_init**: Initializes a client request.
- **aclient_request_connect**: Connects a client to a server.
- **aclient_make_request**: Makes a generic HTTP request.
- **aclient_close**: Closes the client connection.
- **aclient_clear**: Clears the HTTP parsing context.
- **aclient_request_destroy**: Destroys a client request and frees resources.

## Callback Functions

- **on_response**: Handles the HTTP response.
- **on_request**: Invoked on sending a request.
- **on_connect_error**: Handles connection errors.
- **on_chunk_start**: Invoked at the start of a chunked transfer.
- **on_chunk**: Processes chunk data.
- **on_chunk_end**: Invoked at the end of a chunked transfer.

This README offers a detailed guide to using A Client Library, covering dependencies, installation steps, a usage example, and a comprehensive list of its core functionalities and callback mechanisms.