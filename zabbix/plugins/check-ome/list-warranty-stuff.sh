#!/bin/bash

cmds=(
    '/api/DeviceService/Devices(64877)/InventoryDetails'
    '/api/DeviceService/Devices(64877)/HardwareLogs'
    '/api/UpdateService/ComplianceStatuses?\$filter=DeviceId eq 64877'
    '/api/DeviceService/Devices(64877)/RecentActivity'
)

for cmd in "${cmds[@]}"
do
    echo '================================================================================'
    echo $cmd
    echo '================================================================================'
    ./query.py "$cmd"
    echo
done
