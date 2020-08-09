#!/usr/bin/env python3
import ctypes
from xamalk import logger
from xamalk.widgets import Drawer
from xamalk.events import EventsManager, Keys
from xamalk.renderer import ColorCodes, color_print

eventStack = EventsManager()
eventStack.create_events(0)

canvas = Drawer()
canvas.create_particles(4)

canvas.set_particle(text=" "*4, bgcode=ColorCodes.AQUA, position=(3, 2))
canvas.set_particle(text=" "*2, bgcode=ColorCodes.AQUA, position=(3, 3))
canvas.set_particle(text=" "*4, bgcode=ColorCodes.AQUA, position=(3, 4))
canvas.set_particle(end="\n"*3, text=" "*2, bgcode=ColorCodes.GREEN, position=(10, 3), milliseconds=1000)

canvas.draw()

eventStack.mainloop([Keys.Q])
canvas.clear()

color_print(str(canvas.size()))
del canvas.particles
del eventStack.events

canvas.sleep(0.2)
logger.info("Program executed")
