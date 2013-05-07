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
/**
* @file octree.hpp
* @brief
* "octree" is a C++ class template to handle octree data structure.
*
* You can easily use it everywhere supporting standard c++ libraries
* (See #include section).
* Detail description is embbed into octree.hpp with doxygen format.
*
* @author Takashi MICHIKAWA (RCAST, The University of Tokyo )
* @date 090912 michi 0.5.0 Created.
* @date 101104 michi 0.5.1 Use int for coordinate
* @note This code is distributed under BSD license.
*/
#ifndef __OCTREE_HPP__
#define __OCTREE_HPP__ 1
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

namespace mi
{
        /**
	 * @class octree
	 * octree implements octree data structure.
	 * @section ex Example code
	 * @code
        * #include <octree.hpp>
        * int main ()
        * {
        *     int value;
        *     mi::octree<int> tree(1024, 0);
        *
        *     tree.set( 100, 200, 300, 4);
        *
        *     value = tree.get(100, 200, 300); // value = 4;
        *     value = tree.get(  0, 300, 400); // value = 0;
        *
        *     return 1;
        * }
        * @endcode
        */
        template < typename T >
        class  octree
        {
        private:

                /**
                * @class node
                * @brief octree node (used in only octree<T>
                */
                template < typename U >
                class node
                {
                private:
                        //definition of nodes
                        typedef struct _nodedata {
                                unsigned char type;
                                unsigned char level;
                        } nodedata;

                        static unsigned char const INVALID = 0x00; ///< Invalid voxel
                        static unsigned char const EMPTY = 0x01; ///< Empty voxel
                        static unsigned char const INTERMEDIATE = 0x02; ///< Intermediate voxel

                private:
                        nodedata 	_data;  ///< Meta data
                        node<U>*	_child; ///< Pointer to child nodes
                        U 		_value; ///< Value

                private:
                        /**
                        * @brief Default Contructor
                        * @note Used in only node class.
                        */
                        node( void ) {
                                this->_child = NULL;
                                this->_data.type = INVALID;
                                this->_data.level = 0x00;
                                this->_value = U();
                                return;
                        }
                public:
                        /**
                        * @brief Constructor
                        * @param[in] lv Current level.
                        * @param[in] value Set value.
                        */
                        node( const unsigned char lv, const U value ) {

                                this->_child	= NULL;
                                this->init(lv, value);
                                this->_data.type = EMPTY;
                                return;
                        }
                        /**
                        * @brief Destructor
                        */
                        virtual ~node( void ) {
                                this->remove_child();
                                return;
                        }

                        /**
                        * @brief get value at (x, y, z)
                        * @param[in] x x-coordinate
                        * @param[in] y y-coordinate
                        * @param[in] z z-coordinate
                        * @return the value at (x, y, z) or empty value if (x, y, z) is invalid.
                        */



                        T get (const int x, const int y, const int z) const {
                                if (this->_data.type == INTERMEDIATE ) {
                                        const int d 	= static_cast<int>(pow(2.0, this->_data.level-1));
                                        return this->_child[(x/d) + 2 * (y/d) + 4 * (z/d)].get(x % d, y % d, z % d);
                                }
                                return this->_value;
                        }
                        /**
                        * @brief set value at (x, y, z)
                        * @param[in] x x-coordinate
                        * @param[in] y y-coordinate
                        * @param[in] z z-coordinate
                        * @param[in] v value
                        * @note do nothing if (x,y,z) is invalid.
                        */

                        void set (const int x, const int y, const int z, const U v) {
                                if ( this->_data.level == 0 ) {
                                        this->_value = v ;
                                } else {
                                        this->create_child();
                                        const size_t d 	= static_cast<size_t>(pow(2.0, this->_data.level-1));
                                        this->_child[(x/d) + 2 * (y/d) + 4 * (z/d)].set(x % d, y % d, z % d, v);
                                }
                                return;
                        }


                        /**
                        * @param[in] fin input file stream
                        * @retval true Succeeded.
                        * @retval false Failed.
                        */
                        bool boundingbox(const U emptyValue,
                                         int& mnx, int& mny, int& mnz,
                                         int& mxx, int& mxy, int& mxz) {
                                const int d = static_cast< int >( pow(2.0, this->_data.level ) );
                                mnx = mny = mnz = d - 1;
                                mxx = mxy = mxz = 0;

                                if ( this->_data.type == EMPTY ) {
                                        if ( this->_value != emptyValue) {
                                                mnx = mny = mnz = 0;
                                                mxx = mxy = mxz = d - 1;
                                                return true;
                                        } else {
                                                return false;
                                        }
                                } else if ( this->_data.type == INTERMEDIATE ) {
                                        for (int z = 0 ; z < 2 ; z++) {
                                                for (int y = 0 ; y < 2 ; y++) {
                                                        for (int x = 0 ; x < 2 ; x++) {
                                                                const int id = x + 2 * y + 4 * z;

                                                                int lnx, lny, lnz, lxx, lxy, lxz;
                                                                if ( !this->_child[id].boundingbox(emptyValue, lnx, lny, lnz, lxx, lxy, lxz) ) continue;
                                                                const int offx = d/2 * x;
                                                                const int offy = d/2 * y;
                                                                const int offz = d/2 * z;

                                                                lnx += offx;
                                                                lny += offy;
                                                                lnz += offz;

                                                                lxx += offx;
                                                                lxy += offy;
                                                                lxz += offz;

                                                                if ( lnx < mnx ) mnx = lnx;
                                                                if ( lny < mny ) mny = lny;
                                                                if ( lnz < mnz ) mnz = lnz;
                                                                if ( lxx > mxx ) mxx = lxx;
                                                                if ( lxy > mxy ) mxy = lxy;
                                                                if ( lxz > mxz ) mxz = lxz;
                                                        }
                                                }
                                        }
                                        return mnx <= mxx && mny <= mxy && mnz <= mxz;
                                } else {
                                        return false;
                                }
                        }

                        bool optimize( void ) {
                                if ( this->_data.type == INTERMEDIATE ) {
                                        for ( int i = 0 ; i < 8 ; i++ ) {
                                                if ( !this->_child[i].optimize() ) return false;
                                        }
                                        this->_value = this->_child[0]._value;
                                        this->remove_child();
                                }
                                return true;
                        }
                        bool read ( std::ifstream& fin ) {
                                unsigned char type;
                                fin.read( (char*)&type, sizeof(unsigned char) );
                                if ( fin.fail() ) return false;
                                switch ( type ) {
                                case INTERMEDIATE:
                                        this->create_child();
                                        for ( int i = 0 ; i < 8 ; i++) {
                                                if ( !this->_child[i].read(fin) ) return false;
                                        }
                                        return true;

                                case EMPTY:
                                        fin.read( (char*)&_value, sizeof(U) );
                                        return !fin.bad();
                                }
                                _data.type = INVALID;
                                return false;
                        }

                        /**
                        * @param[in] fout output file stream
                        * @retval true Succeeded.
                        * @retval false Failed.
                        */
                        bool write ( std::ofstream& fout ) {

                                fout.write((char*)&(_data.type), 1);
                                switch (_data.type) {
                                case INTERMEDIATE:
                                        for ( int i = 0 ; i < 8 ; i++) {
                                                if ( !this->_child[i].write(fout)) return false;
                                        }
                                        return true;

                                case EMPTY:
                                        fout.write( (char*) &(_value), sizeof(U) );
                                        return true;
                                }
                                return false;
                        }
                        /**
                        * @brief Copy object.
                        * @param[in] d Copying instances.
                        */
                        void copy ( const node<T>& d ) {
                                this->_data.level = d._data.level;
                                this->_value = d._value;
                                if ( d._data.type == INTERMEDIATE ) {
                                        this->create_child();
                                        for (int i = 0 ; i < 8 ; i++) {
                                                this->_child[i].copy(d._child[i]);
                                        }
                                }
                                this->_data.type = d._data.type;
                                return;
                        }
                        /**
                        * @brief Initializing child nodes.
                        * @param[in] lv Level of the node.
                        * @param[in] value Value.
                        */
                        void init ( const unsigned char lv, const U value ) {
                                this->_data.level 	= lv;
                                this->_value 		= value;
                                this->_data.type 	= EMPTY;
                                return;
                        };

                        /**
                        * @brief allocating child nodes.
                        */
                        void create_child( void ) {
                                if (this->_data.type == EMPTY ) {
                                        this->_data.type = INTERMEDIATE;
                                        this->_child = new node<U>[8];
                                        for ( int i = 0 ; i < 8 ; ++i) {
                                                this->_child[i].init(this->_data.level - 0x01, this->_value);
                                        }
                                }
                                return;
                        }
                        /**
                        * @brief deallocating child nodes.
                        */
                        void remove_child( void ) {
                                if ( this->_data.type == INTERMEDIATE ) {
                                        this->_data.type = EMPTY;
                                        delete[] this->_child;
                                }
                                return;
                        }
                };

        private:
                unsigned char   _level; ///< Maximum level of the octree.
                int		_dimension; ///< Size of the octree.
                T		_emptyValue; ///< Empty value of the octree.
                node<T>*	_root; ///< A pointer to root pointer.
	private:
		octree ( const octree<T>& that);
		void operator = ( const octree<T>& that);
        public:
                /**
                * @brief Default constructor.
                *
                */
                octree ( void ) {
                        this->_root = NULL;
                }
                /**
                * @param[in] dimension dimension of the octree
                * @param[in] emptyValue the default value of the octree
                */
                octree ( const int dimension, const T emptyValue = T()) {
                        this->_root = NULL;
                        this->init(dimension, emptyValue);
                        return;
                }

                /**
                * @brief Destructor
                */
                virtual ~octree ( void ) {
                        if ( this->_root != NULL ) delete _root;
                        return;
                }
                /**
                * @param[in] dimension dimension of the octree
                * @param[in] emptyValue the default value of the octree
                */
                void init(const int dimension, const T emptyValue) {
                        if (this->_root != NULL) delete this->_root;
                        this->_level      = this->get_level(dimension);
                        this->_dimension  = static_cast<int>(pow(2.0, this->_level));
                        this->_emptyValue = emptyValue;
                        this->_root = new node<T>( this->_level, this->_emptyValue);
                        return;
                }

                /**
                * @brief get value at (x, y, z)
                * @param[in] x x-coordinate
                * @param[in] y y-coordinate
                * @param[in] z z-coordinate
                * @return the value at (x, y, z) or empty value if (x, y, z) is invalid.
                */
                T get (const int x, const int y, const int z) const {
                        return this->is_valid(x,y,z) ?
                               this->_root->get( x, y, z ) : this->_emptyValue;
                }

                /**
                * @brief set value at (x, y, z)
                * @param[in] x x-coordinate
                * @param[in] y y-coordinate
                * @param[in] z z-coordinate
                * @param[in] v value
                * @note do nothing if (x,y,z) is invalid.
                */
                void set (const int x, const int y, const int z, const T v) {
                        if ( this->is_valid(x,y,z) ) {
                                this->_root->set( x, y, z, v );
                        }
                        return;
                }

                /**
                * @brief check (x, y, z) is valid
                * @param[in] x x-coordinate
                * @param[in] y y-coordinate
                * @param[in] z z-coordinate
                * @retval true (x, y, z) is valid
                * @retval false (x, y, z) is invalid
                */
                bool is_valid (const int x, const int y, const int z) const {
                        if ( x < 0 ) return false;
                        if ( y < 0 ) return false;
                        if ( z < 0 ) return false;
                        if ( this->getDimension() <= x ) return false;
                        if ( this->getDimension() <= y ) return false;
                        if ( this->getDimension() <= z ) return false;
                        return true;
                }

                /**
                * @brief check (x, y, z) is empty
                * @param[in] x x-coordinate
                * @param[in] y y-coordinate
                * @param[in] z z-coordinate
                * @retval true (x, y, z) is empty
                * @retval false (x, y, z) is not empty
                */
                bool is_empty ( const int x, const int y, const int z) {
                        return this->get(x, y, z) == this->_emptyValue;
                }

                /**
                * @brief get bounding box (mnx, mny, mnz) - (mxx, mxy, mxz)
                * @param[out] mnx minimum x-coordinate
                * @param[out] mny minimum y-coordinate
                * @param[out] mnz minimum z-coordinate
                * @param[out] mxx maximum x-coordinate
                * @param[out] mxy maximum y-coordinate
                * @param[out] mxz maximum z-coordinate
                * @param[in] optimized if true, get bounding box of non-empty cells
                */
                void boundingbox (int& mnx, int& mny, int& mnz,  int& mxx, int& mxy, int& mxz,              bool optimized = true) {
                        if (optimized) {
                                this->_root->boundingbox( this->_emptyValue, mnx, mny, mnz, mxx, mxy, mxz);
                        } else {
                                mnx = mny = mnz = 0;
                                mxx = mxy = mxz = this->_dimension - 1;
                        }
                        return;
                }

                /**
                * @param[in] value The value which you count.
                * @return the nuber of voxels with the value.
                */
                size_t count( const T value ) const {
                        return this->_root->count(value);
                }

                /**
                * @param[in] opt optimized or not. if false, do nothing.
                */
                void optimize ( const bool opt = true ) {
                        if ( opt ) _root->optimize();
                        return;
                }

                /**
                * @return a dimension of the octree. It must be a 2^n.
                */
                int getDimension ( void ) const {
                        return this->_dimension;
                }

                /**
                * @return empty value of the octree.
                */
                T getEmptyValue( void ) const {
                        return this->_emptyValue;
                }

                /**
                * @param[in] fin input file stream
                * @retval true Succeeded.
                * @retval false Failed.
                */
                bool read ( std::ifstream& fin ) {
                        int dimension;
                        T emptyValue;
                        fin.read ( (char*)&dimension , sizeof(int) );
                        fin.read ( (char*)&emptyValue, sizeof(T) );
                        this->init(dimension, emptyValue);
                        if (fin.fail()) return false;
                        return this->_root->read(fin);
                }
                /**
                * @param[in] fout output file stream
                * @retval true Succeeded.
                * @retval false Failed.
                */
                bool write ( std::ofstream& fout ) {
                        if ( fout.fail() ) return false;
                        fout.write ( (char*)&_dimension , sizeof(int) );
                        fout.write ( (char*)&_emptyValue, sizeof(T) );
                        if ( fout.fail() ) return false;
                        return this->_root->write(fout);
                }
        private:
                /**
                * @brief get the maximum level of the octree
                * @param[in] dimension dimension of the octree
                * @return maximum level of the octree or 0 if the level is larger than 255.
                */
                unsigned char get_level( const int dimension) const {
                        int x = 1;
                        for (unsigned char i = 0 ; i < 0xFF ; i++) {
                                if ( dimension <= x) return i;
                                x *= 2;
                        }
                        return 0;
                }
        };
};
#endif// __OCTREE_HPP__
