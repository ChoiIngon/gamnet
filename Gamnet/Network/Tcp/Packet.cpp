/*
 * Packet.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#include <mutex>
#include "Packet.h"
#include "../../Library/Pool.h"
//#include <atomic>

//#include <Windows.h>
//#include <DbgHelp.h>
//#include <Winbase.h>

namespace Gamnet { namespace Network { namespace Tcp {

	static Pool<Packet, std::mutex, Packet::Init, Packet::Release> packetPool_(65535);
	std::shared_ptr<Packet> Packet::Create()
	{
		return packetPool_.Create();
	}
	size_t Packet::PoolSize()
	{
		return packetPool_.Size();
	}
	size_t Packet::PoolAvailable()
	{
		return packetPool_.Available();
	}
	size_t Packet::PoolCapacity()
	{
		return packetPool_.Capacity();
	}
	/*
	static std::atomic<int> create_count;
	static std::atomic<int> release_count;
	
	class sym_handler
	{
	public:
		static sym_handler& get_instance()
		{
			static sym_handler instance;
			return instance;
		}

		std::string get_symbol_info(DWORD64 addr)
		{

			char tmp[1024];

			std::stringstream ss;
			DWORD64 displacement64;
			DWORD displacement;
			char symbol_buffer[sizeof(SYMBOL_INFO) + 256];
			SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = 255;

			IMAGEHLP_LINE64 line;
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			//ss << boost::format("[0x%08X] ") % addr;  
			
			if (m_initialized)
			{
				if (SymFromAddr(GetCurrentProcess(), addr, &displacement64, symbol))
				{
					if (SymGetLineFromAddr64(GetCurrentProcess(), addr, &displacement, &line))
					{
						//ss << (boost::format(" (%s:%d)") % line.FileName % line.LineNumber).str();  						
						ss << Format("File Name: ", line.FileName, ", line No: ", line.LineNumber);
					}
				}
				else {

					DWORD err = GetLastError();
				}
			}
			return ss.str();
		}

		void capture_stack_trace(CONTEXT* context, DWORD64* frame_ptrs, size_t count, size_t skip)
		{
			if (m_initialized)
			{
				CONTEXT current_context;
				if (!context)
				{
					RtlCaptureContext(&current_context);
					context = &current_context;
				}

				DWORD machine_type;
				STACKFRAME64 frame;
				ZeroMemory(&frame, sizeof(frame));
				frame.AddrPC.Mode = AddrModeFlat;
				frame.AddrFrame.Mode = AddrModeFlat;
				frame.AddrStack.Mode = AddrModeFlat;
#ifdef _M_X64  
				frame.AddrPC.Offset = context->Rip;
				frame.AddrFrame.Offset = context->Rbp;
				frame.AddrStack.Offset = context->Rsp;
				machine_type = IMAGE_FILE_MACHINE_AMD64;
#else  
				frame.AddrPC.Offset = context->Eip;
				frame.AddrPC.Offset = context->Ebp;
				frame.AddrPC.Offset = context->Esp;
				machine_type = IMAGE_FILE_MACHINE_I386;
#endif  
				for (size_t i = 0; i < count + skip; i++)
				{
					if (StackWalk64(machine_type,
						GetCurrentProcess(),
						GetCurrentThread(),
						&frame,
						context,
						NULL,
						SymFunctionTableAccess64,
						SymGetModuleBase64,
						NULL))
					{
						if (i >= skip)
						{
							frame_ptrs[i - skip] = frame.AddrPC.Offset;
						}
					}
					else
					{
						break;
					}
				}
			}
		}

	private:
		sym_handler()
		{
			m_initialized = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;
		}

		~sym_handler()
		{
			if (m_initialized)
			{
				SymCleanup(GetCurrentProcess());
				m_initialized = false;
			}
		}

		bool m_initialized;
	};



	class stack_trace
	{
	public:
		stack_trace(CONTEXT* context, size_t skip)
		{
			ZeroMemory(m_frame_ptrs, sizeof(m_frame_ptrs));
			sym_handler::get_instance().capture_stack_trace(context,
				m_frame_ptrs,
				max_frame_ptrs,
				skip);

		}

		std::string to_string() const
		{
			std::stringstream ss;
			for (size_t i = 0; i < max_frame_ptrs && m_frame_ptrs[i]; ++i)
			{
				ss << sym_handler::get_instance().get_symbol_info(m_frame_ptrs[i]) << "\n";
			}
			return ss.str();
		}

	private:
		static const size_t max_frame_ptrs = 16;
		DWORD64 m_frame_ptrs[max_frame_ptrs];
	};



	inline std::string to_string(const stack_trace& trace)
	{
		return trace.to_string();
	}
	*/
	Packet* Packet::Init::operator() (Packet* packet)
	{
		packet->Clear();
		packet->length = 0;
		packet->msg_seq = 0;
		packet->msg_id = 0;
		packet->reliable = false;
		//stack_trace *sttrace = new stack_trace(NULL, 0);
		//MessageBoxA(NULL, sttrace->to_string().c_str(), "Stack Trace", 0);
		//OutputDebugStringA(sttrace->to_string().c_str());
		//delete sttrace;
		return packet;
	};

	Packet* Packet::Release::operator() (Packet* packet)
	{
		return packet;
	}
	
	Packet::Packet() : Buffer(Buffer::MAX_SIZE)
	{
	}

	Packet::~Packet()
	{
	}

	bool Packet::Write(uint32_t msgID, const char* buf, size_t bytes)
	{
		Clear();
		length = (uint16_t)(HEADER_SIZE + bytes);
		msg_id = msgID;

		if (false == WriteHeader())
		{
			return false;
		}

		if (nullptr != buf && 0 < bytes)
		{
			std::memcpy(data + HEADER_SIZE, buf, bytes);
		}
		this->write_index += length;
		return true;
	}

	bool Packet::WriteHeader()
	{
		if (Capacity() <= length)
		{
			LOG(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", length, ")");
			return false;
		}

		if ((uint16_t)Available() < length)
		{
			Resize(length);
		}

		(*(uint16_t*)(data + /*write_index +*/ OFFSET_LENGTH)) = length;
		(*(uint32_t*)(data + /*write_index +*/OFFSET_MSGSEQ)) = msg_seq;
		(*(uint32_t*)(data + /*write_index +*/OFFSET_MSGID)) = msg_id;
		(*(uint8_t*)(data + /*write_index +*/OFFSET_RELIABLE)) = reliable;
		(*(uint8_t*)(data + /*write_index +*/OFFSET_RESERVED)) = 0;

		return true;
	}

	bool Packet::ReadHeader()
	{
		length = *((uint16_t*)(data + read_index + OFFSET_LENGTH));
		msg_seq = *((uint32_t*)(data + read_index + OFFSET_MSGSEQ));
		msg_id = *((uint32_t*)(data + read_index + OFFSET_MSGID));
		reliable = *((uint8_t*)(data + read_index + OFFSET_RELIABLE));
		return true;
	}
}}}
