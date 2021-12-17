#include <exception>
#include <string>
#include <vector>

namespace URCL::Internal
{
	extern "C"
	{
	#include "urcl.h"
	}

	void URCL_StringToUpper(std::wstring* str)
	{
		for (size_t i = 0; i < str->length(); i++)
		{
			if ((*str)[i] >= 'a' && (*str)[i] <= 'z') (*str)[i] = ((*str)[i] - 'a') + 'A';
		}
	}

	URCL_Char* URCL_StringToChars(std::wstring str)
	{
		size_t length = str.length();
		URCL_Char* result = new URCL_Char[length + 1];
		for (size_t i = 0; i < length; i++) result[i] = (URCL_Char)str[i];
		result[length] = 0;
		return result;
	}

	std::wstring URCL_CharsToString(const URCL_Char* chars, size_t length)
	{
		std::wstring result = std::wstring(length, '\0');
		for (size_t i = 0; i < length; i++) result[i] = (wchar_t)chars[i];
		return result;
	}
}

////id URCL
namespace URCL
{
	using namespace Internal;

	////namespace URCL
	////description Represents a location within the URCL program.
	class Label
	{
		private:
			std::wstring Name;
			unsigned long long Address;

		public:
			////type ctor
			////class Label
			////namespace URCL
			////description Creates a label with the specified name and address.
			Label(std::wstring name, unsigned long long address)
			{
				Name = name;
				Address = address;
			}

			////class Label
			////namespace URCL
			////description Returns the name of the label.
			std::wstring GetName()
			{
				return Name;
			}

			////class Label
			////namespace URCL
			////description Returns the address of the label.
			unsigned long long GetAddress()
			{
				return Address;
			}
	};

	////namespace URCL
	////description An exception throw when a parsing error occurs.
	class ParserError : public std::exception
	{
		private:
			std::wstring Message;
			unsigned long Position;
			unsigned long Length;

		public:
			////type ctor
			////class ParserError
			////namespace URCL
			////description Creates a ParserError with the specified message.
			ParserError(std::wstring message)
			{
				Message = message;
				Position = 0;
				Length = 0;
			}

			////type ctor
			////class ParserError
			////namespace URCL
			////description Creates a ParserError with the specified message, source position, and source length.
			ParserError(std::wstring message, unsigned long position, unsigned long length)
			{
				Message = message;
				Position = position;
				Length = length;
			}

			////class ParserError
			////namespace URCL
			////description Returns the reported message.
			std::wstring GetMessage()
			{
				return Message;
			}

			////class ParserError
			////namespace URCL
			////description Returns the position in source where the parsing error occurred.
			unsigned long long GetPosition()
			{
				return Position;
			}

			////class ParserError
			////namespace URCL
			////description Returns the length of source that the parsing error occurred in.
			unsigned long long GetLength()
			{
				return Length;
			}
	};

	////namespace URCL
	////description Specifies types of URCL operands.
	////fieldref Custom urcl.h::URCL_OperandType_None Specifies a user-defined operand.
	////fieldref IndexedRegister urcl.h::URCL_OperandType_IndexedRegister Specifies a general-purpose register operand.
	////fieldref SpecialRegister urcl.h::URCL_OperandType_SpecialRegister Specifies a special-purpose register operand.
	////fieldref Immediate urcl.h::URCL_OperandType_Immediate Specifies an immediate operand.
	////fieldref MemoryAddress urcl.h::URCL_OperandType_MemoryAddress Specifies an address operand.
	////fieldref Label urcl.h::URCL_OperandType_Label Specifies a label operand.
	////fieldref Relative urcl.h::URCL_OperandType_Relative Specifies a relative address operand.
	////fieldref Port urcl.h::URCL_OperandType_Port Specifies a named port operand.
	////fieldref Any urcl.h::URCL_OperandType_Any Specifies a generic operand.
	enum class OperandType : unsigned long
	{
		Custom = URCL_OperandType_None,
		IndexedRegister = URCL_OperandType_IndexedRegister,
		SpecialRegister = URCL_OperandType_SpecialRegister,
		Immediate = URCL_OperandType_Immediate,
		MemoryAddress = URCL_OperandType_MemoryAddress,
		Label = URCL_OperandType_Label,
		Relative = URCL_OperandType_Relative,
		Port = URCL_OperandType_Port,
		Any = URCL_OperandType_Any
	};

	////namespace URCL
	////description Specifies types of URCL headers.
	////field Custom 0 Specifies a user-defined header.
	////field Bits 1 Specifies the BITS header.
	////field MinimumRegisters 2 Specifies the MINREG header.
	////field MinimumHeap 3 Specifies the MINHEAP header.
	////field MinimumStack 4 Specifies the MINSTACK header.
	////field InstructionStorage 5 Specifies the RUN header.
	enum class HeaderType : unsigned long
	{
		Custom = 0,
		Bits = 1,
		MinimumRegisters = 2,
		MinimumHeap = 3,
		MinimumStack = 4,
		InstructionStorage = 5
	};

	////namespace URCL
	////description The abstract base class for all operands.
	class Operand
	{
		public:
			////class Operand
			////namespace URCL
			////description Returns the type of the operand.
			virtual OperandType GetOperandType() = 0;
			////class Operand
			////namespace URCL
			////description Returns a string representation of the operand.
			virtual std::wstring ToString() = 0;
	};

	////namespace URCL
	////inherits URCL::Operand
	////description The abstract base class for operands with a numeric value.
	class NumericOperand : public virtual Operand
	{
		public:
			////class NumericOperand
			////namespace URCL
			////description Returns the numeric value of the operand as a signed long long.
			virtual long long GetSignedValue() = 0;
			////class NumericOperand
			////namespace URCL
			////description Returns the numeric value of the operand as an unsigned long long.
			virtual unsigned long long GetUnsignedValue() = 0;
	};

	////namespace URCL
	////inherits URCL::Operand
	////description The abstract base class for operands with a string value.
	class StringOperand : public virtual Operand
	{
		public:
			////class StringOperand
			////namespace URCL
			////description Returns the string value of the operand.
			virtual std::wstring GetStringValue() = 0;

			std::wstring ToString() { return GetStringValue(); }
	};

	////namespace URCL
	////inherits URCL::StringOperand
	////description A generic string operand.
	class AnyOperand : public StringOperand
	{
		private:
			std::wstring Value;
		
		public:
			////type ctor
			////class AnyOperand
			////namespace URCL
			////description Creates a generic operand with the specified string.
			AnyOperand(std::wstring value)
			{
				Value = value;
			}

			OperandType GetOperandType() { return OperandType::Any; }

			std::wstring GetStringValue() { return Value; }
	};

	////namespace URCL
	////inherits URCL::NumericOperand
	////description An operand that references a general-purpose register.
	class IndexedRegisterOperand : public NumericOperand
	{
		private:
			unsigned long long Index;
		
		public:
			////type ctor
			////class IndexedRegisterOperand
			////namespace URCL
			////description Creates a general-purpose register operand with the specified index.
			IndexedRegisterOperand(unsigned long long index)
			{
				Index = index;
			}

			OperandType GetOperandType() { return OperandType::IndexedRegister; }

			long long GetSignedValue() { return (long long)Index; }
			unsigned long long GetUnsignedValue() { return Index; }

			std::wstring ToString()
			{
				return L"R" + std::to_wstring(Index);
			}
	};

	////namespace URCL
	////inherits URCL::StringOperand
	////description An operand that references a special-purpose register.
	class SpecialRegisterOperand : public StringOperand
	{
		private:
			std::wstring Name;
		
		public:
			////type ctor
			////class SpecialRegisterOperand
			////namespace URCL
			////description Creates a special-purpose register operand with the specified name.
			SpecialRegisterOperand(std::wstring name)
			{
				Name = name;
				URCL_StringToUpper(&Name);
			}

			OperandType GetOperandType() { return OperandType::SpecialRegister; }

			std::wstring GetStringValue() { return Name; }
	};

	////namespace URCL
	////inherits URCL::NumericOperand
	////description An operand that is an immediate numeric value.
	class ImmediateOperand : public NumericOperand
	{
		private:
			unsigned long long Value;
		
		public:
			////type ctor
			////class ImmediateOperand
			////namespace URCL
			////description Creates an immediate operand with the specified value.
			ImmediateOperand(unsigned long long value)
			{
				Value = value;
			}

			OperandType GetOperandType() { return OperandType::Immediate; }

			long long GetSignedValue() { return (long long)Value; }
			unsigned long long GetUnsignedValue() { return Value; }

			std::wstring ToString()
			{
				return std::to_wstring(Value);
			}
	};

	////namespace URCL
	////inherits URCL::NumericOperand
	////description An operand that is an immediate memory address.
	class MemoryAddressOperand : public NumericOperand
	{
		private:
			unsigned long long Address;

		public:
			////type ctor
			////class MemoryAddressOperand
			////namespace URCL
			////description Creates a memory address operand with the specified address.
			MemoryAddressOperand(unsigned long long address)
			{
				Address = address;
			}

			OperandType GetOperandType() { return OperandType::MemoryAddress; }

			long long GetSignedValue() { return (long long)Address; }
			unsigned long long GetUnsignedValue() { return Address; }

			std::wstring ToString()
			{
				return L"M" + std::to_wstring(Address);
			}
	};

	////namespace URCL
	////inherits URCL::StringOperand
	////inherits URCL::NumericOperand
	////description An operand that references a label address.
	class LabelOperand : public StringOperand, public NumericOperand
	{
		private:
			Label* LabelRef;
		
		public:
			////type ctor
			////class LabelOperand
			////namespace URCL
			////description Creates a label operand with the specified name and address.
			LabelOperand(std::wstring name, unsigned long long address)
			{
				LabelRef = new Label(name, address);
			}

			////type ctor
			////class LabelOperand
			////namespace URCL
			////description Creates a label operand with the specified label.
			LabelOperand(Label* label)
			{
				LabelRef = label;
			}

			~LabelOperand() { delete LabelRef; }

			OperandType GetOperandType() { return OperandType::Label; }

			long long GetSignedValue() { return (long long)LabelRef->GetAddress(); }
			unsigned long long GetUnsignedValue() { return LabelRef->GetAddress(); }

			std::wstring GetStringValue() { return LabelRef->GetName(); }

			////class LabelOperand
			////namespace URCL
			////description Returns the label referenced by the operand.
			Label* GetLabel()
			{
				return LabelRef;
			}

			////class LabelOperand
			////namespace URCL
			////description Sets the label referenced by the operand.
			void SetLabel(Label* label)
			{
				LabelRef = label;
			}
	};

	////namespace URCL
	////inherits URCL::NumericOperand
	////description An operand that references a relative address.
	class RelativeOperand : public NumericOperand
	{
		private:
			long long Offset;
		
		public:
			////type ctor
			////class RelativeOperand
			////namespace URCL
			////description Creates a relative address operand with the specified offset.
			RelativeOperand(long long offset)
			{
				Offset = offset;
			}

			OperandType GetOperandType() { return OperandType::Relative; }

			long long GetSignedValue() { return Offset; }
			unsigned long long GetUnsignedValue() { return (unsigned long long)Offset; }

			std::wstring ToString()
			{
				return L"~" + std::to_wstring(Offset);
			}
	};

	////namespace URCL
	////inherits StringOperand
	////description An operand that references a named port.
	class PortOperand : public StringOperand
	{
		private:
			std::wstring Port;
		
		public:
			////type ctor
			////class PortOperand
			////namespace URCL
			////description Creates a named port operand with the specified name.
			PortOperand(std::wstring name)
			{
				Port = name;
				URCL_StringToUpper(&Port);
			}

			OperandType GetOperandType() { return OperandType::Port; }

			std::wstring GetStringValue() { return Port; }
	};

	////namespace URCL
	////description An operation with any number of operands.
	class Instruction
	{
		private:
			std::wstring Operation;
			std::vector<Operand*> Operands;
		
		public:
			////type ctor
			////class Instruction
			////namespace URCL
			////description Creates an empty instruction.
			Instruction()
			{}

			////type ctor
			////class Instruction
			////namespace URCL
			////description Creates an instruction with the specified operation.
			Instruction(std::wstring operation)
			{
				Operation = operation;
				URCL_StringToUpper(&Operation);
			}

			~Instruction()
			{
				for (size_t i = 0; i < Operands.size(); i++) delete Operands[i];
			}

			////class Instruction
			////namespace URCL
			////description Returns the operation for the instruction.
			std::wstring GetOperation()
			{
				return Operation;
			}

			////class Instruction
			////namespace URCL
			////description Returns the number of operands in the instruction.
			size_t GetOperandCount()
			{
				return Operands.size();
			}

			////class Instruction
			////namespace URCL
			////description Returns the operand with the specified index.
			Operand* GetOperand(size_t index)
			{
				return Operands[index];
			}

			////class Instruction
			////namespace URCL
			////description Sets the operation for the instruction.
			void SetOperation(std::wstring operation)
			{
				Operation = operation;
			}

			////class Instruction
			////namespace URCL
			////description Sets the operand at the specified index to a label operand with the specified label.
			void SetOperand(size_t index, Label* label)
			{
				Operands[index] = new LabelOperand(label);
			}

			////class Instruction
			////namespace URCL
			////description Sets the operand at the specified index to the specified operand.
			void SetOperand(size_t index, Operand* operand)
			{
				Operands[index] = operand;
			}

			////class Instruction
			////namespace URCL
			////description Adds the specified operand to the list of operands in the instruction.
			void AddOperand(Operand* operand)
			{
				Operands.push_back(operand);
			}

			////class Instruction
			////namespace URCL
			////description Returns a string representation of the instruction.
			std::wstring ToString()
			{
				std::wstring operands = L"";

				for (Operand* operand : Operands) operands += L" " + operand->ToString();

				return Operation + operands;
			}
	};

	////namespace URCL
	////description A header with an optional argument.
	class Header
	{
		private:
			std::wstring Name;
			Operand* Argument;
		
		public:
			////type ctor
			////class Header
			////namespace URCL
			////description Creates a header with the specified name.
			Header(std::wstring name)
			{
				Name = name;
				URCL_StringToUpper(&name);
				Argument = 0;
			}

			////type ctor
			////class Header
			////namespace URCL
			////description Creates a header with the specified name and argument.
			Header(std::wstring name, Operand* argument)
			{
				Name = name;
				URCL_StringToUpper(&name);
				Argument = argument;
			}

			~Header()
			{
				if (Argument != 0) delete Argument;
			}

			////class Header
			////namespace URCL
			////description Returns the name of the header.
			std::wstring GetName()
			{
				return Name;
			}

			////class Header
			////namespace URCL
			////description Returns true if the header argument is not null or false otherwise.
			bool HasArgument()
			{
				return Argument != 0;
			}

			////class Header
			////namespace URCL
			////description Returns the header argument or null if no argument is present.
			URCL::Operand* GetArgument()
			{
				return Argument;
			}

			////class Header
			////namespace URCL
			////description Returns the type of the header based on the name.
			HeaderType GetHeaderType()
			{
				if (Name == L"BITS") return HeaderType::Bits;
				else if (Name == L"MINREG") return HeaderType::MinimumRegisters;
				else if (Name == L"MINHEAP") return HeaderType::MinimumHeap;
				else if (Name == L"MINSTACK") return HeaderType::MinimumStack;
				else if (Name == L"RUN") return HeaderType::InstructionStorage;
				else return HeaderType::Custom;
			}

			////class Header
			////namespace URCL
			////description Returns a string representation of the header.
			std::wstring ToString()
			{
				return Name + L" " + Argument->ToString();
			}
	};

	////namespace URCL
	////description An interface for exporting a finished program.
	class ExportTarget
	{
		public:
			////class ExportTarget
			////namespace URCL
			////description Emits the specified instruction to the exported result.
			virtual void Emit(Instruction* instruction) = 0;
			////class ExportTarget
			////namespace URCL
			////description Adds the specified label to the exported result.
			virtual void AddLabel(Label* label) = 0;
			////class ExportTarget
			////namespace URCL
			////description Adds the specified header to the exported result.
			virtual void AddHeader(Header* header) = 0;
	};

	////namespace URCL
	////description A builder class for URCL programs.
	class Program
	{
		private:
			std::vector<Instruction*> Instructions;
			std::vector<Header*> Headers;
			std::vector<Label*> Labels;

		public:
			~Program()
			{
				Clear();
			}

			////class Program
			////namespace URCL
			////description Disposes of all instructions, labels, and headers and clears the internal lists.
			void Clear()
			{
				for (size_t i = 0; i < Instructions.size(); i++) delete Instructions[i];
				for (size_t i = 0; i < Labels.size(); i++) delete Labels[i];
				for (size_t i = 0; i < Headers.size(); i++) delete Headers[i];
				Instructions.clear();
				Labels.clear();
				Headers.clear();
			}

			////class Program
			////namespace URCL
			////description Parses the specified source and retains the instructions, labels, and headers.
			void AddSource(std::wstring source)
			{
				unsigned long length = source.length();
				URCL_Char* inputBuffer = new URCL_Char[length];
				for (size_t i = 0; i < length; i++) inputBuffer[i] = (URCL_Char)source[i];

				URCL_InstructionInfo* instructions;
				URCL_LabelInfo* labels;
				URCL_InstructionInfo* headers;
				unsigned long instructionCount;
				unsigned long labelsCount;
				unsigned long headersCount;

				if (URCL_TryParseSource(inputBuffer, length, &instructions, &labels, &headers, &instructionCount, &labelsCount, &headersCount))
				{
					for (unsigned long i = 0; i < headersCount; i++)
					{
						URCL_InstructionInfo header = headers[i];
						std::wstring name = URCL_CharsToString(header.Operation, URCL_CharsLength(header.Operation));

						if (header.Operands[0].Type == URCL_OperandType_None)
						{
							AddHeader(name);
						}
						else
						{
							try
							{
								AddHeader(name, GetOperandFromInfo(header.Operands[0]));
							}
							catch (ParserError ex)
							{
								URCL_FreeInstructions(headers, headersCount);
								URCL_FreeLabels(labels, labelsCount);
								URCL_FreeInstructions(instructions, instructionCount);
								throw ex;
							}
						}
					}

					URCL_FreeInstructions(headers, headersCount);

					for (unsigned long i = 0; i < labelsCount; i++)
					{
						URCL_LabelInfo label = labels[i];
						AddLabel(URCL_CharsToString(label.Name, URCL_CharsLength(label.Name)), label.Address + Instructions.size());
					}

					URCL_FreeLabels(labels, labelsCount);

					for (unsigned long i = 0; i < instructionCount; i++)
					{
						URCL_InstructionInfo instruction = instructions[i];
						Instruction* instructionObj = new Instruction(URCL_CharsToString(instruction.Operation, URCL_CharsLength(instruction.Operation)));
						
						for (unsigned long j = 0; (j < URCL_MaxOperands) && (instruction.Operands[j].Type != URCL_OperandType_None); j++)
						{
							URCL_OperandInfo operand = instruction.Operands[j];

							try
							{
								instructionObj->AddOperand(GetOperandFromInfo(operand));
							}
							catch (ParserError ex)
							{
								URCL_FreeInstructions(instructions, instructionCount);
								throw ex;
							}
						}

						Emit(instructionObj);
					}

					URCL_FreeInstructions(instructions, instructionCount);
				}
				else
				{
					throw ParserError(URCL_CharsToString(URCL_Error.Message, URCL_CharsLength(URCL_Error.Message)), (unsigned long)(URCL_Error.Position - inputBuffer), URCL_Error.Length);
				}
			}

			////class Program
			////namespace URCL
			////description Adds a header with the specified name.
			void AddHeader(std::wstring name)
			{
				Headers.push_back(new Header(name));
			}

			////class Program
			////namespace URCL
			////description Adds a header with the specified name and argument.
			void AddHeader(std::wstring name, Operand* argument)
			{
				Headers.push_back(new Header(name, argument));
			}

			////class Program
			////namespace URCL
			////description Adds a generic header with the specified name and value.
			void AddHeader(std::wstring name, std::wstring value)
			{
				Headers.push_back(new Header(name, new AnyOperand(value)));
			}

			////class Program
			////namespace URCL
			////description Adds the specified header.
			void AddHeader(Header* header)
			{
				Headers.push_back(header);
			}

			////class Program
			////namespace URCL
			////description Adds an instruction with the specified operation.
			void Emit(std::wstring operation)
			{
				Emit(new Instruction(operation));
			}

			////class Program
			////namespace URCL
			////description Adds an instruction with the specified operation and operand.
			void Emit(std::wstring operation, Operand* operandA)
			{
				Instruction* instruction = new Instruction(operation);
				instruction->AddOperand(operandA);
				Emit(instruction);
			}

			////class Program
			////namespace URCL
			////description Adds an instruction with the specified operation and two operands.
			void Emit(std::wstring operation, Operand* operandA, Operand* operandB)
			{
				Instruction* instruction = new Instruction(operation);
				instruction->AddOperand(operandA);
				instruction->AddOperand(operandB);
				Emit(instruction);
			}

			////class Program
			////namespace URCL
			////description Adds an instruction with the specified operation and three operands.
			void Emit(std::wstring operation, Operand* operandA, Operand* operandB, Operand* operandC)
			{
				Instruction* instruction = new Instruction(operation);
				instruction->AddOperand(operandA);
				instruction->AddOperand(operandB);
				instruction->AddOperand(operandC);
				Emit(instruction);
			}

			////class Program
			////namespace URCL
			////description Adds the specified instruction.
			void Emit(Instruction* instruction)
			{
				Instructions.push_back(instruction);
			}

			////class Program
			////namespace URCL
			////description Adds an unnamed label at the current address.
			Label* MarkLabel()
			{
				return MarkLabel(L"");
			}

			////class Program
			////namespace URCL
			////description Adds a label at the current address with the specified name.
			Label* MarkLabel(std::wstring name)
			{
				return AddLabel(name, Instructions.size());
			}

			////class Program
			////namespace URCL
			////description Adds a label with the specified name and address.
			Label* AddLabel(std::wstring name, unsigned long long address)
			{
				Label* result = new Label(name, address);
				Labels.push_back(result);
				return result;
			}

			////class Program
			////namespace URCL
			////description Adds the specified label.
			void AddLabel(Label* label)
			{
				Labels.push_back(label);
			}

			////class Program
			////namespace URCL
			////description Exports the entire program to the specified target. Optionally exports labels before instructions and headers if exportLabelsFirst is true.
			void Export(ExportTarget* target, bool exportLabelsFirst)
			{
				if (exportLabelsFirst) for (Label* label : Labels) target->AddLabel(label);
				for (Header* header : Headers) target->AddHeader(header);
				for (Instruction* instruction : Instructions) target->Emit(instruction);
				if (!exportLabelsFirst) for (Label* label : Labels) target->AddLabel(label);
			}

			////class Program
			////namespace URCL
			////description Returns a string representation of the program.
			std::wstring ToString()
			{
				std::wstring result = L"";

				for (Header* header : Headers) result += header->ToString();
				for (size_t i = 0; i < Instructions.size(); i++)
				{
					for (Label* label : Labels)
					{
						if (label->GetAddress() == i) result += label->GetName() + L"\n";
					}

					result += Instructions[i]->ToString() + L"\n";
				}

				return result;
			}

		private:
			Operand* GetOperandFromInfo(URCL_OperandInfo operand)
			{
				switch ((OperandType)operand.Type)
				{
					case OperandType::Any:
						return new AnyOperand(URCL_CharsToString(operand.StringValue, URCL_CharsLength(operand.StringValue)));
					case OperandType::IndexedRegister:
						return new IndexedRegisterOperand(operand.ImmediateValue);
					case OperandType::SpecialRegister:
						return new SpecialRegisterOperand(URCL_CharsToString(operand.StringValue, URCL_CharsLength(operand.StringValue)));
					case OperandType::Immediate:
						return new ImmediateOperand(operand.ImmediateValue);
					case OperandType::MemoryAddress:
						return new MemoryAddressOperand(operand.ImmediateValue);
					case OperandType::Label:
						{
							std::wstring labelName = URCL_CharsToString(operand.StringValue, URCL_CharsLength(operand.StringValue));
							
							for (Label* label : Labels) if (labelName == label->GetName()) return new LabelOperand(label);
							
							throw ParserError(L"No label found with the name \"" + labelName + L"\".");
						}
						break;
					case OperandType::Relative:
						return new RelativeOperand((long long)operand.ImmediateValue);
						break;
					case OperandType::Port:
						return new PortOperand(URCL_CharsToString(operand.StringValue, URCL_CharsLength(operand.StringValue)));
						break;
					default:
						throw ParserError(L"Unknown operand type was generated.");
				}
			}
	};
}