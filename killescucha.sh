#!/bin/bash

echo "Todos los procesos escucha se encuentran en estado: "
kill -9 $(ps aux | grep 'escucha*' | awk '{print $2}')
