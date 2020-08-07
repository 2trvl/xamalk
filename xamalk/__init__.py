#!/usr/bin/env python3
import os
import logging
import platform

def create_logger():
    logger = logging.getLogger("AdminPanel")
    logger.setLevel(logging.INFO)

    fileHandler = os.path.realpath(__file__)
    fileHandler = os.path.dirname(fileHandler)
    fileHandler = os.path.split(fileHandler)[0]
    fileHandler = os.path.join(fileHandler, "runtime.log")
    fileHandler = logging.FileHandler(fileHandler, "w", "utf-8")
    formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
    fileHandler.setFormatter(formatter)
    logger.addHandler(fileHandler)
    
    return logger

def init():
    logger.info("Log created")
    logger.info(f"OS: {HOST_OS}")
    renderer.load_dll(HOST_OS)
    logger.info(f"Library: {renderer.DLL}")


logger = create_logger()
HOST_OS = platform.system()

from . import renderer

init()
logger.info("Init executed, tui imported")
