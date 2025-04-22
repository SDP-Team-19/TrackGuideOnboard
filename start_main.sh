#!/bin/bash

# Wait until the pigpio daemon socket exists
while [ ! -S /dev/pigpio ]; do
    sleep 0.2
done

# Now run the app
/home/team19/TrackGuideOnboard/main