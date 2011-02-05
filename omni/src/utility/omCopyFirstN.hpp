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

template <class C, class T>
static std::string MakeShortStrList(const C& src, const uint32_t max)
{
    std::vector<T> ret;

    om::utils::CopyFirstN(src, ret, max);

    const std::string nums = om::string::join(ret);

    std::string post = "";
    if(src.size() > max){
        post = "...";
    }

    return nums + post;
}

}
}

#endif
