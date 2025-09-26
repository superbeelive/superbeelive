#!/bin/bash

/usr/bin/find /beegfs/superbeelive_data/raw_data/cam_videos/ -type f -mmin 3 -exec sh -c '/usr/local/bin/sblv_extract_one_frame $0 /beegfs/superbeelive_data/web_server/images/`/usr/bin/basename $0 | /usr/bin/cut -d "_" -f1 `.png' {} \;

