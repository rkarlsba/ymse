#!/usr/bin/env python3

import asyncio
from collections import defaultdict, deque
import hashlib
import json
import re
import urllib.parse
import urllib.request

from wbot_secrets import USER_AGENT, GROQ_API_KEY

from meshcore import MeshCore, EventType


HOST = "::1"
PORT = 5234


CHANNELS = {
    0: "Public",
    1: "#bot",
    2: "#roytest",
    3: "#test",
    4: "#goteborg",
    5: "#lillemesh",
    6: "#norge",
    7: "#3d",
    8: "#3dprinting",
}


# ----------------------------------------------------------------------
# KI
# ----------------------------------------------------------------------

AI_URL = "https://api.openai.com/v1/chat/completions"
AI_MODEL = "gpt-5-mini"

# Kort er bra på LoRa.
AI_MAX_REPLY_CHARS = 180

# Hvor mange tidligere meldinger per kanal KI-en får se.
AI_CONTEXT_MESSAGES = 6


# wbot er det nye navnet, men jallabot skal fortsatt fungere.
#
# Treffer f.eks.:
#
#   @wbot hva mener du?
#   wbot, hva mener du?
#   wbot: hva mener du?
#   wbot hva mener du?
#   @jallabot hva mener du?
#   jallabot, hva mener du?
#
# Treffer ikke:
#
#   jeg tror wbot er full
#   det der var jallabot sin feil

BOT_MENTION_RE = re.compile(
    r"^\s*@?(?:wbot|jallabot)\b"
    r"(?:\s*[:,;-]\s*|\s+)"
    r"(.+?)\s*$",
    re.IGNORECASE,
)


AI_SYSTEM_PROMPT = """
Du er wbot, tidligere kjent som jallabot, på et MeshCore LoRa-nettverk.

Du svarer når noen henvender seg direkte til wbot eller jallabot.

Personlighet:
- Du er lakonisk, tørr, ironisk og sarkastisk.
- Du kan være småfrekk og lettere fornærmende.
- Du kan erte brukeren når det passer naturlig.
- Ikke vær hatefull, truende eller grovt nedsettende.
- Bruk tidligere meldinger til å forstå konteksten.
- Lag en relevant replikk til det som faktisk blir sagt.
- Spill gjerne videre på vær, stedsnavn, dårlige valg og det som nettopp ble sagt.
- Understatement er bedre enn overdreven humor.
- Ikke prøv for hardt å være morsom.
- Ikke forklar vitsen.
- Ikke bruk markdown.
- Unngå emoji med mindre det faktisk gjør svaret bedre.
- Ikke si at du er en KI eller språkmodell med mindre du blir spurt direkte.
- Ikke finn på faktiske opplysninger bare for å få til en vits.
- Svar på samme språk som personen som henvender seg til deg.

VIKTIG:
Dette går over LoRa. Airtime er dyrt.
Svar helst med én kort setning.
Helst under 120 tegn og aldri mer enn 180 tegn.

Eksempel:
Værmelding: "Stokke: 5.6 °C, pissregn (eller Bergen), 1 mm neste timen."
Bruker: 'Derfor bare "morgen" fra Stokke.'
Svar: "Ja, men du er på feil sted!"
""".strip()


# Litt korttidshukommelse per kanal.
#
# Innhold:
#
# {
#     3: deque([
#         {"sender": "Benign", "text": ",vær Stokke"},
#         {"sender": "wbot", "text": "Stokke: ..."},
#     ])
# }

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
# MeshCore-kanaler
# ----------------------------------------------------------------------

def hashtag_secret(name: str) -> bytes:
    return hashlib.sha256(name.encode("utf-8")).digest()[:16]


# ----------------------------------------------------------------------
# Kommandoer
# ----------------------------------------------------------------------

def parse_command(message: str):
    """
    Bare komma som prefiks aktiverer kommandoer.

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
# Direkte tiltale til wbot / jallabot
# ----------------------------------------------------------------------

def bot_mention(message: str):
    match = BOT_MENTION_RE.match(message)

    if not match:
        return None

    return match.group(1).strip()


# ----------------------------------------------------------------------
# KI
# ----------------------------------------------------------------------

def ask_ai_sync(sender: str, message: str, history):
    messages = [
        {
            "role": "system",
            "content": AI_SYSTEM_PROMPT,
        }
    ]

    if history:
        context = "\n".join(
            f"{item['sender']}: {item['text']}"
            for item in history
        )

        prompt = (
            "Nylig samtale på MeshCore-kanalen:\n"
            f"{context}\n\n"
            f"{sender} henvender seg nå

    else:
        prompt = (
            f"{sender} henvender seg direkte til deg:\n"
            f"{message}"
        )

    messages.append(
        {
            "role": "user",
            "content": prompt,
        }
    )

    body = json.dumps(
        {
            "model": AI_MODEL,
            "messages": messages,
        }
    ).encode("utf-8")

    request = urllib.request.Request(
        AI_URL,
        data=body,
        headers={
            "Authorization": f"Bearer {GROQ_API_KEY}",
            "Content-Type": "application/json",
        },
        method="POST",
    )

    with urllib.request.urlopen(
        request,
        timeout=20,
    ) as response:
        data = json.load(response)

    answer = (
        data["choices"][0]["message"]["content"]
        .strip()
        .replace("\n", " ")
    )

    # Ikke la språkmodellen holde foredrag over LoRa.

    if len(answer) > AI_MAX_REPLY_CHARS:
        shortened = answer[:AI_MAX_REPLY_CHARS]

        if " " in shortened:
            shortened = shortened.rsplit(" ", 1)[0]

        answer = shortened.rstrip()

    return answer


async def ask_ai(sender: str, message: str, history):
    try:
        return await asyncio.to_thread(
            ask_ai_sync,
            sender,
            message,
            history,
        )

    except Exception as exc:
        print(f"KI-kall feilet: {exc}")
        return None


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

    with urllib.request.urlopen(
        request,
        timeout=10,
    ) as response:
        result = json.load(response)

    if not result:
        return None

    item = result[0]

    address = item.get("address", {})
    country = address.get("country", "")

    # Bruk navnet Nominatim returnerer.
    # Dermed kan "reykjavik" bli "Reykjaví

    request = urllib.request.Request(
        url,
        headers={
            "User-Agent": USER_AGENT,
            "Accept": "application/json",
        },
    )

    with urllib.request.urlopen(
        request,
        timeout=10,
    ) as response:
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
                bits.append(
                    f"{precipitation:g} mm den neste timen"
                )

            if wind is not None:
                bits.append(
                    f"vind {wind:g} m/s"
                )

        case "sv":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm nästa timme"
                )

            if wind is not None:
                bits.append(
                    f"vind {wind:g} m/s"
                )

        case "da":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm den næste time"
                )

            if wind is not None:
                bits.append(
                    f"vind {wind:g} m/s"
                )

        case "is":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm næstu klukkustund"
                )

            if wind is not None:
                bits.append(
                    f"vindur {wind:g} m/s"
                )

        case "sco":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm in the neist oor"
                )

            if wind is not None:
                bits.append(
                    f"wind {wind:g} m/s"
                )

        case "de":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm in der nächsten Stunde"
                )

            if wind is not None:
                bits.append(
                    f"Wind {wind:g} m/s"
                )

        case "nl":
            if precipitation is not None:
                bits.append(
                    f"{precipitation:g} mm in het komende uur"
                )

            if wind is not None:
                bits.append(
                    f"wind {wind:g} m/s"
                )

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
# wbot
# ----------------------------------------------------------------------

async def main():
    print(
        f"Kobler til openHop på "
        f"[{HOST}]:{PORT} ..."
    )

    mc = await MeshCore.create_tcp(
        HOST,
        PORT,
    )

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

    # ------------------------------------------------------------------
    # Send svar
    # ------------------------------------------------------------------

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

            # Våre egne svar blir også del av korttidshukommelsen.
            # Dermed kan KI-en se f.eks. værmeldingen wbot akkurat sendte.

            channel_history[channel_idx].append(
                {
                    "sender": "wbot",
                    "text": text,
                }
            )

    # ------------------------------------------------------------------
    # Innkommende kanalmeldinger
    # ------------------------------------------------------------------

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

        # Ta kopi av historikken før den nye meldingen legges inn.
        # Dette er konteksten KI-en får.

        history_before = list(
            channel_history[channel_idx]
        )

        # Husk alle vanlige meldinger, ikke bare kommandoer.
        # Det gjør at wbot kan forstå hva folk nettopp snakket om.

        channel_history[channel_idx].append(
            {
                "sender": sender,
                "text": message,
            }
        )

        # --------------------------------------------------------------
        # Direkte tiltale til wbot / jallabot
        # --------------------------------------------------------------

        mention = bot_mention(message)

        if mention is not None:

            print(
                f"RX {CHANNELS[channel_idx]} "
                f"{sender!r} -> wbot: "
                f"{mention!r}"
            )

            answer = await ask_ai(
                sender,
                mention,
                history_before,
            )

            # Hvis KI/API feiler, hold kjeft på meshet.

            if answer:
                await reply(
                    channel_idx,
                    answer,
                )

            return

        # --------------------------------------------------------------
        # Vanlige komma-kommandoer
        # --------------------------------------------------------------

        command = parse_command(message)

        # Ikke komma-prefiks og ikke tiltale til oss.
        # Hold kjeft.

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
                    "JALLA! JALLA!",
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
                    ",ping ,pang ,vær <sted>",
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

