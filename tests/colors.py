#!/usr/bin/env python3
import connect
from xamalk import logger
from xamalk.renderer import ColorCodes, color_print

for index, color in enumerate(dir(ColorCodes)):
    if not color.startswith("__"):
        color_print(text="   ", bgcode=getattr(ColorCodes, color), end=" | ")
        color_print(text=color)

color_print(text="Without any color")
color_print(text="With fg only", fgcode=ColorCodes.GREEN)
color_print(text="With bg and fg", bgcode=ColorCodes.WHITE, fgcode=ColorCodes.BLUE)
logger.info("Colors test was succesful")
