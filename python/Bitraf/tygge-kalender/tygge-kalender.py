#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> with the handsome help from Perplexity.
# Licensed under GPLv3. See LICENSE.md details or README.md for more info about this script.

from datetime import datetime, timedelta
from icalendar import Calendar
import argparse
import pytz
import sys

def get_event_datetime(event, key):
    dt = event.get(key).dt
    if isinstance(dt, datetime):
        if dt.tzinfo is not None:
            return dt.astimezone(pytz.UTC).replace(tzinfo=None)
        else:
            return dt
    else:
        return datetime.combine(dt, datetime.min.time())

def main():
    # Globals
    events = []
    total_duration = timedelta()

    # To be overridden by options below
    start_date = datetime.strptime('0001-01-01', "%Y-%m-%d")
    keywords = ['laser', 'resin', 'form']
    verbose = False

    # Parse arguments
    parser = argparse.ArgumentParser(
            prog='tygge-kalender.py',
            description='Tygge en Bitraf-kalkulator',
            epilog='Så får vi se…',
    )
    parser.add_argument('-f', '--filename', required=True)          # Give filename
    parser.add_argument('-d', '--date')                             # Date from which to list, give in the format of yyyy-mm-dd
    parser.add_argument('-v', '--verbose', action='store_true')     # Be verbose (print every event found matching the keywords)
    parser.add_argument('-T', '--keywords', nargs='+')              # List keywords to search for

    args = parser.parse_args()

    if args.keywords:
        keywords = args.keywords

    if args.verbose:
        verbose+=1

    if (args.date):
        try:
            start_date = datetime.strptime(args.date, "%Y-%m-%d")
        except ValueError:
            print("Invalid date format. Please use yyyy-mm-dd.")
            sys.exit(1)

    with open(args.filename, 'rb') as f:
        cal = Calendar.from_ical(f.read())

    for component in cal.walk():
        if component.name == "VEVENT":
            dtstart = get_event_datetime(component, 'DTSTART')
            dtend = get_event_datetime(component, 'DTEND')
            summary = str(component.get('SUMMARY', ''))
            if dtstart and dtend:
                if (
                    dtstart >= start_date and
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
        if verbose:
            print(f"{date_str} {start_time_str}-{end_time_str} ({duration_str}) - {summary}")

    total_events = len(events)
    total_hours, remainder = divmod(total_duration.total_seconds(), 3600)
    total_minutes = remainder // 60
    print(f"Total events: {total_events}")
    print(f"Total time spent: {int(total_hours)}h {int(total_minutes)}m")

if __name__ == "__main__":
    main()

