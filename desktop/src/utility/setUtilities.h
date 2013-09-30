/*
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#ifndef OM_UTILITY_H
#define OM_UTILITY_H

template <typename T> class OmSetUtils {
 public:
  static void setIntersection(const std::set<T>& source,
                              const std::set<T>& target,
                              std::set<T>& intersection) {
    //clear result
    intersection.clear();

    //create intersection insert iterator
    std::insert_iterator<std::set<T> > insert_itr(intersection,
                                                  intersection.begin());

    //perform intersection
    std::set_intersection(source.begin(), source.end(), target.begin(),
                          target.end(), insert_itr);
  }

  /*
 	 *	Directional set differance - what is in source but not target.
 	 *	1,2,3 diff 3,4,5 -> 1,2
 	 *	Present in the first set, but not in the second one.
 	 */
  static void setDifference(const std::set<T>& source,
                            const std::set<T>& target,
                            std::set<T>& differance) {
    //clear result
    differance.clear();

    //create intersection insert iterator
    std::insert_iterator<std::set<T> > insert_itr(differance,
                                                  differance.begin());

    //perform difference
    std::set_difference(source.begin(), source.end(), target.begin(),
                        target.end(), insert_itr);
  }

  /*
 	 *	Union.
 	 */
  static void setUnion(const std::set<T>& source1, const std::set<T>& source2,
                       std::set<T>& result) {
    //clear result
    result.clear();

    //create intersection insert iterator
    std::insert_iterator<std::set<T> > insert_itr(result, result.begin());

    //perform difference
    std::set_union(source1.begin(), source1.end(), source2.begin(),
                   source2.end(), insert_itr);
  }
};
#endif
