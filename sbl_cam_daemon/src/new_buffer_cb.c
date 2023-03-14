#include <stdio.h>
#include <arv.h>

#include "data.h"

void
new_buffer_cb (ArvStream *stream, data_t *data)
{
	ArvBuffer *buffer;

	buffer = arv_stream_try_pop_buffer (stream);
	if (buffer != NULL) {
		if (arv_buffer_get_status (buffer) == ARV_BUFFER_STATUS_SUCCESS) {
			size_t size = 0;
			data->buffer_count++;
			arv_buffer_get_data (buffer, &size);
			data->transferred += size;
		} else {
			data->error_count++;
		}

		/* Image processing here */

		arv_stream_push_buffer (stream, buffer);
	}


}
