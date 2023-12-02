/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef aclient_H
#define aclient_H

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <uv.h>

#include "http-parser-library/hpl.h"

#include "a-memory-library/aml_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aclient_request_s;
typedef struct aclient_request_s aclient_request_t;

typedef int (*aclient_cb)(aclient_request_t *r);
typedef int (*aclient_status_cb)(aclient_request_t *r, int status);

struct aclient_request_s {
    hpl_t *http;
    int fd; // only to be used for reference

    aml_pool_t *pool;

    void *data;
    aclient_cb on_response;
    aclient_cb on_request;
    aclient_status_cb on_connect_error;
    aclient_cb on_chunk_start;
    aclient_cb on_chunk;
    aclient_cb on_chunk_end;

    struct sockaddr_in dest;
    uv_tcp_t tcp;
    uv_connect_t connect;
    uv_buf_t read_buf;
    uv_loop_t *loop;
    uv_write_t write_req;
};

aclient_request_t *aclient_request_init(int pool_size);

void aclient_request_destroy(aclient_request_t* h);

/* connect a client and associate it to a loop */
aclient_request_t *aclient_request_connect(aclient_request_t* r, uv_loop_t *loop, int port);

/* this clears the http parsing, typically is only needed in the on_response handler if another request
   is issued. */
void aclient_clear(aclient_request_t* r);

/* trigger a generic request to the already connected client */
void aclient_make_request(aclient_request_t* r, const char *request, size_t len);

/* close the connection */
void aclient_close(aclient_request_t *r);

#ifdef __cplusplus
}
#endif

#endif
