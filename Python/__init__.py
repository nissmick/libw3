# $Id$
import ctypes
import ctypes.util

__all__ = ["w3", "libw3"]

libpath = ctypes.util.find_library("w3")
if not(libpath):
	raise Exception("LibW3 not found")

libw3_loaded = ctypes.CDLL(libpath)

libw3_loaded.W3_Do_Debug.argtypes = [ctypes.c_bool]
libw3_loaded.W3_Do_Debug = None

libw3_loaded.W3_Library_Init.argtypes = []
libw3_loaded.W3_Library_Init.restype = None

libw3_loaded.W3_Create.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
libw3_loaded.W3_Create.restype = ctypes.c_void_p

libw3_loaded.W3_Send_Request.argtypes = [ctypes.c_void_p]
libw3_loaded.W3_Send_Request.restype = None

libw3_loaded.W3_Set_Method.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
libw3_loaded.W3_Set_Method.restype = None

libw3_loaded.W3_Set_Path.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
libw3_loaded.W3_Set_Path.restype = None

libw3_loaded.W3_On.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
libw3_loaded.W3_On.restype = None

w3_list = []

@ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t)
def global_data_handler(cli, data, size):
	for i in w3_list:
		if cli == i._client:
			ba = bytearray(size)
			for j in range(size):
				ba[j] = data[j]
			if i._data_handler:
				i._data_handler(i, ba, size)


class w3:
	def __init__(self, protocol, hostname, port):
		self._client = libw3_loaded.W3_Create(protocol.encode("utf-8"), hostname.encode("utf-8"), port);
		self._data_handler = None
		if not(self._client):
			raise Exception("Failed to conenct")
		else:
			w3_list.append(self)
			libw3_loaded.W3_On(self._client, b"data", global_data_handler);
		return
	def set_path(self, path):
		libw3_loaded.W3_Set_Path(self._client, path.encode("utf-8"))
	def set_method(self, method):
		libw3_loaded.W3_Set_Method(self._client, method.encode("utf-8"))
	def set_data_handler(self, handler):
		self._data_handler = handler
	def send_request(self):
		libw3_loaded.W3_Send_Request(self._client)

class libw3:
	@staticmethod
	def init():
		libw3_loaded.W3_Library_Init()
	@staticmethod
	def debug(do):
		libw3_loaded.W3_Do_Debug(do)
