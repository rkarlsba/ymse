#!/usr/bin/env python3

import asyncio
from collections import defaultdict, deque
from datetime import date, datetime, time, timedelta
import hashlib
import json
import re
import urllib.parse
import urllib.request
from zoneinfo import ZoneInfo

from grefsenbot_secrets import USER_AGENT
from meshcore import MeshCore, EventType


HOST = "::1"
PORT = 5234
LOCAL_TIMEZONE = ZoneInfo("Europe/Oslo")


CHANNELS = {
    0: "Public",
    1: "#bot",
    2: "#roytest",
    3: "#test",
    4: "#goteborg",
    5: "#lillemesh",
    6: "#norge",
    7: "#oslo",
    8: "#3dprinting",
}


# ----------------------------------------------------------------------
# Korttidshukommelse
# ----------------------------------------------------------------------

AI_CONTEXT_MESSAGES = 6

channel_history = defaultdict(
    lambda: deque(maxlen=AI_CONTEXT_MESSAGES)
)


# ----------------------------------------------------------------------
# Språk
# ----------------------------------------------------------------------

WEATHER_WORDS = {
    # Generell
    "v": "en",

    # Nynorsk, konservativ
    "ver": "nn",
    "vêr": "nn",
    "veret": "nn",
    "vêret": "nn",

    # Norsk bokmål
    "vær": "no",
    "været": "no",

    # Svensk
    "väder": "sv",
    "vädret": "sv",

    # Dansk
    "vejr": "da",
    "vejret": "da",

    # Islandsk
    "veður": "is",
    "veðrið": "is",

    # Engelsk
    "weather": "en",

    # Scots
    "wather": "sco",
    "wether": "sco",

    # Tysk
    "wetter": "de",

    # Nederlandsk
    "weer": "nl",

    # Polsk
    "pogoda": "pl",
    "pogodę": "pl",

    # Finsk
    "saa": "fi",
    "saatila": "fi",
    "sää": "fi",
    "säätila": "fi",
}


NO_LOCATION = {
    "nn": "Det kjem heilt an på kvar du er.",
    "no": "Det avhenger helt av hvor du er.",
    "sv": "Det beror helt på var du är.",
    "da": "Det afhænger helt af, hvor du er.",
    "is": "Það fer algjörlega eftir því hvar þú ert.",
    "en": "That depends entirely on where you are.",
    "sco": "That aw depends whaur ye are.",
    "de": "Das hängt ganz davon ab, wo du bist.",
    "nl": "Dat hangt er helemaal vanaf waar je bent.",
    "pl": "To zależy całkowicie od tego, gdzie jesteś.",
    "fi": "Se riippuu täysin siitä, missä olet.",
}


WEATHER_SYMBOLS = {
    "clearsky": {
        "nn": "klårt",
        "no": "klart",
        "sv": "klart",
        "da": "klart",
        "is": "heiðskírt",
        "en": "clear",
        "sco": "clear",
        "de": "klar",
        "nl": "helder",
        "pl": "bezchmurnie",
        "fi": "selkeää",
    },

    "fair": {
        "nn": "fint vêr",
        "no": "fint vær",
        "sv": "fint väder",
        "da": "fint vejr",
        "is": "gott veður",
        "en": "nice weather",
        "sco": "bonnie weather",
        "de": "schönes Wetter",
        "nl": "mooi weer",
        "pl": "ładna pogoda",
        "fi": "hyvä sää",
    },

    "partlycloudy": {
        "nn": "delvis skya",
        "no": "delvis skyet",
        "sv": "lite molnigt",
        "da": "lidt skyet",
        "is": "smáskýjað",
        "en": "a bit cloudy",
        "sco": "a wee bit cloodie",
        "de": "etwas bewölkt",
        "nl": "een beetje bewolkt",
        "pl": "trochę pochmurno",
        "fi": "vähän pilvistä",
    },

    "cloudy": {
        "nn": "overskya",
        "no": "overskyet",
        "sv": "mulet",
        "da": "overskyet",
        "is": "skýjað",
        "en": "cloudy",
        "sco": "cloodie",
        "de": "bewölkt",
        "nl": "bewolkt",
        "pl": "pochmurno",
        "fi": "pilvistä",
    },

    "fog": {
        "nn": "skodde",
        "no": "tåke",
        "sv": "dimma",
        "da": "tåge",
        "is": "þoka",
        "en": "foggy",
        "sco": "haar",
        "de": "neblig",
        "nl": "mistig",
        "pl": "mglisto",
        "fi": "sumuista",
    },

    "rain": {
        "nn": "regnvêr",
        "no": "regnvær",
        "sv": "regnigt",
        "da": "regnvejr",
        "is": "rigning",
        "en": "rainy",
        "sco": "rainy",
        "de": "regnerisch",
        "nl": "regenachtig",
        "pl": "deszczowo",
        "fi": "sateista",
    },

    "lightrain": {
        "nn": "småregn",
        "no": "småregn",
        "sv": "småregn",
        "da": "småregn",
        "is": "smárigning",
        "en": "a bit of rain",
        "sco": "a wee bit o rain",
        "de": "ein bisschen Regen",
        "nl": "een beetje regen",
        "pl": "trochę deszczu",
        "fi": "vähän sadetta",
    },

    "heavyrain": {
        "nn": "pissregn (eller Bergen)",
        "no": "pissregn (eller Bergen)",
        "sv": "ösregn (eller Bergen)",
        "da": "øsregn (eller Bergen)",
        "is": "hellirigning (eða Bergen)",
        "en": "pouring rain (or Bergen)",
        "sco": "pishin doon (or Bergen)",
        "de": "strömender Regen (oder Bergen)",
        "nl": "stortregen (of Bergen)",
        "pl": "ulewa (albo Bergen)",
        "fi": "kaatosade (tai Bergen)",
    },

    "sleet": {
        "nn": "sludd",
        "no": "sludd",
        "sv": "snöblandat regn",
        "da": "slud",
        "is": "slydda",
        "en": "sleet",
        "sco": "sleet",
        "de": "Schneeregen",
        "nl": "natte sneeuw",
        "pl": "deszcz ze śniegiem",
        "fi": "räntää",
    },

    "snow": {
        "nn": "snø",
        "no": "snø",
        "sv": "snö",
        "da": "sne",
        "is": "snjór",
        "en": "snow",
        "sco": "snaw",
        "de": "Schnee",
        "nl": "sneeuw",
        "pl": "śnieg",
        "fi": "lunta",
    },
}


# ----------------------------------------------------------------------
# Stedsvise visdomsord
# ----------------------------------------------------------------------

PLACE_QUIPS = {
    "drammen": [
        "Bedre med en dram i timen enn en time i Drammen.",
    ],
    "bergen": [
        "Paraply er bekledning, ikke tilbehør.",
    ],
    "stokke": [
        "Du kunne jo valgt et bedre sted.",
    ],
    "kongsvinger": [
        "Flytt til byen!",
    ],
}


def place_quip(place: str):
    if not place:
        return None

    key = place.strip().casefold()
    quips = PLACE_QUIPS.get(key)

    if quips:
        return quips[0]

    first = re.split(r"[,\s]+", key, maxsplit=1)[0]
    quips = PLACE_QUIPS.get(first)

    if not quips:
        return None

    return quips[0]


# ----------------------------------------------------------------------
# Spark / kick
# ----------------------------------------------------------------------

KICK_WORDS = {
    "spark": "no",
    "spark-ut": "nn",
    "sparka": "sv",
    "los": "da",
    "sparkaðu": "is",
    "kick": "en",
    "skelp": "sco",
    "tritt": "de",
    "schop": "nl",
    "kopnij": "pl",
    "potkaise": "fi",
}


KICK_REPLIES = {
    "nn": "Sparkar {target} i ræva. 🥾",
    "no": "Sparker {target} i ræva. 🥾",
    "sv": "Sparkar {target} i röven. 🥾",
    "da": "Sparker {target} bagi. 🥾",
    "is": "Sparka {target} í rassinn. 🥾",
    "en": "Kicks {target} in the arse. 🥾",
    "sco": "Gies {target} a boot up the erse. 🥾",
    "de": "Tritt {target} in den Hintern. 🥾",
    "nl": "Schopt {target} onder z'n kont. 🥾",
    "pl": "Kopie {target} w tyłek. 🥾",
    "fi": "Potkaisee käyttäjää {target} persuksille. 🥾",
}


KICK_MISSING = {
    "nn": "Kven då? Eg er ein bot, ikkje tankelesar.",
    "no": "Hvem da? Jeg er en bot, ikke tankeleser.",
    "sv": "Vem då? Jag är en bot, inte tankeläsare.",
    "da": "Hvem? Jeg er en bot, ikke tankelæser.",
    "is": "Hvern þá? Ég er vélmenni, ekki hugsanalesari.",
    "en": "Who? I'm a bot, not a mind reader.",
    "sco": "Wha? Ah'm a bot, no a mind reader.",
    "de": "Wen denn? Ich bin ein Bot, kein Gedankenleser.",
    "nl": "Wie dan? Ik ben een bot, geen gedachtenlezer.",
    "pl": "Kogo? Jestem botem, nie czytam w myślach.",
    "fi": "Ketä? Olen botti, en ajatustenlukija.",
}


def kick_request(command: str):
    """
    Gjenkjenner sparkekommando og returnerer:
        (is_kick, target, language)
    """

    words = command.strip().split(maxsplit=1)

    if not words:
        return False, None, None

    language = KICK_WORDS.get(words[0].casefold())

    if language is None:
        return False, None, None

    if len(words) == 1:
        return True, None, language

    target = words[1].strip()
    return True, target or None, language


# ----------------------------------------------------------------------
# MeshCore-kanaler
# ----------------------------------------------------------------------

def hashtag_secret(name: str) -> bytes:
    return hashlib.sha256(name.encode("utf-8")).digest()[:16]


# ----------------------------------------------------------------------
# Kommandoer
# ----------------------------------------------------------------------

def parse_command(message: str):
    """Bare komma som prefiks aktiverer kommandoer."""

    message = message.strip()

    if not message.startswith(","):
        return None

    command = message[1:].strip()
    return command or None


# ----------------------------------------------------------------------
# Dato-/dagstolkning for værkommandoer
# ----------------------------------------------------------------------

WEEKDAYS = {
    # Norsk bokmål og nynorsk
    "mandag": 0,
    "måndag": 0,
    "tirsdag": 1,
    "tysdag": 1,
    "onsdag": 2,
    "onsdag": 2,
    "torsdag": 3,
    "fredag": 4,
    "lørdag": 5,
    "laurdag": 5,
    "søndag": 6,
    "sundag": 6,

    # Svensk og dansk
    "tisdag": 1,
    "onsdag": 2,
    "torsdag": 3,
    "lördag": 5,
    "söndag": 6,

    # Engelsk og Scots
    "monday": 0,
    "tuesday": 1,
    "wednesday": 2,
    "thursday": 3,
    "friday": 4,
    "saturday": 5,
    "sunday": 6,

    # Islandsk
    "mánudagur": 0,
    "þriðjudagur": 1,
    "miðvikudagur": 2,
    "fimmtudagur": 3,
    "föstudagur": 4,
    "laugardagur": 5,
    "sunnudagur": 6,

    # Tysk
    "montag": 0,
    "dienstag": 1,
    "mittwoch": 2,
    "donnerstag": 3,
    "freitag": 4,
    "samstag": 5,
    "sonntag": 6,

    # Nederlandsk
    "maandag": 0,
    "dinsdag": 1,
    "woensdag": 2,
    "donderdag": 3,
    "vrijdag": 4,
    "zaterdag": 5,
    "zondag": 6,

    # Polsk
    "poniedziałek": 0,
    "wtorek": 1,
    "środa": 2,
    "czwartek": 3,
    "piątek": 4,
    "sobota": 5,
    "niedziela": 6,

    # Finsk
    "maanantai": 0,
    "tiistai": 1,
    "keskiviikko": 2,
    "torstai": 3,
    "perjantai": 4,
    "lauantai": 5,
    "sunnuntai": 6,
}


def parse_forecast_day(text: str):
    """
    Returnerer (stedsnavn, antall dager fram).

    Eksempler:
        Oslo i dag
        Oslo i morgon
        Oslo i morra
        Oslo om 3 dagar
        Oslo på fredag
        Oslo tomorrow
        Oslo in 3 days
        Oslo jutro
        Oslo huomenna
        Oslo á morgun
    """

    text = text.strip()
    folded = text.casefold()

    today_phrases = (
        "i dag", "idag",                  # norsk, svensk, dansk
        "today",                          # engelsk, Scots
        "heute",                          # tysk
        "vandaag",                        # nederlandsk
        "dzisiaj", "dziś",                # polsk
        "tänään",                         # finsk
        "í dag",                          # islandsk
    )

    for phrase in today_phrases:
        if folded.endswith(phrase):
            return text[:-len(phrase)].strip(" ,"), 0

    tomorrow_phrases = (
        "i morgon", "imorgon",            # nynorsk, svensk
        "i morgen", "i morra",            # bokmål, dansk
        "imorgen", "imorra",
        "tomorrow", "tmrw",               # engelsk, Scots
        "morgen",                         # tysk, nederlandsk
        "jutro",                          # polsk
        "huomenna",                       # finsk
        "á morgun", "a morgun",           # islandsk
    )

    for phrase in tomorrow_phrases:
        if folded.endswith(phrase):
            return text[:-len(phrase)].strip(" ,"), 1

    relative_day_patterns = (
        # Norsk bokmål/nynorsk, svensk, dansk, engelsk, tysk, nederlandsk
        r"\b(?:om|in)\s+(\d+)\s+"
        r"(?:dag|dager|dagar|dagen|dagene|days?|tage|tagen)\s*$",

        # Islandsk: eftir 3 daga
        r"\beftir\s+(\d+)\s+daga\s*$",

        # Polsk: za 3 dni
        r"\bza\s+(\d+)\s+dni\s*$",

        # Finsk: 3 päivän päästä / 3 päivää
        r"\b(\d+)\s+(?:päivän\s+päästä|päivää)\s*$",
    )

    for pattern in relative_day_patterns:
        match = re.search(pattern, folded)
        if match:
            days = int(match.group(1))
            place = text[:match.start()].strip(" ,")
            return place, days

    weekday_names = "|".join(
        re.escape(name)
        for name in sorted(WEEKDAYS, key=len, reverse=True)
    )

    weekday_match = re.search(
        rf"(?:\b(?:på|next|am|kommende|neste|"
        rf"następny|następna|ensi)\s+)?"
        rf"({weekday_names})\s*$",
        folded,
    )

    if weekday_match:
        weekday = WEEKDAYS[weekday_match.group(1)]
        today = datetime.now(LOCAL_TIMEZONE).date().weekday()
        days = (weekday - today) % 7

        # En ukedag uten «i dag» betyr neste forekomst av den dagen.
        if days == 0:
            days = 7

        place = text[:weekday_match.start()].strip(" ,")
        return place, days

    return text, None


# ----------------------------------------------------------------------
# Værkommando
# ----------------------------------------------------------------------

def weather_request(command: str):
    words = command.strip().split()

    if not words:
        return False, None, None, None

    first = words[0].casefold()

    # Enkel form: ,vær Oslo i morgon / ,weather Edinburgh tomorrow
    if first in WEATHER_WORDS:
        language = WEATHER_WORDS[first]
        place, forecast_day = parse_forecast_day(
            " ".join(words[1:]).strip()
        )
        return True, place or None, language, forecast_day

    # Se etter et kjent værord inne i setningen.
    language = None

    for word in words:
        clean = re.sub(
            r"^[^\wæøåäöðþ]+|[^\wæøåäöðþ]+$",
            "",
            word.casefold(),
        )

        if clean in WEATHER_WORDS:
            language = WEATHER_WORDS[clean]
            break

    if language is None:
        return False, None, None, None

    # For eksempel «hvordan er været i Oslo» eller
    # «what is the weather in Edinburgh».
    match = re.search(
        r"\b(?:i|in)\s+(.+)$",
        command,
        re.IGNORECASE,
    )

    if match:
        place, forecast_day = parse_forecast_day(
            match.group(1).strip()
        )
        return True, place or None, language, forecast_day

    return True, None, language, None


# ----------------------------------------------------------------------
# Geokoding via Nominatim
# ----------------------------------------------------------------------

def geocode_place_sync(place: str):
    params = {
        "q": place,
        "format": "jsonv2",
        "limit": 1,
        "addressdetails": 1,
    }

    url = (
        "https://nominatim.openstreetmap.org/search?"
        + urllib.parse.urlencode(params)
    )

    request = urllib.request.Request(
        url,
        headers={
            "User-Agent": USER_AGENT,
            "Accept": "application/json",
        },
    )

    with urllib.request.urlopen(request, timeout=10) as response:
        result = json.load(response)

    if not result:
        return None

    item = result[0]
    address = item.get("address", {})
    country = address.get("country", "")

    display_name = (
        item.get("name")
        or address.get("village")
        or address.get("hamlet")
        or address.get("town")
        or address.get("city")
        or address.get("municipality")
        or place
    )

    lat = float(item["lat"])
    lon = float(item["lon"])

    return lat, lon, display_name, country


async def geocode_place(place: str):
    try:
        return await asyncio.to_thread(geocode_place_sync, place)

    except Exception as exc:
        print(f"Geokoding feilet for {place!r}: {exc}")
        return None


# ----------------------------------------------------------------------
# Vær via MET
# ----------------------------------------------------------------------

def fetch_weather_sync(lat: float, lon: float):
    params = {
        "lat": f"{lat:.4f}",
        "lon": f"{lon:.4f}",
    }

    url = (
        "https://api.met.no/weatherapi/locationforecast/2.0/compact?"
        + urllib.parse.urlencode(params)
    )

    request = urllib.request.Request(
        url,
        headers={
            "User-Agent": USER_AGENT,
            "Accept": "application/json",
        },
    )

    with urllib.request.urlopen(request, timeout=10) as response:
        return json.load(response)


def clean_symbol(symbol: str):
    for suffix in ("_day", "_night", "_polartwilight"):
        if symbol.endswith(suffix):
            return symbol.removesuffix(suffix)

    return symbol


def translate_symbol(symbol: str, language: str):
    symbol = clean_symbol(symbol)
    translations = WEATHER_SYMBOLS.get(symbol)

    if not translations:
        return None

    return translations.get(language, translations.get("en"))


# ----------------------------------------------------------------------
# Formatering av vær
# ----------------------------------------------------------------------

def format_weather(
    place,
    language,
    temperature,
    condition,
    precipitation,
    wind,
):
    bits = [f"{temperature:g} °C"]

    if condition:
        bits.append(condition)

    match language:
        case "nn" | "no":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm den neste timen")
            if wind is not None:
                bits.append(f"vind {wind:g} m/s")

        case "sv":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm nästa timme")
            if wind is not None:
                bits.append(f"vind {wind:g} m/s")

        case "da":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm den næste time")
            if wind is not None:
                bits.append(f"vind {wind:g} m/s")

        case "is":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm næstu klukkustund")
            if wind is not None:
                bits.append(f"vindur {wind:g} m/s")

        case "sco":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm in the neist oor")
            if wind is not None:
                bits.append(f"wind {wind:g} m/s")

        case "de":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm in der nächsten Stunde")
            if wind is not None:
                bits.append(f"Wind {wind:g} m/s")

        case "nl":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm in het komende uur")
            if wind is not None:
                bits.append(f"wind {wind:g} m/s")

        case "pl":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm w ciągu najbliższej godziny"
                )
            if wind is not None:
                bits.append(f"wiatr {wind:g} m/s")

        case "fi":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm seuraavan tunnin aikana"
                )
            if wind is not None:
                bits.append(f"tuuli {wind:g} m/s")

        case _:
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm in the next hour")
            if wind is not None:
                bits.append(f"wind {wind:g} m/s")

    return f"{place}: " + ", ".join(bits)


def local_entry_date(entry):
    timestamp = datetime.fromisoformat(
        entry["time"].replace("Z", "+00:00")
    )
    return timestamp.astimezone(LOCAL_TIMEZONE).date()


def format_daily_forecast(place, target_date, entries, language):
    if not entries:
        return None

    noon = datetime.combine(
        target_date,
        time(12, 0),
        tzinfo=LOCAL_TIMEZONE,
    )

    def local_entry_time(entry):
        return datetime.fromisoformat(
            entry["time"].replace("Z", "+00:00")
        ).astimezone(LOCAL_TIMEZONE)

    chosen = min(
        entries,
        key=lambda entry: abs(local_entry_time(entry) - noon),
    )

    data = chosen["data"]
    instant = data["instant"]["details"]
    temperature = instant.get("air_temperature")
    wind = instant.get("wind_speed")

    next_6_hours = data.get("next_6_hours", {})
    symbol = next_6_hours.get("summary", {}).get("symbol_code")
    precipitation = next_6_hours.get("details", {}).get(
        "precipitation_amount"
    )

    parts = []

    if temperature is not None:
        parts.append(f"{temperature:g} °C")

    if symbol:
        condition = translate_symbol(symbol, language)
        if condition:
            parts.append(condition)

    if precipitation is not None:
        parts.append(f"{precipitation:g} mm / 6 t")

    if wind is not None:
        parts.append(f"vind {wind:g} m/s")

    if not parts:
        return None

    return f"{place} {target_date.strftime('%d.%m')}: " + ", ".join(parts)


async def get_weather(
    place: str,
    language: str,
    forecast_day=None,
):
    location = await geocode_place(place)

    if location is None:
        return None

    lat, lon, display_name, country = location

    print(
        f"Geokoding: {place!r} -> {display_name!r}, "
        f"{country} ({lat:.4f}, {lon:.4f})"
    )

    try:
        data = await asyncio.to_thread(fetch_weather_sync, lat, lon)
        timeseries = data["properties"]["timeseries"]

        if forecast_day is not None:
            if not 0 <= forecast_day <= 7:
                return None

            target_date = (
                datetime.now(LOCAL_TIMEZONE).date()
                + timedelta(days=forecast_day)
            )

            day_entries = [
                entry
                for entry in timeseries
                if local_entry_date(entry) == target_date
            ]

            daily = format_daily_forecast(
                display_name,
                target_date,
                day_entries,
                language,
            )

            if daily is None:
                return None

            return f"{daily} {country}"

        ts = timeseries[0]
        instant = ts["data"]["instant"]["details"]

        temperature = instant["air_temperature"]
        wind = instant.get("wind_speed")

        next_hour = ts["data"].get("next_1_hours", {})
        symbol = next_hour.get("summary", {}).get("symbol_code")
        precipitation = next_hour.get("details", {}).get(
            "precipitation_amount"
        )

    except Exception as exc:
        print(f"Værdata feilet for {display_name!r}: {exc}")
        return None

    condition = translate_symbol(symbol, language) if symbol else None

    weather = format_weather(
        display_name,
        language,
        temperature,
        condition,
        precipitation,
        wind,
    )

    return f"{weather} {country}, {lat:.4f}, {lon:.4f}"


# ----------------------------------------------------------------------
# grefsenbot
# ----------------------------------------------------------------------

async def main():
    print(f"Kobler til openHop på [{HOST}]:{PORT} ...")

    mc = await MeshCore.create_tcp(HOST, PORT)

    print("Tilkoblet.")

    # Public på slot 0 røres ikke.
    # Hashtag-kanalene konfigureres eksplisitt.
    for channel_idx, channel_name in CHANNELS.items():
        if not channel_name.startswith("#"):
            continue

        result = await mc.commands.set_channel(
            channel_idx,
            channel_name,
            hashtag_secret(channel_name),
        )

        if result.type == EventType.ERROR:
            raise RuntimeError(
                f"Kunne ikke konfigurere "
                f"{channel_name}: {result.payload}"
            )

    print(
        "Lytter diskré på "
        + ", ".join(CHANNELS.values())
        + "."
    )

    async def reply(channel_idx: int, text: str):
        result = await mc.commands.send_chan_msg(
            channel_idx,
            text,
        )

        print(
            f"DEBUG send result: channel={channel_idx}, "
            f"type={result.type!r}, payload={result.payload!r}, "
            f"text={text!r}"
        )

        if result.type == EventType.ERROR:
            print(
                f"TX-feil på {CHANNELS[channel_idx]}: "
                f"{result.payload}"
            )
        else:
            print(f"TX {CHANNELS[channel_idx]}: {text}")
            channel_history[channel_idx].append(
                {
                    "sender": "grefsenbot",
                    "text": text,
                }
            )

    async def on_channel_message(event):
        payload = event.payload

        if not isinstance(payload, dict):
            return

        channel_idx = payload.get("channel_idx")

        if channel_idx not in CHANNELS:
            return

        text = payload.get("text")

        if not text or ": " not in text:
            return

        sender, message = text.split(": ", 1)

        channel_history[channel_idx].append(
            {
                "sender": sender,
                "text": message,
            }
        )

        command = parse_command(message)

        if command is None:
            return

        print(
            f"RX {CHANNELS[channel_idx]} "
            f"{sender!r}: {command!r}"
        )

        # Spark / kick
        is_kick, target, kick_language = kick_request(command)

        if is_kick:
            if target is None:
                await reply(
                    channel_idx,
                    KICK_MISSING.get(
                        kick_language,
                        KICK_MISSING["en"],
                    ),
                )
                return

            template = KICK_REPLIES.get(
                kick_language,
                KICK_REPLIES["en"],
            )

            await reply(
                channel_idx,
                template.format(target=target),
            )
            return

        # Vær
        is_weather, place, language, forecast_day = weather_request(
            command
        )

        if is_weather:
            if place is None:
                await reply(
                    channel_idx,
                    NO_LOCATION.get(language, NO_LOCATION["en"]),
                )
                return

            forecast = await get_weather(
                place,
                language,
                forecast_day,
            )

            if forecast:
                quip = place_quip(place)

                if quip:
                    forecast = f"{forecast} {quip}"

                await reply(channel_idx, forecast)

            return

        # Andre kommandoer
        cmd = command.lower().strip()

        match cmd:
            case "teisen" | "theisen":
                await reply(
                    channel_idx,
                    "https://www.youtube.com/watch?v=cFP86wAvp-8",
                )

            case "ping":
                await reply(channel_idx, "pong")

            case "pang":
                await reply(channel_idx, "🔫💥")

            case "jalla":
                await reply(channel_idx, "JALLA! JALLA! https://www.imdb.com/title/tt0269389/")

            # Norsk
            case "hei" | "heisann" | "hallo":
                await reply(channel_idx, f"Hei {sender}!")

            # Svensk
            case "hej" | "hallå":
                await reply(channel_idx, f"Hej {sender}!")

            # Dansk
            case "dav" | "goddag":
                await reply(channel_idx, f"Dav {sender}!")

            # Islandsk
            case "hæ" | "halló":
                await reply(channel_idx, f"Halló {sender}!")

            # Engelsk
            case "hello" | "hi":
                await reply(channel_idx, f"Hello {sender}!")

            # Scots
            case "hullo":
                await reply(channel_idx, f"Hullo {sender}! Aye.")

            # Tysk
            case "moin" | "guten tag":
                await reply(channel_idx, f"Moin {sender}!")

            # Nederlandsk
            case "hoi" | "goedendag":
                await reply(channel_idx, f"Hoi {sender}!")

            # Polsk
            case "cześć" | "czesc" | "hejka":
                await reply(channel_idx, f"Cześć {sender}!")

            # Finsk
            case "moi" | "terve":
                await reply(channel_idx, f"Moi {sender}!")

            case "help" | "hjelp":
                await reply(
                    channel_idx,
                    ",ping ,pang ,kick <navn> ,spark <navn> ,vær <sted> "
                    "[i dag/i morgon/ukedag/om N dagar]",
                )

            # Ukjent kommando: vær stille.
            case _:
                return

    mc.subscribe(
        EventType.CHANNEL_MSG_RECV,
        on_channel_message,
    )

    await mc.start_auto_message_fetching()

    try:
        while True:
            await asyncio.sleep(3600)

    finally:
        await mc.disconnect()


if __name__ == "__main__":
    try:
        asyncio.run(main())

    except KeyboardInterrupt:
        pass

