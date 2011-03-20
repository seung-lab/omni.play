#ifndef	OM_SIMPLE_PROGRESS_HPP
#define OM_SIMPLE_PROGRESS_HPP

#include "utility/omThreadPool.hpp"
#include "utility/omTimer.hpp"

class OmSimpleProgress {
private:
	const int max_;
	const std::string title_;

	int cur_;
	size_t lineLen_;

	OmThreadPool threadPool_;
	OmTimer timer_;

public:
	OmSimpleProgress(const int max, const std::string& title)
		: max_(max)
		, title_(title)
		, cur_(0)
		, lineLen_(0)
	{
		threadPool_.start(1);
		std::cout << title << "\n";
	}

	~OmSimpleProgress(){
		threadPool_.join();
	}

	void DidOne(const std::string& prefix = "")
	{
		threadPool_.push_back(
			zi::run_fn(
				zi::bind(
					&OmSimpleProgress::didOne, this, prefix)));
	}

private:
	void didOne(const std::string prefix)
	{
		++cur_;

		coutLine(prefix);

		if(max_ == cur_){
			printDone();
		}
	}

	void printDone(){
		printf("\ndone in %f seconds\n", timer_.s_elapsed());
	}

	void coutLine(const std::string& prefix)
	{
		std::ostringstream stm;
		stm << "\r\t" << prefix << ": "
			<< cur_ << " of " << max_;

		std::string str = stm.str();
		if(str.size() > lineLen_){
			lineLen_ = str.size();
		} else {
			std::string pad(lineLen_ - str.size(), ' ');
			str += pad;
		}

		std::cout << str << "\r" << std::flush;
	}
};

#endif
