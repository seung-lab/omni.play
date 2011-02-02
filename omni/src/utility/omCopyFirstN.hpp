#ifndef OM_COPY_FIST_N_HPP
#define OM_COPY_FIST_N_HPP

namespace om {
	namespace utils {

		template <class X, class Y>
		static void CopyFirstN(const X& src, Y& dst, const int max)
		{
			int counter = 0;
			FOR_EACH(iter, src){
				if(max == ++counter){
					break;
				}
				dst.push_back(*iter);
			}
		}
	}
};

#endif
