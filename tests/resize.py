#!/usr/bin/env python3
import connect
from time import sleep
from xamalk import logger
from xamalk.renderer import color_print, get_terminal_size, clear_screen

try:
    while True:
        color_print(text=str(get_terminal_size()), end="")
        sleep(0.3)
        clear_screen()
except KeyboardInterrupt:
    pass

logger.info("Resize test was succesful")
