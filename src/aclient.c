#include "a-client-library/aclient.h"
#include "a-memory-library/aml_alloc.h"

void on_client_body(hpl_t *h) {
  aclient_request_t *r = (aclient_request_t*)h->data;
  // printf( "%s\n", __func__ );
  if(r->on_response)
    r->on_response(r);
}

void on_client_chunk(hpl_t *h) {
  aclient_request_t *r = (aclient_request_t *)h->data;
  // printf( "%s\n", __func__ );
  if(r->on_chunk)
    r->on_chunk(r);
}

void on_client_chunk_encoding(hpl_t *h) {
  aclient_request_t *r = (aclient_request_t *)h->data;
  // printf( "%s\n", __func__ );
  if(r->on_chunk_start)
    r->on_chunk_start(r);
}

void on_client_chunk_complete(hpl_t *h) {
  aclient_request_t *r = (aclient_request_t*)h->data;
  // printf( "%s\n", __func__ );
  if(r->on_chunk_end)
    r->on_chunk_end(r);
}

aclient_request_t *aclient_request_init(int pool_size) {
    aclient_request_t* r = (aclient_request_t*)aml_zalloc(sizeof(aclient_request_t));
    r->http = hpl_client_init(on_client_body, pool_size);
    hpl_chunk(r->http, on_client_chunk,
                         on_client_chunk_encoding, on_client_chunk_complete);
    r->http->data = r;
    r->pool = r->http->pool;
    r->read_buf.base = NULL;
    r->read_buf.len = 0;

    return r;
}

void aclient_request_destroy(aclient_request_t* h) {
    hpl_destroy(h->http);
    if(h->read_buf.base)
        aml_free(h->read_buf.base);
    aml_free(h);
}


static void on_alloc(uv_handle_t *client, size_t suggested_size,
                     uv_buf_t *buf) {
    aclient_request_t *r = (aclient_request_t*)client->data;
    if(r->read_buf.base == NULL) {
        r->read_buf.base = (char *)aml_malloc(8192);
        r->read_buf.len = 8192;
    }
    *buf = r->read_buf;
}

static void on_close(uv_handle_t* client) {
    aclient_request_t *r = (aclient_request_t*)client->data;
    // do something
}

void aclient_close(aclient_request_t *r) {
   uv_close((uv_handle_t*)&r->tcp, on_close);
}

static void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  aclient_request_t *r = (aclient_request_t*)stream->data;
  if (nread > 0) {
    if(!hpl_data(r->http, buf->base, nread))
       aclient_close(r);
  } else if (nread == 0) {
      /* no-op - there's no data to be read, but there might be later */
  } else
     aclient_close(r);
}


void after_write(uv_write_t* req, int status) {
    aclient_request_t *r = (aclient_request_t*)req->data;
    if (status < 0) {
        return;
    }

    uv_read_start((uv_stream_t *)&r->tcp, on_alloc, on_read);
}

void aclient_clear(aclient_request_t* r) {
    hpl_clear(r->http);
    r->read_buf.base = NULL;
    r->read_buf.len = 0;
}

void aclient_make_request(aclient_request_t* r, const char *request, size_t len) {
    uv_buf_t buffer = uv_buf_init((char *)request, len);
    uv_write_t *write_req = (uv_write_t*)aml_pool_zalloc(r->pool, sizeof(*write_req));
    write_req->data = r;
    uv_write(write_req, (uv_stream_t *)&r->tcp, &buffer, 1, after_write);
}

void on_connect(uv_connect_t* connection, int status) {
    aclient_request_t *r = (aclient_request_t*)connection->data;
    hpl_clear(r->http);

    printf( "%s\n", __func__ );
    if(status < 0) {
        if(r->on_connect_error)
            r->on_connect_error(r, status);
        return;
    }

    if(r->on_request)
        r->on_request(r);
}

aclient_request_t *aclient_request_connect(aclient_request_t* r, uv_loop_t *loop, int port) {
    r->loop = loop;
    uv_ip4_addr("127.0.0.1", port, &r->dest);
    uv_tcp_init(r->loop, &r->tcp);
    r->tcp.data = r;
    r->connect.data = r;
    uv_tcp_connect(&r->connect, &r->tcp, (const struct sockaddr*)&r->dest, on_connect);
    return r;
}
