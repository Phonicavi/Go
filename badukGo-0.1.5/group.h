/**********************************************************************************************
* Copyright (C) 2016 <BadukGo Project>                                                        *
* All rights reserved                                                                         *
*                                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a copy of             *
* this software and associated documentation files (the “Software”), to deal in the Software  *
* without restriction, including without limitation the rights to use, copy, modify, merge,   *
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons  *
* to whom the Software is furnished to do so, subject to the following conditions:            *
*                                                                                             *
* The above copyright notice and this permission notice shall be included in all copies or    *
* substantial portions of the Software.                                                       *
*                                                                                             *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,         *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR    *
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   *
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR        *
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER      *
* DEALINGS IN THE SOFTWARE.                                                                   *
**********************************************************************************************/
#ifndef __GROUP_H__
#define __GROUP_H__

#include "base.h"
#include <algorithm>
#include <ctime>
#include <cstring>
#include <cstdlib>

class LList{
 public:
  virtual void add(int p) = 0;
  virtual void remove(int p) = 0;
  virtual int operator[](int i)const = 0;
  virtual int length() const = 0;

};

class Group{
 private:
  bool color;
  int stones[MAXSIZE2];
  int num_stones;
  int liberties[MAXSIZE2*2/3];
  int num_libs;
  int libs_ptr[MAXSIZE2+1];
 public:
  Group();
  int get_father(){return num_stones?stones[0]:0;}
  void set_up(int point, bool color, const LList &liberties);
  void clear();
  void attach_group(Group *attached);
  
  bool get_color() const { return color; }
  int get_stones_num() const { return num_stones; }
  int get_the_stone(int i) const { return stones[i]; }
  const int *get_the_stones() const { return stones; }
  
  int get_libs_num() const { return num_libs; }
  int get_the_lib(int i) const { return liberties[i]; }
  const int *get_the_libs() const { return liberties; }
  
  class StnItr{
   protected:
    const int *it;
   public:
    StnItr() { it = 0; }
    StnItr(const Group *gr) { it = gr->get_the_stones(); }
    void operator++() { ++it; }
    int operator*() const { return *it; }
    operator bool() const { return *it != 0; }
  };
  
  class LibItr : public StnItr{
   public:
    LibItr(const Group *gr) { it = gr->get_the_libs(); }
  };
  
  bool has_one_liberty() const { return num_libs == 1; }
  bool has_two_liberties() const { return num_libs == 2; }
  int add_liberties(int lib);
  int erase_liberties(int lib);
  void print_group() const;
};

template<const int S> class GroupSet{
 protected:
  Group *groups[S];
  int len;

 public:
  GroupSet()
  {
    len = 0;
    groups[0] = 0;
  }

  bool add(Group *gr)
  {
    if (gr == 0) return false;
    for (int i = 0; i < len; i++) {
      if (groups[i] == gr) {
        return  false;
      }
    }
    groups[len++] = gr;
    return true;
  }

  Group *operator[](int i) const { return groups[i]; }
  Group *get_by_index(int i) const { return groups[i]; }
  int length() const{ return len; }
};

template<int S> class PointList : public LList {    //不用防止重复
protected:
  int Points[S];
  int len;

public:
  PointList()
  {
    len = 0;
    Points[0] = 0;
  }
  void clear()
  {
    len = 0;
    Points[0] = 0;
  }
  virtual void add(int p)
  {
    if (len == S) {
    #ifdef DEBUG_INFO
      std::cerr << "long list\n";
    #endif
      return;
    }
    Points[len++] = p;
    Points[len] = 0;
  }
  void remove(int p)
  {
    for (int j = 0; j < len; j++) {
      if (Points[j] == p) {
        Points[j] = Points[--len];
        Points[len] = 0;
        break;
      }
    }
  }

  int operator[](int i) const { return Points[i]; }
  int length() const { return len; }

  void shuffle() { std::random_shuffle(Points, Points+len); }

};

template<int S> class PointSet : public PointList<S> { //要防止重复
 public:
  void add(int p)
  {
    if (PointList<S>::len == S) {
      return;
    }
    for (int i = 0; i < PointList<S>::len; i++) {
      if (PointList<S>::Points[i] == p) return;
    }
    PointList<S>::Points[PointList<S>::len++] = p;
    PointList<S>::Points[PointList<S>::len] = 0;
  }
};

template<int S> class EmptyPointSet : public LList {
protected:
  int Points[S];
  int len;
  int pointer[MAXSIZE2+1];

public:
  EmptyPointSet()
  {
    len = 0;
    Points[0] = 0;
    memset(pointer,0,sizeof(pointer));
  }
  void clear()
  {
    len = 0;
    Points[0] = 0;
    memset(pointer,0,sizeof(pointer));

  }
  virtual void add(int p)
  {
    if (len == S) {
      return;
    }
    if (pointer[p] !=0) return;
    pointer[p] = len+1;
    Points[len++] = p;
    Points[len] = 0;

  }
  void remove(int p)
  {

    if (pointer[p] == 0) return;
    else{
      int pos = pointer[p]-1;
      if (pos < len-1){
        Points[pos] = Points[--len];
        pointer[p] = 0;
        pointer[Points[len]] = pos+1;
        Points[len] = 0;
      }else{
        pointer[p] = 0;
        Points[pos] = 0;
        --len;
      }
    }

  }

  int operator[](int i) const { return Points[i]; }
  int length() const { return len; }
  void shuffle() { 
    int a;
    for (int i=0; i<len; ++i){
      a = rand()%len;
      std::swap(pointer[Points[i]],pointer[Points[a]]);
      std::swap(Points[i],Points[a]);
    }
  }
};
#endif
