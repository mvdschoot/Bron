#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace bron {
using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

struct ProfileResult {
	std::string name;

	FloatingPointMicroseconds start;
	std::chrono::microseconds elapsed_time;
	std::thread::id thread_id;
};

struct InstrumentationSession {
	std::string name;
};

class Instrumentor {
public:
	Instrumentor(const Instrumentor&) = delete;
	Instrumentor(Instrumentor&&) = delete;

	void BeginSession(const std::string& name, const std::string& filepath = "results.json") {
		std::lock_guard<std::mutex> lock(m_Mutex);
		if (current_session_) {
			// If there is already a current session, then close it before beginning new one.
			// Subsequent profiling output meant for the original session will end up in the
			// newly opened session instead.  That's better than having badly formatted
			// profiling output.
			if (Logger::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
			{
				BR_CORE_INFO("Instrumentor::BeginSession('{}') when session '{}' already open.", name,
							 current_session_->name);
			}
			InternalEndSession();
		}
		output_stream_.open(filepath);

		if (output_stream_.is_open()) {
			current_session_ = new InstrumentationSession({name});
			WriteHeader();
		} else {
			if (Logger::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
			{
				BR_CORE_ERROR("Instrumentor could not open results file '{}'.", filepath);
			}
		}
		BR_CORE_TRACE("Trace session has begun...");
	}

	void EndSession() {
		std::lock_guard<std::mutex> lock(m_Mutex);
		InternalEndSession();
		BR_CORE_TRACE("Trace session has ended...");
	}

	void WriteProfile(const ProfileResult& result) {
		m_Result = result;
		std::stringstream json;

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.elapsed_time.count()) << ',';
		json << "\"name\":\"" << result.name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.thread_id << ",";
		json << "\"ts\":" << result.start.count();
		json << "}";

		std::lock_guard<std::mutex> lock(m_Mutex);
		if (current_session_) {
			output_stream_ << json.str();
			output_stream_.flush();
		}
	}

	ProfileResult GetResult() { return m_Result; }

	static Instrumentor& Get() {
		static Instrumentor instance;
		return instance;
	}

private:
	Instrumentor() : current_session_(nullptr) {}

	~Instrumentor() { EndSession(); }

	void WriteHeader() {
		output_stream_ << "{\"otherData\": {},\"traceEvents\":[{}";
		output_stream_.flush();
	}

	void WriteFooter() {
		output_stream_ << "]}";
		output_stream_.flush();
	}

	// Note: you must already own lock on m_Mutex before
	// calling InternalEndSession()
	void InternalEndSession() {
		if (current_session_) {
			WriteFooter();
			output_stream_.close();
			delete current_session_;
			current_session_ = nullptr;
		}
	}

private:
	std::mutex m_Mutex;
	InstrumentationSession* current_session_;
	std::ofstream output_stream_;
	ProfileResult m_Result;
};

class InstrumentationTimer {
public:
	InstrumentationTimer(const char* name) : name_(name), stopped_(false) {
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~InstrumentationTimer() {
		if (!stopped_)
			Stop();
	}

	void Stop() {
		auto end_timepoint = std::chrono::high_resolution_clock::now();
		auto high_res_start = FloatingPointMicroseconds{m_StartTimepoint.time_since_epoch()};
		auto elapsed_time =
				std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch() -
				std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

		Instrumentor::Get().WriteProfile({name_, high_res_start, elapsed_time, std::this_thread::get_id()});

		stopped_ = true;
	}

private:
	const char* name_;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool stopped_;
};

namespace instrumentor_utils {
template<size_t N>
struct ChangeResult {
	char data[N];
};

template<size_t N, size_t K>
constexpr auto CleanupOutputString(const char (&expr)[N], const char (&remove)[K]) {
	ChangeResult<N> result = {};

	size_t src_index = 0;
	size_t dst_index = 0;
	while (src_index < N) {
		size_t match_index = 0;
		while (match_index < K - 1 && src_index + match_index < N - 1 &&
			   expr[src_index + match_index] == remove[match_index])
			match_index++;
		if (match_index == K - 1)
			src_index += match_index;
		result.data[dst_index++] = expr[src_index] == '"' ? '\'' : expr[src_index];
		src_index++;
	}
	return result;
}
} // namespace instrumentor_utils
} // namespace bron

#if defined(BR_PROFILE)
  // Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) ||  \
			defined(__ghs__)
		#define BR_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define BR_FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define BR_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define BR_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define BR_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define BR_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define BR_FUNC_SIG __func__
	#else
		#define BR_FUNC_SIG "BR_FUNC_SIG unknown!"
	#endif

	#define BR_PROFILE_BEGIN_SESSION(name, filepath) ::bron::Instrumentor::Get().BeginSession(name, filepath)
	#define BR_PROFILE_END_SESSION() ::bron::Instrumentor::Get().EndSession()
	#define BR_PROFILE_SCOPE_LINE2(name, line)                                                                         \
		constexpr auto fixed_name##line = ::bron::instrumentor_utils::CleanupOutputString(name, "__cdecl ");           \
		::bron::InstrumentationTimer timer##line(fixed_name##line.data)
	#define BR_PROFILE_SCOPE_LINE(name, line) BR_PROFILE_SCOPE_LINE2(name, line)
	#define BR_PROFILE_SCOPE(name) BR_PROFILE_SCOPE_LINE(name, __LINE__)
	#define BR_PROFILE_FUNCTION() BR_PROFILE_SCOPE(BR_FUNC_SIG)
#else
	#define BR_PROFILE_BEGIN_SESSION(name, filepath)
	#define BR_PROFILE_END_SESSION()
	#define BR_PROFILE_SCOPE(name)
	#define BR_PROFILE_FUNCTION()
#endif
