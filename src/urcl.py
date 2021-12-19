import ctypes, sys, os

_path = os.path.abspath(os.path.dirname(__file__))

if sys.maxsize > (2 ** 32):
	_version = "/urcl64.lib"
else:
	_version = "/urcl32.lib"

_urcl_lib = ctypes.cdll.LoadLibrary(_path + _version)

c_Char = ctypes.c_uint16
c_Char_p = ctypes.POINTER(c_Char)

class c_OperandInfo(ctypes.Structure):
	_fields_ = [
		("Type", ctypes.c_ulong),
		("ImmediateValue", ctypes.c_ulonglong),
		("StringValue", c_Char_p)
	]

class c_InstructionInfo(ctypes.Structure):
	_fields_ = [
		("Operation", c_Char_p),
		("Operands", c_OperandInfo * 3)
	]

class c_LabelInfo(ctypes.Structure):
	_fields_ = [
		("Name", c_Char_p),
		("Address", ctypes.c_ulonglong)
	]

c_OperandInfo_p = ctypes.POINTER(c_OperandInfo)
c_InstructionInfo_p = ctypes.POINTER(c_InstructionInfo)
c_LabelInfo_p = ctypes.POINTER(c_LabelInfo)

def _LengthOfChars(chars):
	result = 0
	while int(chars[result]) != 0: result += 1
	return result

def _CharsToString(chars, length=None):
	if length == None: length = _LengthOfChars(chars)
	result = ""
	for i in range(length): result += chr(int(chars[i]))
	return result

class ParserError(Exception):
	def __init__(self, message, position, length):
		super().__init__(message)
		self.message = str(message)
		self.position = int(position)
		self.length = int(length)

class Program:
	def __init__(self):
		self.headers = []
		self.instructions = []
		self.labels = []

	def AddSource(self, source):
		source = str(source)

		sourceSize = ctypes.c_size_t(int(ctypes.sizeof(c_Char)) * (len(source) + 1))
		sourcePtr = c_Char_p(ctypes.c_void_p(_urcl_lib.URCL_malloc(sourceSize)))
		for i in range(len(source)): sourcePtr[i] = c_Char(ord(source[i]))
		sourcePtr[len(source)] = c_Char(0)

		length = ctypes.c_ulonglong(len(source))

		instructions = c_InstructionInfo_p()
		labels = c_LabelInfo_p()
		headers = c_InstructionInfo_p()

		instructionsLength = ctypes.c_ulong()
		labelsLength = ctypes.c_ulong()
		headersLength = ctypes.c_ulong()

		if bool(_urcl_lib.URCL_TryParseSource(sourcePtr, length, ctypes.byref(instructions), ctypes.byref(labels), ctypes.byref(headers), ctypes.byref(instructionsLength), ctypes.byref(labelsLength), ctypes.byref(headersLength))):
			#TODO: Translate from ctypes to Python classes.

			_urcl_lib.URCL_FreeInstructions(instructions, instructionsLength)
			_urcl_lib.URCL_FreeLabels(labels, labelsLength)
			_urcl_lib.URCL_FreeInstructions(headers, headersLength)
			_urcl_lib.URCL_free(instructions)
			_urcl_lib.URCL_free(labels)
			_urcl_lib.URCL_free(headers)
			_urcl_lib.URCL_free(sourcePtr)
		else:
			error = ParserError(_CharsToString(_urcl_lib.URCL_Error.Message), int(sourcePtr - _urcl_lib.URCL_Error.Position), _urcl_lib.URCL_Error.Length)
			_urcl_lib.URCL_free(sourcePtr)
			raise error
	
	def __str__(self):
		result = ""
		for header in self.headers: result += str(header) + "\n"
		for addr in range(len(self.instructions)):
			for label in self.labels:
				if label.Address == addr: result += str(label) + "\n"
			result += str(self.instructions[addr]) + "\n"
		return result

if __name__ == "__main__":
	print("This library should not be invoked as a script. Instead, import this library using the import statement.")