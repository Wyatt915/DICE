#include "gurpslogic.hpp"

namespace gurps{
     disp::disp(int s, int d, unt i, int h){
        st = s; dx = d; iq = i; ht = h;
        dmg = st;
        bl = st * st / 5;
        will = iq;
     }
}
