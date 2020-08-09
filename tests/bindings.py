#!/usr/bin/env python3
import connect
from xamalk import logger
from xamalk.events import Keys, EventType, EventsManager

def example_callback(string):
    print(f"{string}")

eventStack = EventsManager()
eventStack.create_events(4)

eventStack.bind_event([Keys.X], EventType.Press, example_callback, "'X' was pressed")
eventStack.bind_event([Keys.X], EventType.Release, example_callback, "'X' was released")
eventStack.bind_event([Keys.CTRL, Keys.TYPEWRITER_ADD], EventType.Press, example_callback, "'CTRL' + '+' was pressed")
eventStack.bind_event([Keys.CTRL, Keys.TYPEWRITER_ADD], EventType.Release, example_callback, "'CTRL' + '+' was released")

eventStack.mainloop([Keys.Q])
del eventStack.events

logger.info("Bindings test was succesful")
