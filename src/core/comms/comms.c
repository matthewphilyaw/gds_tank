#include <stdio.h>

#include "gds_tank/comms/comms.h"
#include "gds_tank/drivers/network_driver.h"

#include "lwipopts.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define TCP_POLL_TIME_INTERVAL 10
#define BASE_STATION_TCP_PORT 4242
#define BASE_STATION_MESSAGE_BUF_SIZE 128

#if 1
static void dump_bytes(const uint8_t *bptr, uint32_t len) {
    unsigned int i = 0;

    printf("dump_bytes %d", len);
    for (i = 0; i < len;) {
        if ((i & 0x0f) == 0) {
            printf("\n");
        } else if ((i & 0x07) == 0) {
            printf(" ");
        }
        printf("%02x ", bptr[i++]);
    }
    printf("\n");
}
#define DUMP_BYTES dump_bytes
#else
#define DUMP_BYTES(A,B)
#endif

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    uint8_t buffer[BASE_STATION_MESSAGE_BUF_SIZE];
    int buffer_len;
    int sent_len;
    bool connected;
} TCP_CLIENT_T;

static TCP_CLIENT_T tcp_client_state;

static err_t tcp_client_close(TCP_CLIENT_T *state) {
    printf("closing connection.\n");
    err_t err = ERR_OK;
    if (state->tcp_pcb != NULL) {
        tcp_arg(state->tcp_pcb, NULL);
        tcp_poll(state->tcp_pcb, NULL, 0);
        tcp_sent(state->tcp_pcb, NULL);
        tcp_recv(state->tcp_pcb, NULL);
        tcp_err(state->tcp_pcb, NULL);
        err = tcp_close(state->tcp_pcb);
        if (err != ERR_OK) {
            printf("close failed %d, calling abort\n", err);
            tcp_abort(state->tcp_pcb);
            err = ERR_ABRT;
        }
        state->tcp_pcb = NULL;
    }
    return err;
}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tcp_pcb)
{
    printf("LWIP polled app\n");
    return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (err != ERR_OK) {
        printf("connect failed %d\n", err);
        return tcp_client_close(state);
    }

    state->connected = true;
    printf("Waiting for buffer from server\n");
    return ERR_OK;
}

static void tcp_client_err(void *arg, err_t err) {
    printf("tcp_client_err %s\n", lwip_strerr(err));
    if (err != ERR_ABRT) {
        tcp_client_close(arg);
    }
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    printf("tcp_client_sent %u\n", len);
    state->sent_len += len;

    if (state->sent_len >= BASE_STATION_MESSAGE_BUF_SIZE) {
        // We should receive a new buffer from the server
        state->buffer_len = 0;
        state->sent_len = 0;
        printf("Waiting for buffer from server\n");
    }

    return ERR_OK;
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (!p) {
        printf("Connection closed by server. Error %s\n", lwip_strerr(err));
        return tcp_client_close(arg);
    }
    if (p->tot_len > 0) {
        printf("recv %d err %d\n", p->tot_len, err);
        for (struct pbuf *q = p; q != NULL; q = q->next) {
            DUMP_BYTES(q->payload, q->len);
        }
        // Receive the buffer
        const uint16_t buffer_left = BASE_STATION_MESSAGE_BUF_SIZE - state->buffer_len;
        state->buffer_len += pbuf_copy_partial(p, state->buffer + state->buffer_len,
                                               p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);

    // If we have received the whole buffer, send it back to the server
    if (state->buffer_len == BASE_STATION_MESSAGE_BUF_SIZE) {
        printf("Writing %d bytes to server\n", state->buffer_len);
        err_t err = tcp_write(tpcb, state->buffer, state->buffer_len, TCP_WRITE_FLAG_COPY);
        if (err != ERR_OK) {
            printf("Failed to write data %d\n", err);
            return tcp_client_close(state);
        }
    }
    return ERR_OK;
}

static err_t tcp_client_init_state_and_open(TCP_CLIENT_T* tcp_client)
{
    ip4addr_aton(BASE_STATION_SERVER_IP, &tcp_client->remote_addr);
    tcp_client->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&tcp_client->remote_addr));
    if (!tcp_client->tcp_pcb)
    {
        printf("Failed to initialize tcp_client_state - invalid tcp_pcb.\n");
        return false;
    }

    tcp_arg(tcp_client->tcp_pcb, tcp_client);
    tcp_poll(tcp_client->tcp_pcb, tcp_client_poll, TCP_POLL_TIME_INTERVAL);
    tcp_sent(tcp_client->tcp_pcb, tcp_client_sent);
    tcp_recv(tcp_client->tcp_pcb, tcp_client_recv);
    tcp_err(tcp_client->tcp_pcb, tcp_client_err);

    tcp_client->buffer_len = 0;

    printf("Attempting to connect to: %s on %d", ip4addr_ntoa(&tcp_client->remote_addr), BASE_STATION_TCP_PORT);
    err_t err = tcp_connect(tcp_client->tcp_pcb, &tcp_client->remote_addr, BASE_STATION_TCP_PORT, tcp_client_connected);

    return err;
}


uint32_t comms_init()
{
    if (!network_driver_init())
    {
        return 2;
    }


    err_t err;
    if ((err = tcp_client_init_state_and_open(&tcp_client_state)))
    {
        printf("failed to open connection. System halted. status: %s\n", lwip_strerr(err));
        sleep_ms(1000);
        printf("Exiting.\n");
        return 1;
    }

    printf("Connected to server.\n");

    return 0;
}

void comms_poll()
{
    network_driver_poll();
}

void comms_deinit()
{
    network_driver_deinit();
}