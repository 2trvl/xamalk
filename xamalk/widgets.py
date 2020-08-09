#!/usr/bin/env python3
import ctypes
from . import logger
from time import sleep
from .renderer import *


class Drawer():
    '''
    Just lets you draw individual particles

    When you set the particle you can specify:
    *  end - ending of color string (type: <class 'str'>)
    *  text - content of the string (type: <class 'str'>)
    *  bgcode - background color code (type: <class 'int'> from <class 'ColorCodes'>)
    *  fgcode - foreground color code (type: <class 'int'> from <class 'ColorCodes'>)
    *  position - position of color string on the screen (type: <class 'tuple'>)
    *  milliseconds - time of waiting before output the string on the screen (type: <class 'int'>)

    P.S. (0, 0) - position of the upper left corner of the console screen

    '''
    def __init__(self):
        self.__index = None
        self.__amount = None
        self.__particles = []

    def create_particles(self, amount):
        self.__index = 0
        self.__amount = amount
        self.__particles = []
        create_particles(amount)

    def set_particle(self, **kwargs):
        '''
        I made storage in C and my special methods for access to it
        because of ctypes.Structure __init__ call without timeout will result in a Segmentation fault

        '''
        if "index" in kwargs.keys():
            index = kwargs["index"]
            kwargs.pop("index")
        else:
            index = self.__index
            self.__index = (self.__index + 1) % self.__amount
        
        values = Particle._defaults_.copy()

        for key, value in kwargs.items():
            if isinstance(value, str):
                values[key] = value.encode()
                continue

            if isinstance(value, tuple):
                values[key] = ctypes.pointer(Coords(X=value[0], Y=value[1]))
                continue

            values[key] = value
        
        particle = Particle(index, **values)
        particle = ctypes.byref(particle)
        particle = (index, particle)
        self.__particles.append(particle)
    
    @property
    def particles(self):
        return self.__particles

    @particles.deleter
    def particles(self):
        self.__index = None
        self.__amount = None
        self.__particles = []
        logger.info("Particles are set by default")

    def draw(self):
        for index, particle in self.__particles:
            set_particle(particle, index)
        render_frame(self.__amount)

    def clear(self):
        clear_screen()

    def sleep(self, secs):
        sleep(secs)
        logger.info(f"Drawer slept for {secs}s")

    def size(self):
        return get_terminal_size()
