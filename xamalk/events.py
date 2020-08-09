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
        ("reading", ctypes.c_bool),
        ("status", ctypes.POINTER(ctypes.c_bool)),
        ("amount", ctypes.c_ubyte),
        ("lastPressedVirtualKeys", ctypes.POINTER(ctypes.POINTER(ctypes.c_bool)))]


class Keys():
    '''
    Enum _Key from events.h

    '''
    LEFT_MOUSE_BUTTON = 0
    RIGHT_MOUSE_BUTTON = 1
    MIDDLE_MOUSE_BUTTON = 2
    BACKSPACE = 3
    TAB = 4
    ENTER = 5
    SHIFT = 6
    CTRL = 7
    ALT = 8
    PAUSE = 9
    CAPS_LOCK = 10
    ESCAPE = 11
    SPACE = 12
    PAGE_UP = 13
    PAGE_DOWN = 14
    END = 15
    HOME = 16
    LEFT_ARROW = 17
    UP_ARROW = 18
    RIGHT_ARROW = 19
    DOWN_ARROW = 20
    PRINT_SCREEN = 21
    INSERT = 22
    DEL = 23
    TYPEWRITER_0 = 24
    TYPEWRITER_1 = 25
    TYPEWRITER_2 = 26
    TYPEWRITER_3 = 27
    TYPEWRITER_4 = 28
    TYPEWRITER_5 = 29
    TYPEWRITER_6 = 30
    TYPEWRITER_7 = 31
    TYPEWRITER_8 = 32
    TYPEWRITER_9 = 33
    A = 34
    B = 35
    C = 36
    D = 37
    E = 38
    F = 39
    G = 40
    H = 41
    I = 42
    J = 43
    K = 44
    L = 45
    M = 46
    N = 47
    O = 48
    P = 49
    Q = 50
    R = 51
    S = 52
    T = 53
    U = 54
    V = 55
    W = 56
    X = 57
    Y = 58
    Z = 59
    NUMPAD_0 = 60
    NUMPAD_1 = 61
    NUMPAD_2 = 62
    NUMPAD_3 = 63
    NUMPAD_4 = 64
    NUMPAD_5 = 65
    NUMPAD_6 = 66
    NUMPAD_7 = 67
    NUMPAD_8 = 68
    NUMPAD_9 = 69
    NUMPAD_MULTIPLY = 70
    NUMPAD_ADD = 71
    NUMPAD_SEPARATOR = 72
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


def set_states_reading_flag(flag):
    DLL.set_states_reading_flag.argtypes = [ctypes.c_bool]
    DLL.set_states_reading_flag(flag)


def create_events(amount):
    DLL.create_events.argtypes = [ctypes.c_ubyte]
    DLL.create_events(amount)
    logger.info(f"New {amount} events was created")


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
                            virtualKeys=self.__ubyte_array_ptr(virtualKeys))
        event = Event(type=type,
                      condition=shortcut,
                      actionArguments=self.__null_ptr(),
                      action=self.__null_function_ptr())

        eventWithIndex = (index, event)
        self.__events.append(eventWithIndex)

        actionWithArgs = (action, actionArguments)
        self.__actions.append(actionWithArgs)

    def __null_ptr(self):
        nullPtr = ctypes.c_int(0)
        nullPtr = ctypes.byref(nullPtr)
        nullPtr = ctypes.cast(nullPtr, ctypes.c_void_p)
        return nullPtr

    def __ubyte_array_ptr(self, virtualKeys):
        ubyteArrayPtr = (ctypes.c_ubyte * len(virtualKeys))(*virtualKeys)
        ubyteArrayPtr = ctypes.cast(ubyteArrayPtr, ctypes.POINTER(ctypes.c_ubyte))
        return ubyteArrayPtr

    def __null_function(self, data):
        pass

    def __null_function_ptr(self):
        nullFunctionPtr = ctypes.CFUNCTYPE(None, ctypes.c_void_p)
        nullFunctionPtr = nullFunctionPtr(self.__null_function)
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
        self.bind_event(virtualKeys, EventType.Release, self.__exit_mainloop, None)
        for index, event in self.__events:
            bind_event(event.condition, 
                       event.type, 
                       event.action, 
                       event.actionArguments, 
                       index)
        
        ##  to start get_events_state function in thread (from events/dllmain.c)
        set_states_reading_flag(False)

        while self.__mainloopKey:
            self.__execute_actions()

    def __exit_mainloop(self):
        self.__mainloopKey = False

    def __execute_actions(self):
        for index in range(self.__amount):
            if self.__eventsStates.status[index] and self.__actions[index][1] != None:
                self.__actions[index][0](self.__actions[index][1])
            elif self.__eventsStates.status[index]:
                self.__actions[index][0]()
