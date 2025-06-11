#!/usr/bin/env python3

from datetime import datetime, timedelta
from icalendar import Calendar
import argparse
import calendar
import pytz
import sys

# globals
maxcount = -1

# args
parser = argparse.ArgumentParser(
        prog='tyggekalender.py',
        description='Tygge en Bitraf-kalkulator',
        epilog='Så får vi se…',
)
parser.add_argument('-f', '--filename', required=True)          # option that takes a value
parser.add_argument('-d', '--date')                             # date from which to list, give in the format of yyyy-mm-dd
parser.add_argument('-c', '--maxcount', type=int)               # option that takes a value
parser.add_argument('-v', '--verbose', action='store_true')     # on/off flag
parser.add_argument('-T', '--keywords', nargs='+')              # date from which to list, give in the format of yyyy-mm-dd

args = parser.parse_args()

count = 1

def get_event_datetime(event, key):
    dt = event.get(key).dt
    if isinstance(dt, datetime):
        if dt.tzinfo is not None:
            return dt.astimezone(pytz.UTC).replace(tzinfo=None)
        else:
            return dt
    else:
        return datetime.combine(dt, datetime.min.time())

KEYWORDS = ['laser', 'resin', 'form']

if args.keywords:
    KEYWORDS = args.keywords

if (args.date):
    try:
        START_DATE = datetime.strptime(args.date, "%Y-%m-%d")
    except ValueError:
        print("Invalid date format. Please use yyyy-mm-dd.")
        sys.exit(1)
else:
    START_DATE = datetime.strptime('0001-01-01', "%Y-%m-%d")

with open(args.filename, 'rb') as f:
    cal = Calendar.from_ical(f.read())

events = []
total_duration = timedelta()

for component in cal.walk():
    if component.name == "VEVENT":
        dtstart = get_event_datetime(component, 'DTSTART')
        dtend = get_event_datetime(component, 'DTEND')
        summary = str(component.get('SUMMARY', ''))
        if dtstart and dtend:
            if (
                dtstart >= START_DATE and
                any(word.lower() in summary.lower() for word in args.keywords)
            ):
                duration = dtend - dtstart
                events.append((dtstart, dtend, duration, summary))
                total_duration += duration

events.sort(key=lambda x: x[0])

for dtstart, dtend, duration, summary in events:
    date_str = dtstart.strftime('%Y-%m-%d')
    start_time_str = dtstart.strftime('%H:%M')
    end_time_str = dtend.strftime('%H:%M')
    # Duration as hours and minutes
    hours, remainder = divmod(duration.total_seconds(), 3600)
    minutes = remainder // 60
    duration_str = f"{int(hours)}h {int(minutes)}m"
    print(f"{date_str} {start_time_str}-{end_time_str} ({duration_str}) - {summary}")

total_events = len(events)
total_hours, remainder = divmod(total_duration.total_seconds(), 3600)
total_minutes = remainder // 60
print(f"\nTotal events: {total_events}")
print(f"Total time spent: {int(total_hours)}h {int(total_minutes)}m")
