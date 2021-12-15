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

	////id ParserError
	////namespace URCL
	class ParserError : public std::exception
	{
		private:
			std::wstring Message;
			unsigned long Position;
			unsigned long Length;

		public:
			ParserError(std::wstring message)
			{
				Message = message;
				Position = 0;
				Length = 0;
			}

			ParserError(std::wstring message, unsigned long position, unsigned long length)
			{
				Message = message;
				Position = position;
				Length = length;
			}

			std::wstring GetMessage() { return Message; }
			unsigned long long GetPosition() { return Position; }
	};

	////id OperandType
	////namespace URCL
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

	////id HeaderType
	////namespace URCL
	enum class HeaderType : unsigned long
	{
		Custom = 0,
		Bits = 1,
		MinimumRegisters = 2,
		MinimumHeap = 3,
		MinimumStack = 4,
		InstructionStorage = 5
	};

	////id Operand
	////namespace URCL
	class Operand
	{
		public:
			virtual OperandType GetOperandType() = 0;
			virtual std::wstring ToString() = 0;
	};

	////id NumericOperand
	////namespace URCL
	class NumericOperand : public virtual Operand
	{
		public:
			virtual long long GetSignedValue() = 0;
			virtual unsigned long long GetUnsignedValue() = 0;
	};

	////id StringOperand
	////namespace URCL
	class StringOperand : public virtual Operand
	{
		public:
			virtual std::wstring GetStringValue() = 0;

			std::wstring ToString() { return GetStringValue(); }
	};

	////id AnyOperand
	////namespace URCL
	class AnyOperand : public StringOperand
	{
		private:
			std::wstring Value;
		
		public:
			AnyOperand(std::wstring value) { Value = value; }

			OperandType GetOperandType() { return OperandType::Any; }

			std::wstring GetStringValue() { return Value; }
	};

	////id IndexedRegisterOperand
	////namespace URCL
	class IndexedRegisterOperand : public NumericOperand
	{
		private:
			unsigned long long Index;
		
		public:
			IndexedRegisterOperand(unsigned long long index) { Index = index; }

			OperandType GetOperandType() { return OperandType::IndexedRegister; }

			long long GetSignedValue() { return (long long)Index; }
			unsigned long long GetUnsignedValue() { return Index; }

			std::wstring ToString()
			{
				return L"R" + std::to_wstring(Index);
			}
	};

	////id SpecialRegisterOperand
	////namespace URCL
	class SpecialRegisterOperand : public StringOperand
	{
		private:
			std::wstring Name;
		
		public:
			SpecialRegisterOperand(std::wstring name)
			{
				Name = name;
				URCL_StringToUpper(&Name);
			}

			OperandType GetOperandType() { return OperandType::SpecialRegister; }

			std::wstring GetStringValue() { return Name; }
	};

	////id ImmediateOperand
	////namespace URCL
	class ImmediateOperand : public NumericOperand
	{
		private:
			unsigned long long Value;
		
		public:
			ImmediateOperand(unsigned long long value) { Value = value; }

			OperandType GetOperandType() { return OperandType::Immediate; }

			long long GetSignedValue() { return (long long)Value; }
			unsigned long long GetUnsignedValue() { return Value; }

			std::wstring ToString()
			{
				return std::to_wstring(Value);
			}
	};

	////id MemoryAddressOperand
	////namespace URCL
	class MemoryAddressOperand : public NumericOperand
	{
		private:
			unsigned long long Address;

		public:
			MemoryAddressOperand(unsigned long long address) { Address = address; }

			OperandType GetOperandType() { return OperandType::MemoryAddress; }

			long long GetSignedValue() { return (long long)Address; }
			unsigned long long GetUnsignedValue() { return Address; }

			std::wstring ToString()
			{
				return L"M" + std::to_wstring(Address);
			}
	};

	////id LabelOperand
	////namespace URCL
	class LabelOperand : public StringOperand, public NumericOperand
	{
		private:
			URCL::Label* Label;
		
		public:
			LabelOperand(std::wstring name, unsigned long long address) { Label = new URCL::Label(name, address); }
			LabelOperand(URCL::Label* label) { Label = label; }
			~LabelOperand() { delete Label; }

			OperandType GetOperandType() { return OperandType::Label; }

			long long GetSignedValue() { return (long long)Label->GetAddress(); }
			unsigned long long GetUnsignedValue() { return Label->GetAddress(); }

			std::wstring GetStringValue() { return Label->GetName(); }

			URCL::Label* GetLabel() { return Label; }
			void SetLabel(URCL::Label* label) { Label = label; }
	};

	////id RelativeOperand
	////namespace URCL
	class RelativeOperand : public NumericOperand
	{
		private:
			long long Offset;
		
		public:
			RelativeOperand(long long offset) { Offset = offset; }

			OperandType GetOperandType() { return OperandType::Relative; }

			long long GetSignedValue() { return Offset; }
			unsigned long long GetUnsignedValue() { return (unsigned long long)Offset; }

			std::wstring ToString()
			{
				return L"~" + std::to_wstring(Offset);
			}
	};

	////id PortOperand
	////namespace URCL
	class PortOperand : public StringOperand
	{
		private:
			std::wstring Port;
		
		public:
			PortOperand(std::wstring name)
			{
				Port = name;
				URCL_StringToUpper(&Port);
			}

			OperandType GetOperandType() { return OperandType::Port; }

			std::wstring GetStringValue() { return Port; }
	};

	////id Instruction
	////namespace URCL
	class Instruction
	{
		private:
			std::wstring Operation;
			std::vector<Operand*> Operands;
		
		public:
			Instruction() {}

			Instruction(std::wstring operation)
			{
				Operation = operation;
				URCL_StringToUpper(&Operation);
			}

			~Instruction()
			{
				for (size_t i = 0; i < Operands.size(); i++) delete Operands[i];
			}

			std::wstring GetOperation() { return Operation; }
			size_t GetOperandCount() { return Operands.size(); }
			Operand* GetOperand(size_t index) { return Operands[index]; }

			void SetOperation(std::wstring operation) { Operation = operation; }
			void SetOperand(size_t index, Label* label) { Operands[index] = new LabelOperand(label); }
			void SetOperand(size_t index, Operand* operand) { Operands[index] = operand; }
			void AddOperand(Operand* operand) { Operands.push_back(operand); }
	};

	////id Header
	////namespace URCL
	class Header
	{
		private:
			std::wstring Name;
			Operand* Operand;
		
		public:
			Header(std::wstring name)
			{
				Name = name;
				URCL_StringToUpper(&name);
				Operand = 0;
			}

			Header(std::wstring name, URCL::Operand* operand)
			{
				Name = name;
				URCL_StringToUpper(&name);
				Operand = operand;
			}

			~Header()
			{
				if (Operand != 0) delete Operand;
			}

			std::wstring GetName() { return Name; }

			bool HasOperand() { return Operand != 0; }

			URCL::Operand* GetOperand() { return Operand; }

			HeaderType GetHeaderType()
			{
				if (Name == L"BITS") return HeaderType::Bits;
				else if (Name == L"MINREG") return HeaderType::MinimumRegisters;
				else if (Name == L"MINHEAP") return HeaderType::MinimumHeap;
				else if (Name == L"MINSTACK") return HeaderType::MinimumStack;
				else if (Name == L"RUN") return HeaderType::InstructionStorage;
				else return HeaderType::Custom;
			}
	};

	////id Label
	////namespace URCL
	class Label
	{
		private:
			std::wstring Name;
			unsigned long long Address;

		public:
			Label(std::wstring name, unsigned long long address)
			{
				Name = name;
				Address = address;
			}

			std::wstring GetName() { return Name; }
			unsigned long long GetAddress() { return Address; }
	};

	////id ExportTarget
	////namespace URCL
	class ExportTarget
	{
		public:
			virtual void Emit(Instruction* instruction) = 0;
			virtual void AddLabel(Label* label) = 0;
	};

	////id Program
	////namespace URCL
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

			void Clear()
			{
				for (size_t i = 0; i < Instructions.size(); i++) delete Instructions[i];
				for (size_t i = 0; i < Labels.size(); i++) delete Labels[i];
				for (size_t i = 0; i < Headers.size(); i++) delete Headers[i];
				Instructions.clear();
				Labels.clear();
				Headers.clear();
			}

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

			void AddHeader(std::wstring name)
			{
				Headers.push_back(new Header(name));
			}

			void AddHeader(std::wstring name, Operand* operand)
			{
				Headers.push_back(new Header(name, operand));
			}

			void AddHeader(std::wstring name, std::wstring value)
			{
				Headers.push_back(new Header(name, new AnyOperand(value)));
			}

			void AddHeader(Header* header)
			{
				Headers.push_back(header);
			}

			void Emit(std::wstring operation)
			{
				Emit(new Instruction(operation));
			}

			void Emit(std::wstring operation, Operand* operandA)
			{
				Instruction* instruction = new Instruction(operation);
				instruction->AddOperand(operandA);
				Emit(instruction);
			}

			void Emit(std::wstring operation, Operand* operandA, Operand* operandB)
			{
				Instruction* instruction = new Instruction(operation);
				instruction->AddOperand(operandA);
				instruction->AddOperand(operandB);
				Emit(instruction);
			}

			void Emit(std::wstring operation, Operand* operandA, Operand* operandB, Operand* operandC)
			{
				Instruction* instruction = new Instruction(operation);
				instruction->AddOperand(operandA);
				instruction->AddOperand(operandB);
				instruction->AddOperand(operandC);
				Emit(instruction);
			}

			void Emit(Instruction* instruction)
			{
				Instructions.push_back(instruction);
			}

			Label* MarkLabel()
			{
				return MarkLabel(L"");
			}

			Label* MarkLabel(std::wstring name)
			{
				return AddLabel(name, Instructions.size());
			}

			Label* AddLabel(std::wstring name, unsigned long long address)
			{
				Label* result = new Label(name, address);
				Labels.push_back(result);
				return result;
			}

			void AddLabel(Label* label)
			{
				Labels.push_back(label);
			}

			void Export(ExportTarget* target)
			{
				Export(target, false);
			}

			void Export(ExportTarget* target, bool exportLabelsFirst)
			{
				if (exportLabelsFirst) for (Label* label : Labels) target->AddLabel(label);
				for (Instruction* instruction : Instructions) target->Emit(instruction);
				if (!exportLabelsFirst) for (Label* label : Labels) target->AddLabel(label);
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