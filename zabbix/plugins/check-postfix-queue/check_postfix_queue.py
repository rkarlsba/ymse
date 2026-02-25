#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

# Example data {{{
#
# {"queue_name": "deferred", "queue_id": "F1332C04F87", "arrival_time": 1771985177, "message_size": 30857, "forced_expire": false, "sender": "ub.system@oslomet.no", "recipients": [{"address": "mona_hanif@hotmail.no", "delay_reason": "host eur.olc.protection.outlook.com[52.101.73.27] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCD825A78A8D0B] [AMS0EPF000001A7.eurprd05.prod.outlook.com 2026-02-25T11:12:39.245Z 08DE6C12E206EF5D] (in reply to MAIL FROM command)"}]}
# {"queue_name": "deferred", "queue_id": "F0D97C03F4A", "arrival_time": 1771945327, "message_size": 6515, "forced_expire": false, "sender": "international-hv@oslomet.no", "recipients": [{"address": "mina.engstrand@hotmail.no", "delay_reason": "host eur.olc.protection.outlook.com[52.101.68.26] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCD77C86CDBFC3] [DU2PEPF00028D05.eurprd03.prod.outlook.com 2026-02-25T11:12:39.343Z 08DE6BBE52283549] (in reply to MAIL FROM command)"}]}
# {"queue_name": "deferred", "queue_id": "F1F27C04FB1", "arrival_time": 1772005276, "message_size": 3343, "forced_expire": false, "sender": "MAILER-DAEMON", "recipients": [{"address": "martinemena@hotmail.no", "delay_reason": "host eur.olc.protection.outlook.com[52.101.68.2] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCE58817C1EBE0] [DU2PEPF0001E9C1.eurprd03.prod.outlook.com 2026-02-25T11:12:39.356Z 08DE72C40EF2862F] (in reply to MAIL FROM command)"}]}
# {"queue_name": "deferred", "queue_id": "F0249C003EC", "arrival_time": 1771923375, "message_size": 2750, "forced_expire": false, "sender": "no-reply@oslomet.no", "recipients": [{"address": "trine.lillehammer@outlook.com", "delay_reason": "host outlook-com.olc.protection.outlook.com[52.101.73.10] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCD75E610A64A1] [AM3PEPF0000A791.eurprd04.prod.outlook.com 2026-02-25T11:12:43.503Z 08DE6BAF3F595E24] (in reply to MAIL FROM command)"}]}
# {"queue_name": "deferred", "queue_id": "F2A39C003CE", "arrival_time": 1771930404, "message_size": 2072, "forced_expire": false, "sender": "opptak@oslomet.no", "recipients": [{"address": "lise.so@outlook.com", "delay_reason": "host outlook-com.olc.protection.outlook.com[52.101.73.10] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCD78E7D82E1F2] [AM3PEPF0000A792.eurprd04.prod.outlook.com 2026-02-25T11:12:44.654Z 08DE6BC74D713E2A] (in reply to MAIL FROM command)"}]}
# {"queue_name": "deferred", "queue_id": "F181FC04808", "arrival_time": 1771967840, "message_size": 3025, "forced_expire": false, "sender": "no-reply@oslomet.no", "recipients": [{"address": "trine.lillehammer@outlook.com", "delay_reason": "host outlook-com.olc.protection.outlook.com[52.101.41.57] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCDE3D75AFC073] [SJ1PEPF000023D4.namprd21.prod.outlook.com 2026-02-25T11:12:45.277Z 08DE6F1EC1565A94] (in reply to MAIL FROM command)"}]}
# {"queue_name": "deferred", "queue_id": "F09D9C07FD0", "arrival_time": 1771898526, "message_size": 30919, "forced_expire": false, "sender": "ub.system@oslomet.no", "recipients": [{"address": "suzanneschulzki@hotmail.com", "delay_reason": "host hotmail-com.olc.protection.outlook.com[52.101.73.3] said: 451 4.7.650 The mail server [158.36.161.6] has been temporarily rate limited due to IP reputation. For e-mail delivery information, see https://aka.ms/postmaster (S775) [Name=Protocol Filter Agent][AGT=PFA][MxId=11BCD7F0C84DB014] [AM1PEPF000252DD.eurprd07.prod.outlook.com 2026-02-25T11:12:54.216Z 08DE6BF872D36486] (in reply to MAIL FROM command)"}]}
#
# }}}

import subprocess
import json
from collections import Counter

QUEUES = {"maildrop", "hold", "incoming", "active", "deferred"}

def count_postqueue():
    # Run postqueue -j and capture output
    result = subprocess.run(
        ["postqueue", "-j"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=True,
    )

    counts = Counter()

    for line in result.stdout.splitlines():
        if not line.strip():
            continue

        try:
            item = json.loads(line)
        except json.JSONDecodeError:
            # Anything unparsable is "invalid"
            counts["invalid"] += 1
            continue

        q = item.get("queue_name")
        if q in QUEUES:
            counts[q] += 1
        else:
            counts["invalid"] += 1

    # Ensure all categories exist
    summary = {q: counts.get(q, 0) for q in sorted(QUEUES)}
    summary["invalid"] = counts.get("invalid", 0)

    return summary


if __name__ == "__main__":
    summary = count_postqueue()
    print(json.dumps(summary, indent=2))

