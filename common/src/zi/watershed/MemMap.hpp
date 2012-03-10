#pragma once

#include <fcntl.h>
#include <sys/mman.h>

void error(const std::string& str){
	mexErrMsgTxt(str.c_str());
}

// based on http://www.linuxquestions.org/questions/programming-9/mmap-tutorial-c-c-511265/

template <typename T>
class MemMappedFileRead{
public:
	MemMappedFileRead(const std::string& fileName,
			  const size_t numBytes)
		: fileName_(fileName)
		, numBytes_(numBytes)
	{
		cout << "num bytes to map: " << numBytes << "\n";

		fd_ = open(fileName_.c_str(), O_RDONLY);
		if(fd_ == -1) {
			error("Error opening "+fileName_+" for read");
		}

		map_ = (T*)mmap(0, numBytes_, PROT_READ, MAP_SHARED, fd_, 0);
		if (map_ == MAP_FAILED) {
			close(fd_);
			error("Error mmapping file "+fileName_);
		}else{
			cout << "mmapped " << numBytes << " bytes\n";
		}
	}

	T* GetMapPtr(){
		return map_;
	}

	~MemMappedFileRead()
	{
		if (munmap(map_, numBytes_) == -1) {
			error("Error un-mmapping the file");
		}
		close(fd_);
	}
private:
	const std::string fileName_;
	const size_t numBytes_;
	int fd_;
	T* map_;
};


template <typename T>
class MemMappedFileWrite{
public:
	MemMappedFileWrite(const std::string& fileName,
			   const size_t numBytes)
		: fileName_(fileName)
		, numBytes_(numBytes)
	{
		fd_ = open(fileName_.c_str(), 
			  O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
		if(fd_ == -1) {
			error("Error opening "+fileName_+" for write");
		}

		//allocate
		int result = lseek(fd_, numBytes_-1, SEEK_SET);
		if (result == -1) {
			close(fd_);
			error("Error calling lseek() to 'stretch' the file");
		}
		result = write(fd_, "", 1);
		if (result != 1) {
			close(fd_);
			error("Error writing last byte of the file");
		}

		map_ = (T*)mmap(0, numBytes_, 
				PROT_READ | PROT_WRITE, 
				MAP_SHARED, fd_, 0);
		if (map_ == MAP_FAILED) {
			close(fd_);
			error("Error mmapping file "+fileName_);
		}else{
			cout << "mmapped " << numBytes << " bytes for writing\n";
		}
	}

	T* GetMapPtr(){
		return map_;
	}

	~MemMappedFileWrite()
	{
		if (munmap(map_, numBytes_) == -1) {
			error("Error un-mmapping the file");
		}
		close(fd_);
	}
private:
	const std::string fileName_;
	const size_t numBytes_;
	int fd_;
	T* map_;
};


