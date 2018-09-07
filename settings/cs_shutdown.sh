#!/bin/bash

# Get ES pid
ES_PID=`pidof emulationstation`

if [[ $ES_PID != "" ]] ; then

  echo "ES_PID: $ES_PID"

  # Touch the shutdown file
  touch /tmp/es-shutdown
  chown pi:pi /tmp/es-shutdown

  # Tell ES to terminate
  kill -s SIGTERM $ES_PID

  echo "ES has been asked to shutdown"

else

  echo "Could not find ES PID ($ES_PID)"
  sudo shutdown -h now &

fi

