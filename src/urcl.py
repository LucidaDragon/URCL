import ctypes, sys, os

_path = os.path.abspath(os.path.dirname(__file__))

if sys.maxsize > (2 ** 32):
	_version = "/urcl64.lib"
else:
	_version = "/urcl32.lib"

_urcl_lib = ctypes.cdll.LoadLibrary(_path + _version)