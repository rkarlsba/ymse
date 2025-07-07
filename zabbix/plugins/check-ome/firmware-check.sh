#!/bin/bash

# Get inventory details
cmd='/api/deviceservice/devices(64877)/inventorydetails'
echo '================================================================================'
echo $cmd
echo '================================================================================'
echo

# Get hardware logs
cmd='/api/deviceservice/devices(64877)/hardwarelogs'

# Get compliance/baseline info
cmd='/api/updateservice/compliancestatuses?$filter=deviceid eq 64877'

# Get recent activity
cmd='/api/deviceservice/devices(64877)/recentactivity'
