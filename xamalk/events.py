#!/usr/bin/env python3
import os
import ctypes

DLL = None

from . import logger


def load_dll(HOST_OS, rootPath):
    global DLL
    DLL = os.path.join(rootPath, "libs", "events")
    if HOST_OS == "Windows":
        DLL = os.path.join(DLL, "events.dll")
    else:
        DLL = os.path.join(DLL, "libevents.so")
    DLL = ctypes.cdll.LoadLibrary(DLL)


class EventType():
    '''
    Enum _EventType from events.h

    '''
    Press = 0
    Release = 1


class Shortcut(ctypes.Structure):
    '''
    Structure _Shortcut from events.h

    '''
    _fields_ = [
        ("amount", ctypes.c_ubyte),
        ("virtualKeys", ctypes.POINTER(ctypes.c_ubyte))]


class Event():
    '''
    Structure _Event from events.h

    _fields_ = [
        ("type", ctypes.c_uint),
        ("condition", Shortcut),
        ("actionArguments", ctypes.c_void_p),
        ("action", ctypes.POINTER(ctypes.CFUNCTYPE(None, ctypes.c_void_p)))]

    '''
    def __init__(self, **kwargs):
        for key, value in kwargs.items():
            setattr(self, key, value)


class States(ctypes.Structure):
    '''
    Structure _States from events.h

    '''
    _fields_ = [
        ("status", ctypes.POINTER(ctypes.c_bool)),
        ("amount", ctypes.c_ubyte),
        ("reading", ctypes.c_ubyte),
        ("lastPressedVirtualKeys", ctypes.POINTER(ctypes.POINTER(ctypes.c_bool)))]


class Keys():
    '''
    Enum _Key from events.h

    '''
    BACKSPACE = 0
    TAB = 1
    TYPEWRITER_ENTER = 2
    NUMPAD_ENTER = 3
    LEFT_SHIFT = 4
    RIGHT_SHIFT = 5
    LEFT_CTRL = 6
    RIGHT_CTRL = 7
    LEFT_ALT = 8
    RIGHT_ALT = 9
    PAUSE = 10
    CAPS_LOCK = 11
    ESCAPE = 12
    SPACE = 13
    PAGE_UP = 14
    PAGE_DOWN = 15
    END = 16
    HOME = 17
    LEFT_ARROW = 18
    UP_ARROW = 19
    RIGHT_ARROW = 20
    DOWN_ARROW = 21
    PRINT_SCREEN = 22
    INSERT = 23
    DEL = 24
    TYPEWRITER_0 = 25
    TYPEWRITER_1 = 26
    TYPEWRITER_2 = 27
    TYPEWRITER_3 = 28
    TYPEWRITER_4 = 29
    TYPEWRITER_5 = 30
    TYPEWRITER_6 = 31
    TYPEWRITER_7 = 32
    TYPEWRITER_8 = 33
    TYPEWRITER_9 = 34
    A = 35
    B = 36
    C = 37
    D = 38
    E = 39
    F = 40
    G = 41
    H = 42
    I = 43
    J = 44
    K = 45
    L = 46
    M = 47
    N = 48
    O = 49
    P = 50
    Q = 51
    R = 52
    S = 53
    T = 54
    U = 55
    V = 56
    W = 57
    X = 58
    Y = 59
    Z = 60
    NUMPAD_0 = 61
    NUMPAD_1 = 62
    NUMPAD_2 = 63
    NUMPAD_3 = 64
    NUMPAD_4 = 65
    NUMPAD_5 = 66
    NUMPAD_6 = 67
    NUMPAD_7 = 68
    NUMPAD_8 = 69
    NUMPAD_9 = 70
    NUMPAD_MULTIPLY = 71
    NUMPAD_ADD = 72
    NUMPAD_SUBTRACT = 73
    NUMPAD_DOT = 74
    NUMPAD_DIVIDE = 75
    F1 = 76
    F2 = 77
    F3 = 78
    F4 = 79
    F5 = 80
    F6 = 81
    F7 = 82
    F8 = 83
    F9 = 84
    F10 = 85
    F11 = 86
    F12 = 87
    NUM_LOCK = 88
    TYPEWRITER_ADD = 89
    COMMA = 90
    TYPEWRITER_SUBTRACT = 91
    TYPEWRITER_DOT = 92
    TILDE = 93


def set_states_reading_flag(reading):
    '''
    This flag changes by the EventsManager class 
    to avoid conflicts with the dll entry point thread

    '''
    DLL.set_states_reading_flag.argtypes = [ctypes.c_ubyte]
    DLL.set_states_reading_flag(reading)
    logger.info(f"eventsStates.reading flag set to {reading}")


def set_events_listener_flag(globally):
    '''
    This flag indicates how you want to listen for hotkeys.
    
    *  True - listen for all hotkeys in your OS
    *  False - listen for hotkey pressed only within the active console window

    P.S. False is a default value

    '''
    DLL.set_events_listener_flag.argtypes = [ctypes.c_bool]
    DLL.set_events_listener_flag(globally)
    logger.info(f"getEventsGlobally flag set to {globally}")


def create_events(amount):
    DLL.create_events.argtypes = [ctypes.c_ubyte]
    DLL.create_events(amount)
    logger.info(f"New {amount+1} events was created")


def bind_event(condition, type, action, actionArguments, index):
    DLL.bind_event.argtypes = [ctypes.POINTER(Shortcut),
                               ctypes.c_uint,
                               ctypes.POINTER(ctypes.CFUNCTYPE(None, ctypes.c_void_p)),
                               ctypes.c_void_p,
                               ctypes.c_ubyte]
    DLL.bind_event(condition, type, action, actionArguments, index)
    logger.info(f"Event {index} was binded")


def get_events_state():
    eventsStates = States.in_dll(DLL, "eventsStates")
    return eventsStates


class EventsManager():
    '''
    Create and manage events simply

    Every event is a shortcut
    On execution of each event, the specified function is called, which can be passed arguments
    
    The event can be one of the following:
    1. button pressed (for short and cyclical actions)
    2. button is released (for long and rare actions, because the action will only be executed when one of the hotkeys is released)

    When you bind an event you must specify:
    *  virtualKeys - hotkey set (type: <class 'list'> of <class 'int'> from <class 'Keys'>)
    *  type - event type Press/Release (type: <class 'int'> from <class 'EventType'>)
    *  action - function to be called when the event is executed (type: <class 'function'>)
    
    And you can specify:
    *  actionArguments - args for calling action (type: <any>)

    P.S. if actionArguments == None, the function hasn't arguments, it's a default value

    '''
    def __init__(self):
        self.__index = None
        self.__events = []
        self.__actions = []
        self.__amount = None
        self.__mainloopKey = None
        self.__eventsStates = None

    def create_events(self, amount):
        self.__index = 0
        self.__events = []
        self.__actions = []
        self.__amount = amount + 1
        self.__mainloopKey = True
        self.__eventsStates = get_events_state()
        create_events(amount)

    def bind_event(self, virtualKeys, type, action, actionArguments=None):
        index = self.__index
        self.__index = (self.__index + 1) % self.__amount

        shortcut = Shortcut(amount=len(virtualKeys),
                            virtualKeys=self.__ubyte_array_ptr__(virtualKeys))
        event = Event(type=type,
                      condition=shortcut,
                      actionArguments=self.__null_ptr__(),
                      action=self.__null_function_ptr__())

        eventWithIndex = (index, event)
        self.__events.append(eventWithIndex)

        actionWithArgs = (action, actionArguments)
        self.__actions.append(actionWithArgs)

    def __null_ptr__(self):
        nullPtr = ctypes.c_int(0)
        nullPtr = ctypes.byref(nullPtr)
        nullPtr = ctypes.cast(nullPtr, ctypes.c_void_p)
        return nullPtr

    def __ubyte_array_ptr__(self, virtualKeys):
        ubyteArrayPtr = (ctypes.c_ubyte * len(virtualKeys))(*virtualKeys)
        ubyteArrayPtr = ctypes.cast(ubyteArrayPtr, ctypes.POINTER(ctypes.c_ubyte))
        return ubyteArrayPtr

    def __null_function_ptr__(self):
        nullFunctionPtr = ctypes.CFUNCTYPE(None, ctypes.c_void_p)
        nullFunctionPtr = nullFunctionPtr(lambda data: None)
        nullFunctionPtr = ctypes.byref(nullFunctionPtr)
        return nullFunctionPtr
    
    @property
    def events(self):
        return self.__events, self.__actions
    
    @events.deleter
    def events(self):
        self.__index = None
        self.__events = []
        self.__actions = []
        self.__amount = None
        self.__mainloopKey = None
        self.__eventsStates = None
        logger.info("Events are set by default")
    
    def mainloop(self, virtualKeys):
        self.bind_event(virtualKeys, EventType.Release, self.__exit_mainloop__, None)
        for index, event in self.__events:
            bind_event(event.condition, 
                       event.type, 
                       event.action, 
                       event.actionArguments, 
                       index)
        
        ##  to start get_events_state function in thread (from events/dllmain.c)
        set_states_reading_flag(self.__eventsStates.reading - 1)
        logger.info("Mainloop started")

        while self.__mainloopKey:
            self.__execute_actions__()

    def __exit_mainloop__(self):
        self.__mainloopKey = False
        logger.info("Mainloop finished")

    def __execute_actions__(self):
        for index in range(self.__amount):
            if self.__eventsStates.status[index] and self.__actions[index][1] != None:
                self.__actions[index][0](self.__actions[index][1])
            elif self.__eventsStates.status[index]:
                self.__actions[index][0]()
