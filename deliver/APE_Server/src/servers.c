/*
  Copyright (C) 2006, 2007, 2008, 2009  Anthony Catel <a.catel@weelya.com>

  This file is part of APE Server.
  APE is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  APE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with APE ; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* servers.c */

#include "servers.h"
#include "sock.h"
#include "utils.h"
#include "config.h"
#include "http.h"
#include "handle_http.h"
#include "transports.h"
#include "main.h"
#include "log.h"

static void ape_read(ape_socket *co, ape_buffer *buffer, size_t offset, acetables *g_ape)
{
	process_http(&co->buffer_in, &co->http);

	if (co->http.ready == 1) {
		co->attach = checkrecv(buffer->data, co, g_ape, co->ip_client);

		co->buffer_in.length = 0;
		co->http.ready = -1;

	} else if (co->http.error == 1) {
		shutdown(co->fd, 2);
	}
}

static void ape_sent(ape_socket *co, acetables *g_ape)
{
	if (co->attach != NULL && ((subuser *)(co->attach))->burn_after_writing) {
		transport_data_completly_sent((subuser *)(co->attach), ((subuser *)(co->attach))->user->transport);
		((subuser *)(co->attach))->burn_after_writing = 0;
	}	
}

static void ape_disconnect(ape_socket *co, acetables *g_ape)
{
	if (co->attach != NULL) {
		if (co->fd == ((subuser *)(co->attach))->client->fd) {
			((subuser *)(co->attach))->headers.sent = 0;
			((subuser *)(co->attach))->state = ADIED;
			
			http_headers_free(((subuser *)(co->attach))->headers.content);
			((subuser *)(co->attach))->headers.content = NULL;
		}
		//if (((subuser *)(co->attach))->wait_for_free == 1) {
        if (co->attach != NULL && ((subuser *)(co->attach))->wait_for_free == 1) {
			free(co->attach);
			co->attach = NULL;
		}
	}
}

int servers_init(acetables *g_ape)
{
	ape_socket *main_server;
	if ((main_server = ape_listen(atoi(CONFIG_VAL(Server, port, g_ape->srv)), CONFIG_VAL(Server, ip_listen, g_ape->srv), g_ape)) == NULL) {
		return 0;
	}

	main_server->callbacks.on_read = ape_read;
	main_server->callbacks.on_disconnect = ape_disconnect;
	main_server->callbacks.on_data_completly_sent = ape_sent;
	
	return 1;
}

