#!/usr/bin/env python3
import os
import sys

folder = os.path.realpath(__file__)
folder = os.path.dirname(folder)
folder = os.path.split(folder)[0]
folder = os.path.join(folder)
sys.path.insert(1, folder)
