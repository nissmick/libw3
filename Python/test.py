from LibW3 import w3, libw3

def data_handler(w3cli, data, size):
	print(data)

def header_handler(w3cli, key, value):
	print("`" + key + "`: `" + value + "`")

def status_handler(w3cli, status):
	print("Status: " + str(status))

libw3.init()
w3cli = w3("http", "localhost", 1024)
w3cli.set_path("/")
w3cli.set_method("GET")
w3cli.set_data_handler(data_handler)
w3cli.set_header_handler(header_handler)
w3cli.set_status_handler(status_handler)
w3cli.set_read_size(2048)
w3cli.send_request()
w3cli.destroy()
