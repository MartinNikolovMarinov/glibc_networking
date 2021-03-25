#!/bin/bash

if [[ "$EUID" != 0 ]]; then
    echo "Must be root"
    exit 1
fi

sudo setcap cap_net_admin=eip ${OUTDIR}/main
${OUTDIR}/main &
pid=$!
    sudo ip addr add 192.168.0.1/24 dev tun0
    sudo ip link set up dev tun0
wait $pid