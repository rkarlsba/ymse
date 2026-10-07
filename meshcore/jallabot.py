#!/usr/bin/env python3

import asyncio
import hashlib
import json
import re
import urllib.parse
import urllib.request
import jallabot_secrets

from meshcore import MeshCore, EventType


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
        "nn": "litt skya",
        "no": "litt skyet",
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
        "nn": "høljeregn (eller Bergen)",
        "no": "høljeregn (eller Bergen)",
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
    # Dermed kan "reykjavik" bli "Reykjavík".

    display_name = (
        item.get("name")
        or item.get("display_name", "").split(",")[0]
        or place
    )

    print(f"Nominatim: {display_name}, {country}")

    return (
        float(item["lat"]),
        float(item["lon"]),
        display_name,
        country,
    )


async def geocode_place(place: str):
    try:
        return await asyncio.to_thread(
            geocode_place_sync,
            place,
        )

    except Exception as exc:
        print(f"Geokoding feilet for {place!r}: {exc}")
        return None


# ----------------------------------------------------------------------
# MET Locationforecast
# ----------------------------------------------------------------------

def fetch_weather_sync(lat: float, lon: float):
    url = (
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

    return f"{place}: " + ", ".join(bits) + ". Yr/MET."


async def get_weather(place: str, language: str):
    location = await geocode_place(place)

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
                f"TX-feil på "
                f"{CHANNELS[channel_idx]}: "
                f"{result.payload}"
            )
        else:
            print(
                f"TX {CHANNELS[channel_idx]}: "
                f"{text}"
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
        # Vær
        # --------------------------------------------------------------

        (
            wants_weather,
            place,
            language,
        ) = weather_request(command)

        if wants_weather:

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

