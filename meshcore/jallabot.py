#!/usr/bin/env python3

import asyncio
import hashlib
import json
import re
import urllib.parse
import urllib.request
import jallabot_secrets

from meshcore import MeshCore, EventType

host_identifier = "openhop.karlsbakk.net"


HOST = "::1"
PORT = 5234

CHANNELS = {
    0: "Public",
    1: "#roytest",
    2: "#test",
    3: "#goteborg",
    4: "#lilmesh",
    5: "#norge",
    6: "#3d",
    7: "#3dprinting",
}

USER_AGENT = f"jallabot/0.5 {host_identifier}"


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
        "no": "pent vær",
        "sv": "mest klart",
        "da": "mest klart",
        "is": "bjart",
        "en": "fair",
        "sco": "fair",
        "de": "heiter",
        "nl": "vrij helder",
        "pl": "pogodnie",
        "fi": "melko selkeää",
    },

    "partlycloudy": {
        "nn": "delvis skya",
        "no": "delvis skyet",
        "sv": "halvklart",
        "da": "delvist skyet",
        "is": "hálfskýjað",
        "en": "partly cloudy",
        "sco": "pairtly cloodie",
        "de": "teilweise bewölkt",
        "nl": "gedeeltelijk bewolkt",
        "pl": "częściowe zachmurzenie",
        "fi": "puolipilvistä",
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
        "en": "fog",
        "sco": "haar",
        "de": "Nebel",
        "nl": "mist",
        "pl": "mgła",
        "fi": "sumua",
    },

    "rain": {
        "nn": "regn",
        "no": "regn",
        "sv": "regn",
        "da": "regn",
        "is": "rigning",
        "en": "rain",
        "sco": "rain",
        "de": "Regen",
        "nl": "regen",
        "pl": "deszcz",
        "fi": "sadetta",
    },

    "lightrain": {
        "nn": "lett regn",
        "no": "lett regn",
        "sv": "lätt regn",
        "da": "let regn",
        "is": "lítil rigning",
        "en": "light rain",
        "sco": "licht rain",
        "de": "leichter Regen",
        "nl": "lichte regen",
        "pl": "lekki deszcz",
        "fi": "heikkoa sadetta",
    },

    "heavyrain": {
        "no": "kraftig regn (eller Bergen)",
        "nn": "kraftig regn (eller Bergen)",
        "sv": "kraftigt regn (eller Bergen)",
        "da": "kraftig regn (eller Bergen)",
        "is": "mikil rigning (eða Bergen)",
        "en": "heavy rain (or Bergen)",
        "sco": "gey heavy rain (or Bergen)",
        "de": "starker Regen (oder Bergen)",
        "nl": "zware regen (of Bergen)",
        "pl": "silny deszcz (albo Bergen)",
        "fi": "voimakasta sadetta (tai Bergen)",
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
# MeshCore-kanaler
# ----------------------------------------------------------------------

def hashtag_secret(name: str) -> bytes:
    return hashlib.sha256(name.encode("utf-8")).digest()[:16]


# ----------------------------------------------------------------------
# Kommandoer
# ----------------------------------------------------------------------

def parse_command(message: str):
    """
    Bare komma som prefiks aktiverer boten.

    Eksempler:
        ,ping
        ,pang
        ,vær Oslo
        ,veður reykjavik
        ,weather Edinburgh
        ,wetter Berlin
    """

    message = message.strip()

    if not message.startswith(","):
        return None

    command = message[1:].strip()

    if not command:
        return None

    return command


# ----------------------------------------------------------------------
# Værkommando
# ----------------------------------------------------------------------

def weather_request(command: str):
    words = command.strip().split()

    if not words:
        return False, None, None

    first = words[0].lower()

    # Enkel form:
    #
    # ,vær Oslo
    # ,weather Edinburgh
    # ,veður reykjavik
    # ,wetter Berlin
    # ,weer Amsterdam
    # ,pogoda Warszawa
    # ,sää Helsinki

    if first in WEATHER_WORDS:
        language = WEATHER_WORDS[first]
        place = " ".join(words[1:]).strip()

        return True, place or None, language

    # Se etter et kjent værord inne i setningen.

    language = None

    for word in words:
        clean = re.sub(
            r"^[^\wæøåäöðþ]+|[^\wæøåäöðþ]+$",
            "",
            word.lower(),
        )

        if clean in WEATHER_WORDS:
            language = WEATHER_WORDS[clean]
            break

    if language is None:
        return False, None, None

    # Skandinavisk:
    # hvordan er været i Oslo

    match = re.search(
        r"\bi\s+(.+)$",
        command,
        re.IGNORECASE,
    )

    if match:
        return True, match.group(1).strip(), language

    # Engelsk/Scots:
    # what's the weather in Edinburgh

    match = re.search(
        r"\bin\s+(.+)$",
        command,
        re.IGNORECASE,
    )

    if match:
        return True, match.group(1).strip(), language

    return True, None, language


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

    # Bruk navnet Nominatim returnerer.
    # Dermed kan "reykjavik" bli "Reykjaví
        "https://api.met.no/weatherapi/locationforecast/2.0/compact"
        f"?lat={lat:.4f}&lon={lon:.4f}"
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
    for suffix in (
        "_day",
        "_night",
        "_polartwilight",
    ):
        if symbol.endswith(suffix):
            return symbol.removesuffix(suffix)

    return symbol


def translate_symbol(symbol: str, language: str):
    symbol = clean_symbol(symbol)

    translations = WEATHER_SYMBOLS.get(symbol)

    if not translations:
        return None

    return translations.get(
        language,
        translations.get("en"),
    )


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

        case "nn":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm den neste timen"
                )

            if wind is not None:
                bits.append(
                    f"vind {wind:g} m/s"
                )
        case "no":
            if precipitation is not None:
                bits.append(f"{precipitation:g} mm neste timen")
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
                bits.append(
                    f"{precipitation:g} mm in der nächsten Stunde"
                )
            if wind is not None:
                bits.append(f"Wind {wind:g} m/s")

        case "nl":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm in het komende uur"
                )
            if wind is not None:
                bits.append(f"wind {wind:g} m/s")

        case "pl":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm w ciągu najbliż
    if location is None:
        return None

    lat, lon, display_name, country = location

    print(
        f"Geokoding: {place!r} -> {display_name!r}, "
        f"{country} ({lat:.4f}, {lon:.4f})"
    )

    try:
        data = await asyncio.to_thread(
            fetch_weather_sync,
            lat,
            lon,
        )

        ts = data["properties"]["timeseries"][0]

        instant = ts["data"]["instant"]["details"]

        temperature = instant["air_temperature"]
        wind = instant.get("wind_speed")

        next_hour = ts["data"].get(
            "next_1_hours",
            {},
        )

        symbol = (
            next_hour
            .get("summary", {})
            .get("symbol_code")
        )

        precipitation = (
            next_hour
            .get("details", {})
            .get("precipitation_amount")
        )

    except Exception as exc:
        print(
            f"Værdata feilet for "
            f"{display_name!r}: {exc}"
        )
        return None

    condition = None

    if symbol:
        condition = translate_symbol(
            symbol,
            language,
        )

    weather = format_weather(
        display_name,
        language,
        temperature,
        condition,
        precipitation,
        wind,
    )

    return (
        f"{weather} "
        f"{country}, "
        f"{lat:.4f}, {lon:.4f}"
    )



# ----------------------------------------------------------------------
# Jallabot
# ----------------------------------------------------------------------

async def main():
    print(f"Kobler til openHop på [{HOST}]:{PORT} ...")

    mc = await MeshCore.create_tcp(
        HOST,
        PORT,
    )

    print("Tilkoblet.")

    # Public på slot 0 røres ikke.
    # #roytest og #test konfigureres som hashtag-kanaler.

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

        if result.type == EventType.ERROR:
            print(
                f"TX-feil på
            )

    async def on_channel_message(event):
        payload = event.payload

        if not isinstance(payload, dict):
            return

        channel_idx = payload.get("channel_idx")

        if channel_idx not in CHANNELS:
            return

        text = payload.get("text")

        if not text:
            return

        # openHop/MeshCore leverer f.eks.:
        #
        # Grefsenposten BLE: ,ping

        if ": " not in text:
            return

        sender, message = text.split(
            ": ",
            1,
        )

        command = parse_command(message)

        # Ikke komma-prefiks. Hold kjeft.

        if command is None:
            return

        print(
            f"RX {CHANNELS[channel_idx]} "
            f"{sender!r}: {command!r}"
        )

        # --------------------------------------------------------------
        # Væ

            if place is None:
                await reply(
                    channel_idx,
                    NO_LOCATION.get(
                        language,
                        NO_LOCATION["en"],
                    ),
                )
                return

            forecast = await get_weather(
                place,
                language,
            )

            # Vet vi ikke, sier vi ingenting.

            if forecast:
                await reply(
                    channel_idx,
                    forecast,
                )

            return

        # --------------------------------------------------------------
        # Andre kommandoer
        # --------------------------------------------------------------

        cmd = command.lower().strip()

        match cmd:

            case "ping":
                await reply(
                    channel_idx,
                    "pong",
                )

            case "pang":
                await reply(
                    channel_idx,
                    "🔫💥",
                )

            case "jalla":
                await reply(
                    channel_idx,
                    "JALLA!",
                )

            # Norsk
            case "hei" | "heisann" | "hallo":
                await reply(
                    channel_idx,
                    f"Hei {sender}!",
                )

            # Svensk
            case "hej" | "hallå":
                await reply(
                    channel_idx,
                    f"Hej {sender}!",
                )

            # Dansk
            case "dav" | "goddag":
                await reply(
                    channel_idx,
                    f"Dav {sender}!",
                )

            # Islandsk
            case "hæ" | "halló":
                await reply(
                    channel_idx,
                    f"Halló {sender}!",
                )

            # Engelsk
            case "hello" | "hi":
                await reply(
                    channel_idx,
                    f"Hello {sender}!",
                )

            # Scots
            case "hullo":
                await reply(
                    channel_idx,
                    f"Hullo {sender}! Aye.",
                )

            # Tysk
            case "moin" | "guten tag":
                await reply(
                    channel_idx,
                    f"Moin {sender}!",
                )

            # Nederlandsk
            case "hoi" | "goedendag":
                await reply(
                    channel_idx,
                    f"Hoi {sender}!",
                )

            # Polsk
            case "cześć" | "czesc" | "hejka":
                await reply(
                    channel_idx,
                    f"Cześć {sender}!",
                )

            # Finsk
            case "moi" | "terve":
                await reply(
                    channel_idx,
                    f"Moi {sender}!",
                )

            case "help" | "hjelp":
                await reply(
                    channel_idx,
                    ",ping ,pang ,jalla ,vær <sted>",
                )

            # Ukjent kommando. Vær stille.

            case _:
                return

    mc.subscribe(
        EventType.CHANNEL_MSG_RECV,
        on_channel_message,
    )

    await mc.start_auto_message_fetching()

    # Ingen startup-melding på mesh-et.

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

