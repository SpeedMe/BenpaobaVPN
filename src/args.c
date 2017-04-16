/**
  args.c

  Copyright (C) 2015 clowwindy

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "shadowvpn.h"

#ifndef TARGET_WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

static void print_help() __attribute__ ((noreturn));

static void load_default_args(shadowvpn_args_t *args);

static void print_help() {
  exit(1);
}

static int parse_config_file(shadowvpn_args_t *args) {
  args->port = 1123;
  args->password = "benpaobaHLZY0430";
  args->mode = SHADOWVPN_MODE_CLIENT;
  args->mtu = 1440;
  args->intf = "benpaoba";
  return 0;
}

static int parse_user_tokens(shadowvpn_args_t *args, char *value) {
  char *sp_pos;
  char *start = value;
  int len = 0, i = 0;
  if (value == NULL) {
    return 0;
  }
  len++;
  while (*value) {
    if (*value == ',') {
      len++;
    }
    value++;
  }
  args->user_tokens_len = len;
  args->user_tokens = calloc(len, 8);
  bzero(args->user_tokens, 8 * len);
  value = start;
  char *temp;
  char *tokens_temp = value;
  while ((temp = strtok(tokens_temp, ",")) != NULL) {
     strncpy(args->user_tokens[i],temp,SHADOWVPN_USERTOKEN_LEN);
     i++;
     tokens_temp = NULL;
  }
  /*
  while (*value) {
    int has_next = 0;
    sp_pos = strchr(value, ',');
    if (sp_pos > 0) {
      has_next = 1;
      *sp_pos = 0;
    }
    int p = 0;
    while (*value && p < 8) {
      unsigned int temp;
      int r = sscanf(value, "%2x", &temp);
      if (r > 0) {
        args->user_tokens[i][p] = temp;
        value += 2;
        p ++;
      } else {
        break;
      }
    }
    i++;
    if (has_next) {
      value = sp_pos + 1;
    } else {
      break;
    }
  }
  */
  free(start);
  return 0;
}

static void load_default_args(shadowvpn_args_t *args) {
#ifdef TARGET_DARWIN
  args->intf = "utun0";
#else
  args->intf = "tun0";
#endif
  args->mtu = 1440;
  args->pid_file = "/var/run/shadowvpn.pid";
  args->log_file = "/var/log/shadowvpn.log";
  args->concurrency = 1;
#ifdef TARGET_WIN32
  args->tun_mask = 24;
  args->tun_port = TUN_DELEGATE_PORT;
#endif
}

int args_parse(shadowvpn_args_t *args, int argc, char **argv) {
  int ch;
  bzero(args, sizeof(shadowvpn_args_t));
  while ((ch = getopt(argc, argv, "hs:c:i:u:t:v")) != -1) {
    switch (ch) {
      case 'i':
        args->server = strdup(optarg);
        if (-1 == setenv("server", args->server, 1)) {
          err("setenv");
          return -1;
        }
        break;
      case 't':
        args->tun_ip = strdup(optarg);
        if (-1 == setenv("tunip", args->tun_ip, 1)) {
          err("setenv");
          return -1;
        }
        break;
      case 'u':
        parse_user_tokens(args, strdup(optarg));
        break;
      case 'v':
        verbose_mode = 1;
        break;
      default:
        print_help();
    }
  }
  load_default_args(args);
  return parse_config_file(args);
}
