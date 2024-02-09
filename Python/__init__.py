# $Id$
import ctypes
import ctypes.util

__all__ = ["w3", "libw3"]

libw3_loaded = ctypes.CDLL("libw3.so")

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

libw3_loaded.W3_Set_Read_Size.argtypes = [ctypes.c_void_p, ctypes.c_int]
libw3_loaded.W3_Set_Read_Size.restype = None

libw3_loaded.W3_Set_Header.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
libw3_loaded.W3_Set_Header.restype = None

libw3_loaded.W3_Disconnect.argtypes = [ctypes.c_void_p]
libw3_loaded.W3_Disconnect.restype = None

libw3_loaded.W3_Free.argtypes = [ctypes.c_void_p]
libw3_loaded.W3_Free.restype = None


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

@ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
def global_header_handler(cli, key, value):
	for i in w3_list:
		if cli == i._client:
			if i._header_handler:
				i._header_handler(i, key.decode("utf-8"), value.decode("utf-8"))

@ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_int)
def global_status_handler(cli, status):
	for i in w3_list:
		if cli == i._client:
			if i._status_handler:
				i._status_handler(i, status)

class w3:
	def __init__(self, protocol, hostname, port):
		self._client = libw3_loaded.W3_Create(protocol.encode("utf-8"), hostname.encode("utf-8"), port);
		self._data_handler = None
		self._header_handler = None
		self._status_handler = None
		if not(self._client):
			raise Exception("Failed to conenct")
		else:
			w3_list.append(self)
			libw3_loaded.W3_On(self._client, b"data", global_data_handler);
			libw3_loaded.W3_On(self._client, b"header", global_header_handler);
			libw3_loaded.W3_On(self._client, b"status", global_status_handler);
		return
	def set_path(self, path):
		libw3_loaded.W3_Set_Path(self._client, path.encode("utf-8"))
	def set_method(self, method):
		libw3_loaded.W3_Set_Method(self._client, method.encode("utf-8"))
	def set_data_handler(self, handler):
		self._data_handler = handler
	def set_header_handler(self, handler):
		self._header_handler = handler
	def set_status_handler(self, handler):
		self._status_handler = handler
	def set_read_size(self, size):
		libw3_loaded.W3_Set_Read_Size(self._client, size)
	def set_header(self, key, value):
		libw3_loaded.W3_Set_Header(self._client, key.encode("utf-8"), value.encode("utf-8"))
	def disconnect(self):
		libw3_loaded.W3_Disconnect(self._client)
	def send_request(self):
		libw3_loaded.W3_Send_Request(self._client)
	def destroy(self):
		global w3_list
		new_list = list()
		for i in w3_list:
			if i._client != self._client:
				new_list.append(i)
		w3_list = new_list
		libw3_loaded.W3_Free(self._client)

class libw3:
	@staticmethod
	def init():
		libw3_loaded.W3_Library_Init()
	@staticmethod
	def debug(do):
		libw3_loaded.W3_Do_Debug(do)
