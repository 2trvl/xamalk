#!/usr/bin/env python3
import os
import logging
import platform

def get_xamalk_root_path():
    rootPath = os.path.realpath(__file__)
    rootPath = os.path.dirname(rootPath)
    rootPath = os.path.split(rootPath)[0]
    return rootPath

def create_logger(rootPath):
    logger = logging.getLogger("xamalk")
    logger.setLevel(logging.INFO)

    fileHandler = os.path.join(rootPath, "runtime.log")
    fileHandler = logging.FileHandler(fileHandler, "w", "utf-8")
    formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
    fileHandler.setFormatter(formatter)
    logger.addHandler(fileHandler)
    
    return logger

def init():
    logger.info("Log created")
    logger.info(f"OS: {HOST_OS}")

    events.load_dll(HOST_OS, rootPath)
    logger.info(f"Events library: {events.DLL}")
    
    renderer.load_dll(HOST_OS, rootPath)
    logger.info(f"Renderer library: {renderer.DLL}")


HOST_OS = platform.system()
rootPath = get_xamalk_root_path()
logger = create_logger(rootPath)

from . import events
from . import renderer

init()
logger.info("Init executed, xamalk imported")
