#!/bin/bash

if [[ "$EUID" != 0 ]]; then
    echo "Must be root"
    exit 1
fi

# The generated binary needs cap_net_admin capability:
sudo setcap cap_net_admin=eip ${OUT_DIR}/main

# Start main as a backgroud process:
${OUT_DIR}/main &
main_pid=$!

# After main was started, configure the tun device:
sudo ip addr add 192.168.0.1/24 dev tun0    # Create net interface
sudo ip link set up dev tun0                # link interface to tun device

# wait for main to to finish
wait $main_pid