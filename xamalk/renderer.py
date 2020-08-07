#!/usr/bin/env python3
import os
import ctypes

DLL = None

from . import logger


def load_dll(HOST_OS):
    global DLL
    DLL = os.path.realpath(__file__)
    DLL = os.path.dirname(DLL)
    DLL = os.path.split(DLL)[0]
    DLL = os.path.join(DLL, "libs", "renderer")
    if HOST_OS == "Windows":
        DLL = os.path.join(DLL, "renderer.dll")
    else:
        DLL = os.path.join(DLL, "librenderer.so")
    DLL = ctypes.cdll.LoadLibrary(DLL)


class ColorCodes():
    RED = 0
    AQUA = 1
    BLUE = 2
    GRAY = 3
    BLACK = 4
    GREEN = 5
    WHITE = 6
    PURPLE = 7
    YELLOW = 8
    NO_COLOR = 14
    LIGHT_RED = 9
    LIGHT_BLUE = 10
    LIGHT_GRAY = 11
    LIGHT_GREEN = 12
    LIGHT_PURPLE = 13


class TerminalSize(ctypes.Structure):
    '''
    Structure for get_terminal_size()

    '''
    _fields_ = [
        ("cols", ctypes.c_ushort),
        ("rows", ctypes.c_ushort)]


class Coords(ctypes.Structure):
    '''
    Structure _Coordinates from renderer.h

    '''
    _fields_ = [
        ("X", ctypes.c_ushort),
        ("Y", ctypes.c_ushort)]


class Particle(ctypes.Structure):
    _fields_ = [
        ("end", ctypes.c_char_p),
        ("text", ctypes.c_char_p),
        ("bgcode", ctypes.c_uint),
        ("fgcode", ctypes.c_uint),
        ("position", ctypes.POINTER(Coords)),
        ("milliseconds", ctypes.c_uint)]

    _defaults_ = {
        "end": "\n".encode(),
        "text": " ".encode(),
        "bgcode": ColorCodes.NO_COLOR,
        "fgcode": ColorCodes.NO_COLOR,
        "position": ctypes.pointer(Coords(0, 0)),
        "milliseconds": 0}

    def __init__(self, index, **kwargs):
        super().__init__(**kwargs)
        logger.info(f"Particle {index} with values {kwargs} has been set")


def clear_screen():
    global DLL
    DLL.clear_screen()
    logger.info("The screen has been cleared")


def get_terminal_size():
    global DLL
    resolution = TerminalSize.in_dll(DLL, "resolution")
    logger.info(f"Current terminal resolution: {resolution.cols}x{resolution.rows}")
    return resolution.cols, resolution.rows


def color_print(text, bgcode=ColorCodes.NO_COLOR, fgcode=ColorCodes.NO_COLOR, end="\n"):
    global DLL
    end = end.encode()
    text = text.encode()
    DLL.color_print.argtypes = [ctypes.c_char_p, ctypes.c_ubyte, ctypes.c_ubyte, ctypes.c_char_p]
    DLL.color_print(text, bgcode, fgcode, end)
    logger.info("Colored string has been output")


def create_particles(amount):
    global DLL
    DLL.create_particles.argtypes = [ctypes.c_uint]
    DLL.create_particles(amount)
    logger.info(f"New frame of {amount} particles was created")


def set_particle(particle, index):
    global DLL, Particle
    DLL.set_particle.argtypes = [ctypes.POINTER(Particle), ctypes.c_uint]
    DLL.set_particle(particle, index)


def render_frame(amount):
    global DLL
    DLL.render_frame.argtypes = [ctypes.c_uint]
    DLL.render_frame(amount)
    logger.info(f"Frame of {amount} particles was rendered")
