#pragma once

#ifndef __H_LOG_WRITER_H__
#define __H_LOG_WRITER_H__

// write program log
// Add by humorly 2017/7/21

#include <ctime>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include "afxmt.h"
#include "direct.h"
#include<atlconv.h>

namespace wstd
{
#define __FILE_LINE__	__FILE__, __LINE__

	// std::size_t to string
	inline std::string to_string(std::size_t value);

	class log_writer;
	// log writer set
	class log_writer
	{
	public:
		log_writer() { InitializeCriticalSection(&cslock_); }
		virtual ~log_writer() { DeleteCriticalSection(&cslock_); }

		// lock
		void lock() 
		{
			EnterCriticalSection(&cslock_);
		}

		// unlock
		void unlock()
		{
			LeaveCriticalSection(&cslock_);
		}

		// write log
		static log_writer & log_store(const std::string str, unsigned short RoomID, unsigned short ChairID, char * file, int line, bool ClearFile = false)
		{
			static log_writer log_;
			log_.lock();
			log_.write(str, RoomID, ChairID, file, line, ClearFile);
			log_.unlock();
			return log_;
		}

	private:

		// 创建互斥锁
		CRITICAL_SECTION cslock_;

		enum
		{
			invalid_ptr = 0,
		};

		// write log
		//void write(const std::string & str, char * file = __FILE__, int line = __LINE__, std::string RoomID=std::string("")) 
		void write(const std::string & str, unsigned short RoomID, unsigned short ChairID, char * file = __FILE__, int line = __LINE__, bool ClearFile = false)
		{

			// get current time now
			std::time_t time_ = std::time(reinterpret_cast<time_t *>(invalid_ptr));
			tm * tm_ = std::localtime(&time_);
			assert(tm_);

			// make file name
			//std::string name_ = std::string("RoomID_") + RoomID.c_str() + std::string("_") + wstd::to_string(tm_->tm_year + 1900) + std::string("-") +
			//	wstd::to_string(tm_->tm_mon + 1) + std::string("-") + wstd::to_string(tm_->tm_mday) + std::string("-")+ wstd::to_string(tm_->tm_hour) + std::string(".log");
			std::string name_ = std::string("TableID-") +wstd::to_string(ChairID) /*+ std::string("_") + wstd::to_string(tm_->tm_year + 1900) + std::string("-") +
				wstd::to_string(tm_->tm_mon + 1) + std::string("-") + wstd::to_string(tm_->tm_mday) + std::string("-")+ wstd::to_string(tm_->tm_hour)*/ + std::string(".log");

			// make now time
			/*std::string now_time_ = wstd::to_string(tm_->tm_year + 1900) + std::string("-") +
				wstd::to_string(tm_->tm_mon + 1) + std::string("-") + wstd::to_string(tm_->tm_mday) + std::string("-") +
				wstd::to_string(tm_->tm_hour) + std::string(":") + wstd::to_string(tm_->tm_min) + std::string(":")
				+ wstd::to_string(tm_->tm_sec);*/
			//std::string now_time_ = wstd::to_string(tm_->tm_mday) + std::string("-") +
			//	wstd::to_string(tm_->tm_hour) + std::string(":") + wstd::to_string(tm_->tm_min) + std::string(":")
			//	+ wstd::to_string(tm_->tm_sec);
			std::string now_time_ = wstd::to_string(tm_->tm_hour) + std::string(":") + wstd::to_string(tm_->tm_min) + std::string(":")
				+ wstd::to_string(tm_->tm_sec);

			// cut file name
			//std::string file_(file);
			//std::size_t npos_ = file_.rfind("\\");
			//if (std::string::npos != npos_) 
			//{
			//	file_ = file_.substr(npos_);
			//}

			char strFlieName[255];
			getcwd(strFlieName,255);
			std::string strPath(strFlieName);

			strPath+="\\GameLog";
			_mkdir(strPath.c_str());

			strPath+="\\CheXuan";
			_mkdir(strPath.c_str());

			std::string strRoomID=std::string("\\RoomID-") + wstd::to_string(RoomID);
			strPath+=strRoomID;
			_mkdir(strPath.c_str());

			strPath+="\\";
			strPath.append(name_);

			// write log to file
			//std::ofstream of_(name_.c_str(), std::ios::out | std::ios::app);

			//std::ofstream of_ = (ClearFile) ? (std::ofstream of_(strPath.c_str(), std::ios::out | std::ios::trunc)) : (std::ofstream of_(strPath.c_str(), std::ios::out | std::ios::app));
			if(ClearFile)
			{
				std::ofstream of_(strPath.c_str(), std::ios::out | std::ios::trunc);
				if (of_.is_open())
				{
					//of_ << "[log start. ";
					//of_ << now_time_ << ". ";
					//of_ << "" << file_ << ". " << "line " << line << "]";
					//of_ <<  "line " << line << "]";
					of_ <<"["<< now_time_ << ". "<<line<<"]";
					of_ << " " << str << "\n";
					of_.close();
				}
			}
			else
			{
				std::ofstream of_(strPath.c_str(), std::ios::out | std::ios::app);
				if (of_.is_open())
				{
					//of_ << "[log start. ";
					//of_ << now_time_ << ". ";
					//of_ << "" << file_ << ". " << "line " << line << "]";
					//of_ <<  "line " << line << "]";
					of_ <<"["<< now_time_ << ". "<<line<<"]";
					of_ << " " << str << "\n";
					of_.close();
				}
			}
		}
	};

	// std::size_t to string
	inline std::string to_string(std::size_t value)
	{
		char num_[128] = { 0 };
		std::memset(num_, 0, sizeof(num_));
		sprintf(num_, "%02d", value);
		return std::string(num_);
	}

	inline bool LogWriteLoopException(WORD wRoomID, WORD wChairID, char * file = __FILE__, int line = __LINE__)
	{
		//写到文件
		std::string strMessage("循环次数异常");
		wstd::log_writer::log_store(strMessage, wRoomID, wChairID, file, line);

		//输出到窗口
		CString str;
		str.Format(TEXT("RoomID_%d_chairID_%d_LINE_%d：循环次数异常"),wRoomID,wChairID,line);
		CTraceService::TraceString(str,TraceLevel_Exception);

		return false;
	}
}

#endif // !__H_LOG_WRITER_H__