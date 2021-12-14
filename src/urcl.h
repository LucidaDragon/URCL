#ifndef URCL_Available
#define URCL_Available

void* URCL_malloc(size_t size);
void* URCL_realloc(void* ptr, size_t size);
void URCL_free(void* ptr);

#ifdef URCL_NoChar16
	typedef unsigned short URCL_Char;
#else
	typedef char16_t URCL_Char;
#endif

#ifndef URCL_NoDependencies
	void* malloc(size_t size);
	void* realloc(void* ptr, size_t size);
	void free(void* ptr);

	void* URCL_malloc(size_t size) { return malloc(size); }
	void* URCL_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
	void URCL_free(void* ptr) { free(ptr); }
#endif

#ifndef __cplusplus
	typedef int bool;
	#define true 1
	#define false 0
#endif

#define URCL_MaxOperands 3

#define URCL_OperandType_None 0
#define URCL_OperandType_IndexedRegister 1
#define URCL_OperandType_SpecialRegister 2
#define URCL_OperandType_Immediate 3
#define URCL_OperandType_MemoryAddress 4
#define URCL_OperandType_Label 5
#define URCL_OperandType_Relative 6
#define URCL_OperandType_Port 7
#define URCL_OperandType_Any 8

typedef struct URCL_OperandInfo
{
	unsigned long Type;
	unsigned long long ImmediateValue;
	URCL_Char* StringValue;
} URCL_OperandInfo;

typedef struct URCL_InstructionInfo
{
	URCL_Char* Operation;
	URCL_OperandInfo Operands[URCL_MaxOperands];
} URCL_InstructionInfo;

typedef struct URCL_LabelInfo
{
	URCL_Char* Name;
	unsigned long long Address;
} URCL_LabelInfo;

void URCL_FreeInstructions(URCL_InstructionInfo* instructions, unsigned long count)
{
	for (unsigned long i = 0; i < count; i++)
	{
		URCL_free(instructions[i].Operation);
		instructions[i].Operation = 0;

		for (unsigned long j = 0; j < URCL_MaxOperands; j++)
		{
			if (instructions[i].Operands[j].Type != URCL_OperandType_None)
			{
				URCL_free(instructions[i].Operands[j].StringValue);
				instructions[i].Operands[j].StringValue = 0;
				instructions[i].Operands[j].ImmediateValue = 0;
				instructions[i].Operands[j].Type = URCL_OperandType_None;
			}
		}
	}
}

void URCL_FreeLabels(URCL_LabelInfo* labels, unsigned long count)
{
	for (unsigned long i = 0; i < count; i++) URCL_free(labels[i].Name);
}

#define URCL_Register_StackPointer 0
#define URCL_Register_ProgramCounter 1

const URCL_Char* URCL_Prefixes_Instruction = u"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const URCL_Char* URCL_Prefixes_Macro = u"@";
const URCL_Char* URCL_Prefixes_Label = u".";
const URCL_Char* URCL_Prefixes_Port = u"%";
const URCL_Char* URCL_Prefixes_Immediate = u"0123456789";
const URCL_Char* URCL_Prefixes_Relative = u"~";
const URCL_Char* URCL_Prefixes_Memory = u"Mm";
const URCL_Char* URCL_Prefixes_Register = u"Rr";

const URCL_Char* URCL_Header_Bits = u"BITS";
const URCL_Char* URCL_Header_MinimumRegisters = u"MINREG";
const URCL_Char* URCL_Header_MinimumHeap = u"MINHEAP";
const URCL_Char* URCL_Header_MinimumStack = u"MINSTACK";
const URCL_Char* URCL_Header_InstructionStorage = u"RUN";

const URCL_Char* URCL_Error_NoError = u"No error message has been set.";
const URCL_Char* URCL_Error_OutOfMemory = u"Out of memory.";
const URCL_Char* URCL_Error_MissingOperandPrefix = u"Missing valid operand prefix.";
const URCL_Char* URCL_Error_UnexpectedHexChar = u"Unexpected character in hexadecimal value.";
const URCL_Char* URCL_Error_UnexpectedOctChar = u"Unexpected character in octal value.";
const URCL_Char* URCL_Error_UnexpectedBinChar = u"Unexpected character in binary value.";
const URCL_Char* URCL_Error_UnexpectedDecChar = u"Unexpected character in decimal value.";
const URCL_Char* URCL_Error_UnknownImmTypePrefix = u"Unknown immediate type prefix.";
const URCL_Char* URCL_Error_UnexpectedRelativeChar = u"Unexpected character in relative value.";
const URCL_Char* URCL_Error_RelativeValueTooLarge = u"Relative value is too large.";
const URCL_Char* URCL_Error_InvalidRelativeValue = u"Invalid relative value.";
const URCL_Char* URCL_Error_InvalidMemoryAddress = u"Invalid memory address value.";
const URCL_Char* URCL_Error_InvalidRegisterIndex = u"Invalid register index.";
const URCL_Char* URCL_Error_UnknownRegister = u"Unknown register.";
const URCL_Char* URCL_Error_UnknownMacro = u"Unknown macro.";
const URCL_Char* URCL_Error_ExpectedOperand = u"Expected operand.";
const URCL_Char* URCL_Error_ExpectedOperation = u"Expected operation.";

struct URCL_ErrorInfo
{
	bool HasError;
	const URCL_Char* Position;
	unsigned long Length;
	const URCL_Char* Message;
};

struct URCL_ErrorInfo URCL_Error = { false, 0, 0, u"No error message has been set." };

void URCL_ResetError()
{
	URCL_Error.HasError = false;
	URCL_Error.Position = 0;
	URCL_Error.Length = 0;
	URCL_Error.Message = URCL_Error_NoError;
}

void URCL_SetError(const URCL_Char* message, const URCL_Char* position, unsigned long length)
{
	URCL_Error.HasError = true;
	URCL_Error.Position = position;
	URCL_Error.Length = length;
	URCL_Error.Message = message;
}

bool URCL_HasError()
{
	return URCL_Error.HasError;
}

bool URCL_NoMacroHandler(void* data, const URCL_Char* name, const URCL_Char* source, URCL_Char** result)
{
	unsigned long length = 0;
	for (; name[length] != 0; length++);

	URCL_SetError(URCL_Error_UnknownMacro, source, length);

	return false;
}

void* URCL_memcpy(void* dest, const void* src, size_t n)
{
	for (size_t i = 0; i < n; i++) ((char*)dest)[i] = ((char*)src)[i];
	return dest;
}

URCL_Char* URCL_Substring(const URCL_Char* value, unsigned long index, unsigned long length)
{
	URCL_Char* result = (URCL_Char*)URCL_malloc((length + 1) * sizeof(URCL_Char));
	
	if (result == 0)
	{
		URCL_SetError(URCL_Error_OutOfMemory, value, 0);
		return 0;
	}

	URCL_memcpy(result, value + index, length * sizeof(URCL_Char));
	result[length] = 0;

	return result;
}

unsigned long URCL_TargetBits = 64;

URCL_Char* URCL_ValueToString(unsigned long long value)
{
	URCL_Char* result = (URCL_Char*)URCL_malloc(sizeof(URCL_Char) * (URCL_TargetBits + 3));
	
	if (result == 0)
	{
		URCL_SetError(URCL_Error_OutOfMemory, 0, 0);
		return 0;
	}
	
	result[0] = '0';
	result[1] = 'b';

	for (unsigned long offset = URCL_TargetBits, i = 2; offset > 0; offset--, i++)
	{
		result[i] = ((value >> (offset - 1)) & 1) ? '1' : '0';
	}

	result[URCL_TargetBits + 2] = 0;
	
	return result;
}

bool URCL_BasicMacroHandler(void* data, const URCL_Char* name, const URCL_Char* source, URCL_Char** result);

bool (*URCL_MacroHandler)(void* data, const URCL_Char* name, const URCL_Char* source, URCL_Char** result) = URCL_BasicMacroHandler;
void* URCL_MacroHandlerData = 0;

void URCL_ClearMacroHandler()
{
	URCL_MacroHandler = URCL_NoMacroHandler;
	URCL_MacroHandlerData = 0;
}

void URCL_SetMacroHandler(bool (*getMacroValue)(void* data, const URCL_Char* name, const URCL_Char* source, URCL_Char** result), void* data)
{
	URCL_MacroHandler = getMacroValue;
	URCL_MacroHandlerData = data;
}

void URCL_UseBasicMacroHandler()
{
	URCL_SetMacroHandler(URCL_BasicMacroHandler, 0);
}

bool URCL_SetTargetBits(unsigned long bits)
{
	if ((bits / 8) > sizeof(unsigned long long))
	{
		return false;
	}
	else
	{
		URCL_TargetBits = bits;
		return true;
	}
}

unsigned long long URCL_GetMaxUnsignedValue(unsigned long long size)
{
	unsigned long long result = 0;
	for (unsigned long long i = 0; i < size; i++) result = (result << 8) | 0xFF;
	return result;
}

unsigned long long URCL_GetMaxSignedValue(unsigned long long size)
{
	unsigned long long result = 0;
	for (unsigned long long i = 0; i < size; i++) result = (result << 8) | 0xFF;
	return result >> 1;
}

unsigned long long URCL_GetLowerBits(unsigned long long size)
{
	return URCL_GetMaxUnsignedValue(size) >> (size * 4);
}

unsigned long long URCL_GetUpperBits(unsigned long long size)
{
	return URCL_GetMaxUnsignedValue(size) & ~URCL_GetLowerBits(size);
}

unsigned long long URCL_GetMostSignificantBit(unsigned long long size)
{
	if (size == 0) return 0;
	else return ((unsigned long long)1) << ((size * 8) - 1);
}

unsigned long long URCL_GetSecondMostSignificantBit(unsigned long long size)
{
	return URCL_GetMostSignificantBit(size) >> 1;
}

bool URCL_CharIsWord(URCL_Char value)
{
	return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') || (value == '_');
}

bool URCL_CharIsDigit(URCL_Char value)
{
	return value >= '0' && value <= '9';
}

bool URCL_CharIsWhitespace(URCL_Char value)
{
	switch (value)
	{
		case 0x0009:
		case 0x000A:
		case 0x000B:
		case 0x000C:
		case 0x000D:
		case 0x0020:
		case 0x0085:
		case 0x00A0:
		case 0x1680:
		case 0x2000:
		case 0x2001:
		case 0x2002:
		case 0x2003:
		case 0x2004:
		case 0x2005:
		case 0x2006:
		case 0x2007:
		case 0x2008:
		case 0x2009:
		case 0x200A:
		case 0x2028:
		case 0x2029:
		case 0x202F:
		case 0x205F:
		case 0x3000:
			return true;
		default:
			return false;
	}
}

URCL_Char URCL_CharToLower(URCL_Char value)
{
	if (value >= 'A' && value <= 'Z') return (value - 'A') + 'a';
	else return value;
}

URCL_Char URCL_CharToUpper(URCL_Char value)
{
	if (value >= 'a' && value <= 'z') return (value - 'a') + 'A';
	else return value;
}

unsigned long URCL_CharsLength(const URCL_Char* chars)
{
	unsigned long result = 0;
	for (; chars[result] != 0; result++);
	return result;
}

bool URCL_IsStringEqualCaseInsensitive(const URCL_Char* a, unsigned long aLength, const URCL_Char* b, unsigned long bLength)
{
	if (aLength != bLength) return false;

	for (unsigned long i = 0; i < aLength; i++)
	{
		if (URCL_CharToUpper(a[i]) != URCL_CharToUpper(b[i])) return false;
	}

	return true;
}

unsigned long URCL_GetWhitespaceLength(const URCL_Char* value, unsigned long length, bool includeNewLine)
{
	unsigned long result = 0;
	for (; result < length; result++)
	{
		if (!URCL_CharIsWhitespace(value[result]) || (!includeNewLine && (value[result] == '\n'))) break;
	}
	return result;
}

unsigned long URCL_GetOperandLength(const URCL_Char* value, unsigned long length)
{
	unsigned long result = 0;
	for (; result < length; result++)
	{
		if (URCL_CharIsWhitespace(value[result]) || value[result] == ',') break;
	}
	return result;
}

unsigned long URCL_GetLineLength(const URCL_Char* value, unsigned long length)
{
	unsigned long result = 0;
	URCL_Char lastChar = 0;
	bool isInComment = false;

	for (; result < length; result++)
	{
		if (isInComment)
		{
			if (value[result] == '/' && lastChar == '*') isInComment = false;
		}
		else
		{
			if (value[result] == '*' && lastChar == '/') isInComment = true;
			else if (value[result] == '\n') break;
		}
		lastChar = value[result];
	}

	return result;
}

bool URCL_BasicMacroHandler(void* data, const URCL_Char* name, const URCL_Char* source, URCL_Char** result)
{
	unsigned long nameLength = 0;
	for (; name[nameLength] != 0; nameLength++);

	if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@MAX", 4))
	{
		*result = URCL_ValueToString(URCL_GetMaxUnsignedValue(URCL_TargetBits / 8));
		return !URCL_HasError();
	}
	else if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@SMAX", 5))
	{
		*result = URCL_ValueToString(URCL_GetMaxSignedValue(URCL_TargetBits / 8));
		return !URCL_HasError();
	}
	else if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@MSB", 4))
	{
		*result = URCL_ValueToString(URCL_GetMostSignificantBit(URCL_TargetBits / 8));
		return !URCL_HasError();
	}
	else if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@SMSB", 5))
	{
		*result = URCL_ValueToString(URCL_GetSecondMostSignificantBit(URCL_TargetBits / 8));
		return !URCL_HasError();
	}
	else if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@LHALF", 6))
	{
		*result = URCL_ValueToString(URCL_GetLowerBits(URCL_TargetBits / 8));
		return !URCL_HasError();
	}
	else if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@UHALF", 6))
	{
		*result = URCL_ValueToString(URCL_GetUpperBits(URCL_TargetBits / 8));
		return !URCL_HasError();
	}
	else if (URCL_IsStringEqualCaseInsensitive(name, nameLength, u"@BITS", 5))
	{
		*result = URCL_ValueToString(URCL_TargetBits);
		return !URCL_HasError();
	}
	else
	{
		URCL_SetError(URCL_Error_UnknownMacro, source, nameLength);
		return false;
	}
}

bool URCL_InvokeMacroHandler(const URCL_Char* value, unsigned long length, URCL_Char** result)
{
	URCL_Char* name = URCL_Substring(value, 0, URCL_GetOperandLength(value, length));
	bool returnValue = URCL_MacroHandler(URCL_MacroHandlerData, name, value, result);
	URCL_free(name);
	return returnValue;
}

bool URCL_TryParseOperandWithPrefix(URCL_Char prefix, const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	if (length == 0 || value[0] != prefix)
	{
		URCL_SetError(URCL_Error_MissingOperandPrefix, value, 1);
		return false;
	}

	*resultLength = URCL_GetOperandLength(value, length);
	*result = URCL_Substring(value, 0, *resultLength);

	return *result != 0;
}

bool URCL_TryParseOperandWithPrefixes(const URCL_Char* prefixes, const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	for (unsigned long i = 0; prefixes[i] != 0; i++)
	{
		if (URCL_TryParseOperandWithPrefix(prefixes[i], value, length, result, resultLength))
		{
			URCL_ResetError();
			return true;
		}
		else
		{
			if (prefixes[i + 1] != 0) URCL_ResetError();
		}
	}

	return false;
}

bool URCL_TryParseAny(const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	*resultLength = URCL_GetOperandLength(value, length);
	*result = URCL_Substring(value, 0, *resultLength);

	return *result != 0;
}

bool URCL_TryParseLabel(const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	return URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Label, value, length, result, resultLength);
}

bool URCL_TryParsePort(const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	return URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Port, value, length, result, resultLength);
}

bool URCL_TryParseImmediate(const URCL_Char* value, unsigned long length, unsigned long long* result, unsigned long* resultLength)
{
	URCL_Char* resultString = 0;
	if (URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Immediate, value, length, &resultString, resultLength))
	{
		if (*resultLength > 2 && resultString[0] == '0' && !URCL_CharIsDigit(resultString[1]))
		{
			if (resultString[1] == 'x' || resultString[1] == 'X')
			{
				*result = 0;
				for (unsigned long i = 2; i < *resultLength; i++)
				{
					if (resultString[i] >= '0' && resultString[i] <= '9')
					{
						*result = (*result << 4) | (resultString[i] - '0');
					}
					else if (resultString[i] >= 'A' && resultString[i] <= 'F')
					{
						*result = (*result << 4) | ((resultString[i] - 'A') + 10);
					}
					else if (resultString[i] >= 'a' && resultString[i] <= 'f')
					{
						*result = (*result << 4) | ((resultString[i] - 'a') + 10);
					}
					else
					{
						URCL_SetError(URCL_Error_UnexpectedHexChar, value + i, 1);
						URCL_free(resultString);
						return false;
					}
				}
			}
			else if (resultString[1] == 'o' || resultString[1] == 'O')
			{
				*result = 0;
				for (unsigned long i = 2; i < *resultLength; i++)
				{
					if (resultString[i] >= '0' && resultString[i] <= '7')
					{
						*result = (*result << 3) | (resultString[i] - '0');
					}
					else
					{
						URCL_SetError(URCL_Error_UnexpectedOctChar, value + i, 1);
						URCL_free(resultString);
						return false;
					}
				}
			}
			else if (resultString[1] == 'b' || resultString[1] == 'B')
			{
				*result = 0;
				for (unsigned long i = 2; i < *resultLength; i++)
				{
					if (resultString[i] >= '0' && resultString[i] <= '1')
					{
						*result = (*result << 1) | (resultString[i] == '1' ? 1 : 0);
					}
					else
					{
						URCL_SetError(URCL_Error_UnexpectedBinChar, value + i, 1);
						URCL_free(resultString);
						return false;
					}
				}
			}
			else
			{
				URCL_SetError(URCL_Error_UnknownImmTypePrefix, value + 1, 1);
				URCL_free(resultString);
				return false;
			}
		}
		else
		{
			*result = 0;
			for (unsigned long i = 0; i < *resultLength; i++)
			{
				if (resultString[i] >= '0' && resultString[i] <= '9')
				{
					*result = (*result * 10) + (resultString[i] - '0');
				}
				else
				{
					URCL_SetError(URCL_Error_UnexpectedDecChar, value + i, 1);
					URCL_free(resultString);
					return false;
				}
			}
		}

		URCL_free(resultString);
		return true;
	}
	else
	{
		return false;
	}
}

bool URCL_TryParseRelative(const URCL_Char* value, unsigned long length, long long* result, unsigned long* resultLength)
{
	URCL_Char* resultString = 0;
	if (URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Relative, value, length, &resultString, resultLength))
	{
		if ((*resultLength > 2) || (*resultLength == 2 && URCL_CharIsDigit(resultString[1])))
		{
			unsigned long offset = 1;
			bool negative = false;
			
			if (resultString[offset] == '-')
			{
				negative = true;
				offset++;
			}
			else if (resultString[offset] == '+')
			{
				offset++;
			}
			else if (!URCL_CharIsDigit(resultString[offset]))
			{
				URCL_SetError(URCL_Error_UnexpectedRelativeChar, value + offset, 1);
				URCL_free(resultString);
				return false;
			}

			URCL_free(resultString);

			unsigned long long immediateValue;
			unsigned long immediateLength;
			if (URCL_TryParseImmediate(value + offset, *resultLength - offset, &immediateValue, &immediateLength))
			{
				if (immediateValue > URCL_GetMaxSignedValue(sizeof(long long)))
				{
					URCL_SetError(URCL_Error_RelativeValueTooLarge, value + offset, *resultLength);
					return false;
				}

				if (negative) *result = -((long long)immediateValue);
				else *result = (long long)immediateValue;

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			URCL_SetError(URCL_Error_InvalidRelativeValue, value + 1, 1);
			URCL_free(resultString);
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool URCL_TryParseMemoryAddress(const URCL_Char* value, unsigned long length, unsigned long long* result, unsigned long* resultLength)
{
	URCL_Char* resultString = 0;
	if (URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Memory, value, length, &resultString, resultLength))
	{
		URCL_free(resultString);

		if (*resultLength > 1)
		{
			unsigned long immediateLength;
			return URCL_TryParseImmediate(value + 1, *resultLength - 1, result, &immediateLength);
		}
		else
		{
			URCL_SetError(URCL_Error_InvalidMemoryAddress, value + 1, 1);
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool URCL_TryParseRegister(const URCL_Char* value, unsigned long length, unsigned long long* result, bool* isSpecialRegister, unsigned long* resultLength)
{
	URCL_Char* resultString = 0;
	if (URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Register, value, length, &resultString, resultLength))
	{
		URCL_free(resultString);

		if (*resultLength > 1)
		{
			unsigned long immediateLength;
			*isSpecialRegister = false;
			return URCL_TryParseImmediate(value + 1, *resultLength - 1, result, &immediateLength);
		}
		else
		{
			URCL_SetError(URCL_Error_InvalidRegisterIndex, value + 1, 1);
			return false;
		}
	}
	else
	{
		URCL_ResetError();

		*resultLength = URCL_GetOperandLength(value, length);

		if (URCL_IsStringEqualCaseInsensitive(u"SP", 2, value, *resultLength))
		{
			*isSpecialRegister = true;
			*result = URCL_Register_StackPointer;
			return true;
		}
		else if (URCL_IsStringEqualCaseInsensitive(u"PC", 2, value, *resultLength))
		{
			*isSpecialRegister = true;
			*result = URCL_Register_ProgramCounter;
			return true;
		}
		else
		{
			URCL_SetError(URCL_Error_UnknownRegister, value, *resultLength);
			return false;
		}
	}
}

bool URCL_TryParseMacro(const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	URCL_Char* resultString = 0;
	if (URCL_TryParseOperandWithPrefixes(URCL_Prefixes_Macro, value, length, &resultString, resultLength))
	{
		URCL_free(resultString);

		if (URCL_InvokeMacroHandler(value, *resultLength, result))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool URCL_TryParseOperand(const URCL_Char* value, unsigned long length, bool allowHeaderFields, URCL_OperandInfo* result, unsigned long* resultLength);

bool URCL_TryParseMacroOperand(const URCL_Char* value, unsigned long length, URCL_OperandInfo* result, unsigned long* resultLength)
{
	URCL_Char* resultString = 0;
	if (URCL_TryParseMacro(value, length, &resultString, resultLength))
	{
		unsigned long operandLength = 0;
		for (; resultString[operandLength] != 0; operandLength++);

		unsigned long parsedLength = 0;
		if (URCL_TryParseOperand(resultString, operandLength, false, result, &parsedLength))
		{
			URCL_free(resultString);
			return true;
		}
		else
		{
			URCL_free(resultString);
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool URCL_TryParseOperand(const URCL_Char* value, unsigned long length, bool allowHeaderFields, URCL_OperandInfo* result, unsigned long* resultLength)
{
	unsigned long startPadding = URCL_GetWhitespaceLength(value, length, false);
	value += startPadding;
	length -= startPadding;

	if (length == 0)
	{
		URCL_SetError(URCL_Error_ExpectedOperand, value, 0);
		return false;
	}

	result->Type = URCL_OperandType_None;
	result->ImmediateValue = 0;
	result->StringValue = 0;

	unsigned long trimmedLength = 0;
	bool isSpecialRegister = false;
	if (URCL_TryParseLabel(value, length, &(result->StringValue), &trimmedLength))
	{
		result->Type = URCL_OperandType_Label;
	}
	else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParsePort(value, length, &(result->StringValue), &trimmedLength))
	{
		URCL_ResetError();
		result->Type = URCL_OperandType_Port;
	}
	else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseImmediate(value, length, &(result->ImmediateValue), &trimmedLength))
	{
		URCL_ResetError();
		result->Type = URCL_OperandType_Immediate;
		result->StringValue = URCL_Substring(value, 0, trimmedLength);
	}
	else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseRelative(value, length, (long long*)&(result->ImmediateValue), &trimmedLength))
	{
		URCL_ResetError();
		result->Type = URCL_OperandType_Relative;
		result->StringValue = URCL_Substring(value, 0, trimmedLength);
	}
	else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseMemoryAddress(value, length, &(result->ImmediateValue), &trimmedLength))
	{
		URCL_ResetError();
		result->Type = URCL_OperandType_MemoryAddress;
		result->StringValue = URCL_Substring(value, 0, trimmedLength);
	}
	else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseMacroOperand(value, length, result, &trimmedLength))
	{
		URCL_ResetError();
	}
	else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseRegister(value, length, &(result->ImmediateValue), &isSpecialRegister, &trimmedLength))
	{
		URCL_ResetError();
		result->Type = isSpecialRegister ? URCL_OperandType_SpecialRegister : URCL_OperandType_IndexedRegister;
		result->StringValue = URCL_Substring(value, 0, trimmedLength);
	}
	else if (URCL_TryParseAny(value, length, &(result->StringValue), &trimmedLength))
	{
		URCL_ResetError();
		result->Type = URCL_OperandType_Any;
	}
	else
	{
		return false;
	}

	if (URCL_HasError())
	{
		result->Type = URCL_OperandType_None;
		result->ImmediateValue = 0;
		if (result->StringValue != 0)
		{
			URCL_free(result->StringValue);
			result->StringValue = 0;
		}

		return false;
	}

	value += trimmedLength;
	length -= trimmedLength;

	unsigned long endPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength = startPadding + trimmedLength + endPadding;
	return true;
}

unsigned long URCL_GetCommentLength(const URCL_Char* value, unsigned long length)
{
	unsigned long result = 0;

	if (length >= 2 && value[0] == '/')
	{
		if (value[1] == '/')
		{
			for (; result < length; result++)
			{
				if (value[result] == '\n') break;
			}

			return result;
		}
		else if (value[1] == '*')
		{
			URCL_Char previousChar = 0;
			for (; result < length; result++)
			{
				URCL_Char currentChar = value[result];
				
				if (currentChar == '/' && previousChar == '*')
				{
					result++;
					return result;
				}

				previousChar = currentChar;
			}
		}
	}

	return 0;
}

bool URCL_IsInstructionHeaderName(URCL_Char* name, unsigned long nameLength)
{
	return URCL_IsStringEqualCaseInsensitive(URCL_Header_Bits, URCL_CharsLength(URCL_Header_Bits), name, nameLength) ||
		URCL_IsStringEqualCaseInsensitive(URCL_Header_MinimumRegisters, URCL_CharsLength(URCL_Header_MinimumRegisters), name, nameLength) ||
		URCL_IsStringEqualCaseInsensitive(URCL_Header_MinimumHeap, URCL_CharsLength(URCL_Header_MinimumHeap), name, nameLength) ||
		URCL_IsStringEqualCaseInsensitive(URCL_Header_MinimumStack, URCL_CharsLength(URCL_Header_MinimumStack), name, nameLength) ||
		URCL_IsStringEqualCaseInsensitive(URCL_Header_InstructionStorage, URCL_CharsLength(URCL_Header_InstructionStorage), name, nameLength);
}

bool URCL_IsInstructionHeader(URCL_InstructionInfo* instruction)
{
	URCL_Char* name = instruction->Operation;
	unsigned long nameLength = URCL_CharsLength(name);
	return URCL_IsInstructionHeaderName(name, nameLength);
}

bool URCL_TryParseInstruction(const URCL_Char* value, unsigned long length, URCL_InstructionInfo* result, unsigned long* resultLength)
{
	URCL_ResetError();
	*resultLength = 0;

	unsigned long preCommentPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += preCommentPadding;
	value += preCommentPadding;
	length -= preCommentPadding;

	unsigned long preCommentLength = URCL_GetCommentLength(value, length);
	*resultLength += preCommentLength;
	value += preCommentLength;
	length -= preCommentLength;

	unsigned long startPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += startPadding;
	value += startPadding;
	length -= startPadding;

	unsigned long operationLength = URCL_GetOperandLength(value, length);
	*resultLength += operationLength;
	value += operationLength;
	length -= operationLength;

	if (operationLength == 0)
	{
		*resultLength = 0;
		URCL_SetError(URCL_Error_ExpectedOperation, value, 0);
		return false;
	}

	result->Operation = URCL_Substring(value - operationLength, 0, operationLength);

	for (unsigned long i = 0; i < URCL_MaxOperands; i++)
	{
		result->Operands[i].Type = URCL_OperandType_None;
		result->Operands[i].ImmediateValue = 0;
		result->Operands[i].StringValue = 0;
	}

	unsigned long endPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += endPadding;
	value += endPadding;
	length -= endPadding;

	unsigned long commentLength = URCL_GetCommentLength(value, length);
	*resultLength += commentLength;
	value += commentLength;
	length -= commentLength;

	unsigned long trailingPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += trailingPadding;
	value += trailingPadding;
	length -= trailingPadding;

	if (length == 0) return true;

	bool isHeader = URCL_IsInstructionHeaderName(result->Operation, URCL_CharsLength(result->Operation));

	for (unsigned long i = 0; i < URCL_MaxOperands && ((length > 0) && (*value != '\n')); i++)
	{
		unsigned long preOperandPadding = URCL_GetWhitespaceLength(value, length, false);
		*resultLength += preOperandPadding;
		value += preOperandPadding;
		length -= preOperandPadding;

		unsigned long preOperandCommentLength = URCL_GetCommentLength(value, length);
		*resultLength += preOperandCommentLength;
		value += preOperandCommentLength;
		length -= preOperandCommentLength;

		unsigned long operandLength = 0;
		if (URCL_TryParseOperand(value, length, isHeader, &(result->Operands[i]), &operandLength))
		{
			*resultLength += operandLength;
			value += operandLength;
			length -= operandLength;

			unsigned long postOperandCommentLength = URCL_GetCommentLength(value, length);
			*resultLength += postOperandCommentLength;
			value += postOperandCommentLength;
			length -= postOperandCommentLength;

			unsigned long operandPadding = URCL_GetWhitespaceLength(value, length, false);
			*resultLength += operandPadding;
			value += operandPadding;
			length -= operandPadding;

			if (length > 0 && value[0] == ',')
			{
				*resultLength += 1;
				value += 1;
				length -= 1;

				if (length == 0)
				{
					*resultLength = 0;
					URCL_SetError(URCL_Error_ExpectedOperand, value - 1, 1);
					URCL_FreeInstructions(result, 1);
					return false;
				}
			}
		}
		else
		{
			*resultLength = 0;
			URCL_FreeInstructions(result, 1);
			return false;
		}
	}

	if (!URCL_HasError() && ((length == 0) || (*value == '\n'))) return true;

	*resultLength = 0;
	URCL_FreeInstructions(result, 1);
	return false;
}

bool URCL_TryParseMacroInstruction(const URCL_Char* value, unsigned long length, URCL_InstructionInfo* result, unsigned long* resultLength)
{
	URCL_ResetError();
	*resultLength = 0;

	unsigned long preCommentPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += preCommentPadding;
	value += preCommentPadding;
	length -= preCommentPadding;

	unsigned long preCommentLength = URCL_GetCommentLength(value, length);
	*resultLength += preCommentLength;
	value += preCommentLength;
	length -= preCommentLength;

	unsigned long startPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += startPadding;
	value += startPadding;
	length -= startPadding;

	URCL_Char* resultString = 0;
	unsigned long macroLength = 0;
	if (URCL_TryParseMacro(value, length, &resultString, &macroLength))
	{
		*resultLength += macroLength;
		value += macroLength;
		length -= macroLength;

		unsigned long instructionLength = 0;
		for (; resultString[instructionLength] != 0; instructionLength++);

		unsigned long parsedLength = 0;
		if (URCL_TryParseInstruction(resultString, instructionLength, result, &parsedLength))
		{
			URCL_free(resultString);

			unsigned long endPadding = URCL_GetWhitespaceLength(value, length, false);
			*resultLength += endPadding;
			value += endPadding;
			length -= endPadding;

			unsigned long commentLength = URCL_GetCommentLength(value, length);
			*resultLength += commentLength;
			value += commentLength;
			length -= commentLength;

			unsigned long trailingPadding = URCL_GetWhitespaceLength(value, length, false);
			*resultLength += trailingPadding;
			value += trailingPadding;
			length -= trailingPadding;

			if (!URCL_HasError() && ((length == 0) || (*value == '\n')))
			{
				return true;
			}
			else
			{
				*resultLength = 0;
				return false;
			}
		}
		else
		{
			*resultLength = 0;
			URCL_free(resultString);
			return false;
		}
	}
	else
	{
		*resultLength = 0;
		return false;
	}
}

bool URCL_TryParseLabelLine(const URCL_Char* value, unsigned long length, URCL_Char** result, unsigned long* resultLength)
{
	URCL_ResetError();
	*resultLength = 0;

	unsigned long preCommentPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += preCommentPadding;
	value += preCommentPadding;
	length -= preCommentPadding;

	unsigned long preCommentLength = URCL_GetCommentLength(value, length);
	*resultLength += preCommentLength;
	value += preCommentLength;
	length -= preCommentLength;

	unsigned long startPadding = URCL_GetWhitespaceLength(value, length, false);
	*resultLength += startPadding;
	value += startPadding;
	length -= startPadding;

	unsigned long labelLength = 0;
	if (URCL_TryParseLabel(value, length, result, &labelLength))
	{
		*resultLength += labelLength;
		value += labelLength;
		length -= labelLength;

		unsigned long endPadding = URCL_GetWhitespaceLength(value, length, false);
		*resultLength += endPadding;
		value += endPadding;
		length -= endPadding;

		unsigned long commentLength = URCL_GetCommentLength(value, length);
		*resultLength += commentLength;
		value += commentLength;
		length -= commentLength;

		unsigned long trailingPadding = URCL_GetWhitespaceLength(value, length, false);
		*resultLength += trailingPadding;
		value += trailingPadding;
		length -= trailingPadding;

		if (!URCL_HasError() && ((length == 0) || (*value == '\n')))
		{
			return true;
		}
		else
		{
			*resultLength = 0;
			return false;
		}
	}
	else
	{
		*resultLength = 0;
		return false;
	}
}

bool URCL_TryGrowInstructionsArray(URCL_InstructionInfo** instructions, unsigned long* capacity)
{
	*capacity *= 2;
	void* resized = URCL_realloc(*instructions, *capacity * sizeof(URCL_InstructionInfo));

	if (resized == 0)
	{
		*capacity /= 2;
		return false;
	}
	
	*instructions = (URCL_InstructionInfo*)resized;
	return true;
}

bool URCL_TryParseSource(const URCL_Char* source, unsigned long length, URCL_InstructionInfo** result, URCL_LabelInfo** labels, URCL_InstructionInfo** headers, unsigned long* instructionCount, unsigned long* labelCount, unsigned long* headerCount)
{
	unsigned long resultCapacity = 1;
	unsigned long resultLength = 0;
	*result = (URCL_InstructionInfo*)URCL_malloc(resultCapacity * sizeof(URCL_InstructionInfo));

	unsigned long labelsCapacity = 1;
	unsigned long labelsLength = 0;
	*labels = (URCL_LabelInfo*)URCL_malloc(labelsCapacity * sizeof(URCL_LabelInfo));

	unsigned long headersCapacity = 1;
	unsigned long headersLength = 0;
	*headers = (URCL_InstructionInfo*)URCL_malloc(headersCapacity * sizeof(URCL_InstructionInfo));

	while (length > 0)
	{
		bool generatedNewInstruction = false;
		URCL_Char* labelName = 0;
		unsigned long offset = 0;
		if (URCL_TryParseLabelLine(source, length, &labelName, &offset))
		{
			labels[labelsLength]->Name = labelName;
			labels[labelsLength]->Address = resultLength;
			labelsLength++;
			
			if (labelsLength == labelsCapacity)
			{
				labelsCapacity *= 2;
				void* resized = URCL_realloc(*labels, labelsCapacity * sizeof(URCL_LabelInfo));

				if (resized == 0)
				{
					URCL_SetError(URCL_Error_OutOfMemory, source, offset);
					URCL_FreeInstructions(*result, resultLength);
					URCL_FreeLabels(*labels, labelsLength);
					URCL_FreeInstructions(*headers, headersLength);
					URCL_free(*result);
					URCL_free(*labels);
					return false;
				}

				*labels = (URCL_LabelInfo*)resized;
			}
		}
		else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseMacroInstruction(source, length, &((*result)[resultLength]), &offset))
		{
			URCL_ResetError();
			generatedNewInstruction = true;
		}
		else if ((URCL_Error.Message == URCL_Error_MissingOperandPrefix) && URCL_TryParseInstruction(source, length, &((*result)[resultLength]), &offset))
		{
			URCL_ResetError();
			generatedNewInstruction = true;
		}
		else
		{
			if (URCL_Error.Message == URCL_Error_ExpectedOperation)
			{
				URCL_ResetError();
				offset = URCL_GetLineLength(source, length);
			}
			else
			{
				URCL_FreeInstructions(*result, resultLength);
				URCL_FreeLabels(*labels, labelsLength);
				URCL_FreeInstructions(*headers, headersLength);
				URCL_free(*result);
				URCL_free(*labels);
				URCL_free(*headers);
				return false;
			}
		}

		if (generatedNewInstruction)
		{
			bool outOfMemory = false;
			if (URCL_IsInstructionHeader(&((*result)[resultLength])))
			{
				(*headers)[headersLength] = (*result)[resultLength];
				headersLength++;
				if (headersLength == headersCapacity) outOfMemory = !URCL_TryGrowInstructionsArray(headers, &headersCapacity);
			}
			else
			{
				resultLength++;
				if (resultLength == resultCapacity) outOfMemory = !URCL_TryGrowInstructionsArray(result, &resultCapacity);
			}
			
			if (outOfMemory)
			{
				URCL_SetError(URCL_Error_OutOfMemory, source, offset);
				URCL_FreeInstructions(*result, resultLength);
				URCL_FreeLabels(*labels, labelsLength);
				URCL_free(*result);
				URCL_free(*labels);
				URCL_free(*headers);
				return false;
			}
		}

		source += offset;
		length -= offset;

		if ((length > 0) && (*source == '\n'))
		{
			source++;
			length--;
		}
	}
	
	void* shrunk = URCL_realloc(*result, resultLength * sizeof(URCL_InstructionInfo));
	if (shrunk != 0) *result = (URCL_InstructionInfo*)shrunk;

	shrunk = URCL_realloc(*labels, labelsLength * sizeof(URCL_LabelInfo));
	if (shrunk != 0) *labels = (URCL_LabelInfo*)shrunk;

	shrunk = URCL_realloc(*headers, headersLength * sizeof(URCL_InstructionInfo));
	if (shrunk != 0) *headers = (URCL_InstructionInfo*)shrunk;

	*instructionCount = resultLength;
	*labelCount = labelsLength;
	*headerCount = headersLength;
	return true;
}

#endif