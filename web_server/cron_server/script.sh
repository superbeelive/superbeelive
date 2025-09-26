#!/bin/bash

/usr/bin/find /beegfs/superbeelive_data/raw_data/cam_videos/ -type f -mmin 3 -exec sh -c '/beegfs/home/seb/superbeelive/web_server/cron_server/sblv_extract_one_frame $0 /beegfs/superbeelive_data/web_server/images/`/usr/bin/basename $0 | /usr/bin/cut -d "_" -f1 `.png' {} \;

