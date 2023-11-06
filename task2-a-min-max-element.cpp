#include <stdexcept>
#include <utility>
#include <vector>
/**
 * @brief deterministic algorithm for calculating min/max value in the given vector
 *
 * @param v - vector to find min/max value in
 *
 * Constraints:
 *      1. @p v is not empty
 *      2. first element of the returned pair should be <= than the second element
 *      3. no constraints on the values inside @p v
 *
 * @return std::pair<int, int> pair of min and max elements of @p v
 */
std::pair<int, int> minMaxElement(const std::vector<int> &v)
{
   int min = v[0];
   int max = v[0];
   for(int i = 1; i<v.size();++i){
       if(v[i]>max){
           max = v[i];
       }
       if (v[i] < min) {
           min = v[i];
       }
   }
   std::pair<int, int> myPair(min, max);
   return myPair;
}
// lengths of vectors to benchmark (feel free to change)
const std::vector<long long> Ns{ 100, 600, 1100, 1600, 2100 };

// don't touch
#include "utils/min-max-element.h"
