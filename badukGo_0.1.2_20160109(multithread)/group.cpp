/***************************************************************************************
* Copyright (c) 2014, Antonio Garro.                                                   *
* All rights reserved                                                                  *
*                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are *
* permitted provided that the following conditions are met:                            *
*                                                                                      *
* 1. Redistributions of source code must retain the above copyright notice, this list  *
* of conditions and the following disclaimer.                                          *
*                                                                                      *
* 2. Redistributions in binary form must reproduce the above copyright notice, this    *
* list of conditions and the following disclaimer in the documentation and/or other    *
* materials provided with the distribution.                                            *
*                                                                                      *
* 3. Neither the name of the copyright holder nor the names of its contributors may be *
* used to endorse or promote products derived from this software without specific      *
* prior written permission.                                                            *
*                                                                                      * 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"          *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE            *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE           *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE            *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL    *
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR           *
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER           *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF        *
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
***************************************************************************************/
#include "group.h"
#include <cstring>
#include <assert.h>

Group::Group()
{
  color = 0;
  stones[0] = 0;
  liberties[0] = 0;
  num_stones = 0;
  num_libs = 0;
  memset(libs_ptr,0,sizeof(libs_ptr));
  // if (!check()){std::cerr<<"here 2"<<std::endl;assert(0);}
}

void Group::set_up(int point, bool new_color, const PList &new_liberties)
{
  color = new_color;
  num_stones = 0;
  stones[num_stones++] = point;
  stones[num_stones] = 0;
  num_libs = 0;
  memset(libs_ptr,0,sizeof(libs_ptr));

  for (int i = 0; i < new_liberties.length(); i++) {
    // libs_ptr[new_liberties[i]] = num_libs;//
    liberties[num_libs++] = new_liberties[i];
    libs_ptr[new_liberties[i]] = num_libs;
    liberties[num_libs] = 0;
  }
  // if (!check()){std::cerr<<"here 1"<<std::endl;assert(0);}
  // bool f  =1;
  // for (int i=0; i<num_libs; ++i){
  //   if (i != libs_ptr[liberties[i]]-1){
  //     std::cerr << i << " " <<  libs_ptr[liberties[i]]-1<< std::endl;
  //     f = 0;
      
  //   }
  // }
  // std::cerr << " set" << std::endl;
  // if (!f){

  //   assert(0);
  // }
}

int Group::add_liberties(int i)
{
  if (libs_ptr[i] != 0) return 0; 


  libs_ptr[i] = num_libs+1;
  liberties[num_libs++] = i;
  liberties[num_libs] = 0;
  // bool f = 1;
  // for (int i=0; i<num_libs; ++i){
  //   if (i != libs_ptr[liberties[i]]-1){
  //     std::cerr << i << " " <<  libs_ptr[liberties[i]]-1<< std::endl;
  //     f = 0;
      
  //   }
  // }
  // std::cerr << " add" << std::endl;
  // if (!f){

  //   assert(0);
  // }
  // if (!check()){std::cerr<<"here 4"<<std::endl;assert(0);}
  return num_libs;
}

int Group::erase_liberties(int lib)
{
  //   for (int j = 0; j < num_libs; j++) {
  //   if (liberties[j] == lib) {

  //     liberties[j] = liberties[--num_libs];
  //     liberties[num_libs]  = 0;
  //     return num_libs;
  //   }
  // }
  // return 0;
  // std::cerr << " erase" <<lib<<std::endl;
  // if (!check()){std::cerr<<"here 5"<<std::endl;assert(0);}

  if (libs_ptr[lib] == 0 ) return 0;
  else{
    int pos = libs_ptr[lib]-1;
    // if (liberties[pos] != lib) {
    //   std::cerr << liberties[pos] <<" "<< pos << " "<< lib << " erase"<< num_libs<<std::endl;
    //   for (int j = 0; j < num_libs; j++) {
    //     std::cerr<< liberties[j] << " ";
       // if (liberties[pos] != lib) {std::cerr << lib << std::endl;}
    //   }
    //   std::cerr << std::endl;
    //   assert(0);
    // } 
    // for (int j = 0; j < num_libs; j++) 
    //    if (liberties[j] == lib) {std::cerr << j <<" " << pos<< std::endl;}
    //  if (!check()){std::cerr<<"here 6.1"<<std::endl;assert(0);}
     if (pos<num_libs-1){
      liberties[pos] = liberties[--num_libs];
      libs_ptr[lib] = 0;
      libs_ptr[liberties[num_libs]] = pos+1;
      liberties[num_libs] = 0;
    }else{
      libs_ptr[lib] = 0;
      liberties[pos] = 0;
      --num_libs;
    }
    // if (!check(liberties[pos])){std::cerr<<"here 6"<< " " <<pos << " " << lib << " " << num_libs<<std::endl;assert(0);}
    return num_libs;
  }

}

void Group::clear()
{
  // std::cerr << " clean" <<std::endl;
  num_stones = 0;
  stones[num_stones] = 0;
  num_libs = 0;
  liberties[num_libs] = 0;
  memset(libs_ptr,0,sizeof(libs_ptr));
  // if (!check()){std::cerr<<"here 7"<<std::endl;assert(0);}
}

void Group::attach_group(Group *attached)
{  
  for (int i = 0; i < attached->num_stones; i++) {
    this->stones[num_stones++] = attached->stones[i];
    this->stones[num_stones] = 0;
  }
  for (int i = 0; i < attached->num_libs; i++) {
    this->add_liberties(attached->liberties[i]);
  }
  // if (!check()){std::cerr<<"here 8"<<std::endl;assert(0);}
}
void Group::print_group() const
{
  std::cerr << "Color: " << color;
  for (int i = 0; i < num_stones; i++) std::cerr << " " << stones[i];
  std::cerr << "\n";
}
