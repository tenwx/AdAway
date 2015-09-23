#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <android/log.h>
#include <errno.h>
#include "mongoose.h"

#define THIS_FILE "AdAway"

static int s_sig_num = 0;

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  (void) p;
  if (ev == MG_EV_HTTP_REQUEST) {
    mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n"
                          "Content-Type: text/plain\r\n\r\n");
  }
}

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_sig_num = sig_num;
}

int main(void) {
  struct mg_mgr mgr;
  struct mg_connection *nc1;
  struct mg_connection *nc2;
  const char *port1 = "127.0.0.1:80", *port2 = "127.0.0.1:443";

  mg_mgr_init(&mgr, NULL);
  nc1 = mg_bind(&mgr, port1, ev_handler);
  nc2 = mg_bind(&mgr, port2, ev_handler);

  mg_set_protocol_http_websocket(nc1);
  mg_set_protocol_http_websocket(nc2);

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  __android_log_print(ANDROID_LOG_INFO, THIS_FILE, "AdAway Native Webserver: starting");
  while (s_sig_num == 0) {
    mg_mgr_poll(&mgr, 1000);
  }

  mg_mgr_free(&mgr);
  __android_log_print(ANDROID_LOG_INFO, THIS_FILE, "AdAway Native Webserver: exited on signal %d", s_sig_num);
  return EXIT_SUCCESS;
}
