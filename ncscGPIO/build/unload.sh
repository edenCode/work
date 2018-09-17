#!/bin/sh

module="ncsc"

# invoke rmmod with all arguments we got
rmmod $module.ko || exit 1

# Remove stale nodes
rm -f /dev/${module}
