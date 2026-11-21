import secrets
import time
from datetime import datetime, timezone

from arduino.app_utils import App, Bridge

print("RF24Ethernet MQTT BearSSL: serving time and random numbers to the MCU")


def get_time():
    """Unix epoch seconds (UTC) from the Linux clock."""
    return int(time.time())


def get_datetime():
    """Human-readable UTC date/time, for logging on the MCU."""
    return datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")


def get_random():
    """32-bit random number from the kernel CSPRNG (/dev/urandom)."""
    return secrets.randbits(32)


Bridge.provide("get_time", get_time)
Bridge.provide("get_datetime", get_datetime)
Bridge.provide("get_random", get_random)

App.run()
