/*

Copyright (c) 2009, Takashi Michikawa
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of RCAST, The University of Tokyo nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/
#include "octree.hpp"
#include <iostream>
// compile : g++ octree_main.cpp
int main(int argc, char** argv)
{
        //basic test
        mi::octree<int> tree(1024, 0);
        tree.set(1,3,4, 10);

        if ( tree.get(1,3,4) != 10 ) {
                std::cerr<<"Error at mi::octree<int>::get() "<<tree.get(1,3,4)<<std::endl;
                return EXIT_FAILURE;
        }

        if ( tree.get(1,0,4) != 0 ) {
                std::cerr<<"Error at mi::octree<int>::get() "<<tree.get(1,0,4)<<std::endl;
                return EXIT_FAILURE;
        }
        //test copy constructor
        mi::octree<int> tree2(1024, 0);
        tree2.set(1,3,4, 10);
        if ( tree2.get(1,3,4) != 10 ) {
                std::cerr<<"Error at mi::octree<int>::get() "<<tree2.get(1,3,4)<<std::endl;
                return EXIT_FAILURE;
        }

        if ( tree2.get(1,0,4) != 0 ) {
                std::cerr<<"Error at mi::octree<int>::get() "<<tree2.get(1,0,4)<<std::endl;
                return EXIT_FAILURE;
        }

        //test read()/write();
        std::ofstream fout("out.oct", std::ios::binary);
        tree2.write(fout);
        fout.close();

        mi::octree<int> tree3;
        std::ifstream fin("out.oct", std::ios::binary);
        tree3.read(fin);
        fin.close();
        if ( tree3.get(1,3,4) != 10 ) {
                std::cerr<<"Error at mi::octree<int>::get() "<<tree3.get(1,3,4)<<std::endl;
                return EXIT_FAILURE;
        }
        if ( tree3.get(1,0,4) != 0 ) {
                std::cerr<<"Error at mi::octree<int>::get() "<<tree3.get(1,0,4)<<std::endl;
                return EXIT_FAILURE;
        }

        //test octree::boundingbox();
        tree3.set( 100, 200, 300, 3);
        int mnx, mny, mnz, mxx, mxy, mxz;
        tree3.boundingbox(mnx,mny,mnz,mxx,mxy,mxz, true);
        if (!( mnx == 1 && mny == 3 && mnz == 4 && mxx == 100 && mxy == 200 && mxz == 300)) {
                std::cerr<<"invalid bounding box = ("<<mnx<<", "<<mny<<", "<<mnz<<")-("<<mxx<<", "<<mxy<<", "<<mxz<<")"<<std::endl;
                return EXIT_FAILURE;
        }
        std::cerr<<"OK"<<std::endl;
        return EXIT_SUCCESS;
}
