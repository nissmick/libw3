" $Id$ "
import ctypes
import ctypes.util

__all__ = ["w3", "libw3"]

libpath = ctypes.util.find_library("w3")
if not(libpath):
	raise Exception("LibW3 not found")

class w3:
	def __init__(self):
		return

libw3_loaded = ctypes.CDLL(libpath)

libw3_loaded.W3_Do_Debug.argtypes = [ctypes.c_bool]

class libw3:
	@staticmethod
	def init():
		libw3_loaded.W3_Library_Init()
	@staticmethod
	def debug(do):
		libw3_loaded.W3_Do_Debug(do)
