//#define AMETHYST_EXCEPTION_SAFETY
#include "quick_vector.hpp"
#include<iostream>
#include<ostream>
#include<vector>
#include<ctime>
#include<cstdlib>
#include<cassert>


/*
  A naive performance comparison of std::vector and amethyst::quick_vector .
  This program individually tests the performance of a few basic operations.

  There are several perf issues this program does not address:

  (a) The possiblility that other running programs may interfere with test
      results.
      
  (b) The fact that real applications may have usage patterns whose 
      performance characteristics differ wildly from what might be guessed 
      based on individual tests of basic operations.

  I don't have any good ideas for addressing (a) or (b).
      For now I will just ignore them.


  I may adapt this to use getrusage or some such (this would partially
      address (a)), mostly to examine perf characteristics other than
      run time.
 */

#include <sys/times.h>

clock_t process_clock()
{
  struct tms process_times;

  times(&process_times);

  return process_times.tms_utime + process_times.tms_stime;
}

using std::vector;
using std::cout;
using std::endl;
using std::clock;

using amethyst::quick_vector;

template<class container, template <class tested_container> class test_t>
  clock_t time_test(int iterations, int size, test_t<container> test)
  {
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      test(size);
    }
    return process_clock() - start_time;    
  }

template<class container>
clock_t time_test_creation(int iterations, int size)
  {
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      container v(size);
      v[0]= i;
    }
    return process_clock() - start_time;
  }

template<class container>
clock_t time_test_copy(int iterations, int size)
  {
    container v(size);
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      container v_c(v);
      v_c[0]= i;
    }
    return process_clock() - start_time;
  }

template<class container>
clock_t time_test_assign(int iterations, int size)
  {
    container v(size);
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      container v_c;
      v_c= v;
    }
    return process_clock() - start_time;
  }

template<class container>
clock_t time_test_push_back(int iterations, int size)
  {
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      container v(1);
      for(int j= 0 ; j < size ; ++j)
      {
        v.push_back(typename container::value_type(j));
      }
    }
    return process_clock() - start_time;
  }

template<class container>
  clock_t time_test_iterator_traversal(int iterations, int size)
  {
    container v(size);
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      for(typename container::iterator i= v.begin() ; i != v.end() ; ++i)
      {
        typename container::value_type a(*i);
        *i= a;
      }
    }
    return process_clock() - start_time;
  }

template<class container>
  clock_t time_test_index_traversal(int iterations, int size)
  {
    container v(size);
    clock_t start_time= process_clock();
    for(int i= 0 ; i < iterations ; ++i)
    {
      for(size_t i= 0 ; i < v.size() ; ++i)
      {
        typename container::value_type a(v[i]);
        v[i]= a;
      }
    }
    return process_clock() - start_time;
  }

int random(int range)
  {
    if(range < RAND_MAX)
    {
      int const ranges(RAND_MAX / range);
      int const range_max(ranges*range);

      int result;

      do
      {
        result= rand();
      }while(result >= range_max);

      return result%range;
    }
    else
    {
      long long const ranges((long long)RAND_MAX*RAND_MAX / range);
      long long const range_max(ranges*range);

      long long result;

      do
      {
        result= rand()*(long long)RAND_MAX + rand();
      }while(result >= range_max);

      return result%range;
    }
  }

template<class container>
  clock_t time_test_random_access(int iterations, int size)
  {
    container v(size);
    clock_t start_time= process_clock();
    srand(0);
    for(int i= 0 ; i < iterations ; ++i)
    {
      for(size_t i= 0 ; i < v.size() ; ++i)
      {
        int r= random(size);
        typename container::value_type a(v[r]);
        v[r]= a;
      }
    }
    return process_clock() - start_time;
  }


template <class T>
struct foo
{
  static long long construct_count;
  static long long destruct_count;
  static long long assign_count;
  T val;

  inline foo() { ++construct_count; }
  inline foo(const T& v): val(v) { ++construct_count; }
  inline foo(const foo<T>& f): val(f.val) { ++construct_count; }
  inline ~foo() { ++destruct_count; }
  inline foo<T>& operator=(const foo<T>& f) { ++assign_count; val = f.val; return *this; }
};

template <class T> long long foo<T>::construct_count = 0;
template <class T> long long foo<T>::destruct_count = 0;
template <class T> long long foo<T>::assign_count = 0;

#define PRE_SETUP() \
prev_constructs = foo<int>::construct_count; \
prev_destructs = foo<int>::destruct_count; \
prev_assigns = foo<int>::assign_count;

#define POST_ADD(HEADER) \
constructs_##HEADER += foo<int>::construct_count - prev_constructs; \
destructs_##HEADER += foo<int>::destruct_count - prev_destructs; \
assigns_##HEADER += foo<int>::assign_count - prev_assigns;

int main(int argc, char** argv)
  {
    long long constructs_qv = 0;
    long long destructs_qv = 0;
    long long assigns_qv = 0;

    long long constructs_stlv = 0;
    long long destructs_stlv = 0;
    long long assigns_stlv = 0;

    long long prev_constructs, prev_destructs, prev_assigns;
    
    int const iterations= (argc > 1) ? atoi(argv[1]) : 100000;
    int const size= (argc > 2) ? atoi(argv[2]) : 100;

    PRE_SETUP();
    clock_t vector_creation_time=
      time_test_creation< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_creation< vector<struct foo<int> > > (" << iterations << ", "
      << size << ") :       " << vector_creation_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();
    clock_t quick_vector_creation_time=
      time_test_creation< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_creation< quick_vector<struct foo<int> > > (" << iterations << ", "
      << size << ") : " << quick_vector_creation_time
      << "\n";
    POST_ADD(qv);
    PRE_SETUP();
    clock_t vector_copy_time=
      time_test_copy< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_copy< vector<struct foo<int> > > (" << iterations << ", "
      << size << ") :       " << vector_copy_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();    
    clock_t quick_vector_copy_time=
      time_test_copy< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_copy< quick_vector<struct foo<int> > > ("
      << iterations << ", " << size << ") : " << quick_vector_copy_time
      << "\n";
    POST_ADD(qv);
    PRE_SETUP();
    
    clock_t vector_assign_time=
      time_test_assign< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_assign< vector<struct foo<int> > > (" << iterations << ", "
      << size << ") :       " << vector_assign_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();    
    clock_t quick_vector_assign_time=
      time_test_assign< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_assign< quick_vector<struct foo<int> > > ("
      << iterations << ", " << size << ") : " << quick_vector_assign_time
      << "\n";
    POST_ADD(qv);
    PRE_SETUP();    

    clock_t vector_push_back_time=
      time_test_push_back< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_push_back< vector<struct foo<int> > > (" << iterations << ", "
      << size << ") :       " << vector_push_back_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();    
    clock_t quick_vector_push_back_time=
      time_test_push_back< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_push_back< quick_vector<struct foo<int> > > (" << iterations
      << ", " << size << ") : " << quick_vector_push_back_time << "\n";
    POST_ADD(qv);
    PRE_SETUP();    

    clock_t vector_iterator_traversal_time=
      time_test_iterator_traversal< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_iterator_traversal< vector<struct foo<int> > > (" << iterations
      << ", " << size << ") :       " << vector_iterator_traversal_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();    
    clock_t quick_vector_iterator_traversal_time=
      time_test_iterator_traversal< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_iterator_traversal< quick_vector<struct foo<int> > > ("
      << iterations << ", " << size << ") : "
      << quick_vector_iterator_traversal_time << "\n";
    POST_ADD(qv);
    PRE_SETUP();    
    
    clock_t vector_index_traversal_time=
      time_test_index_traversal< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_index_traversal< vector<struct foo<int> > > (" << iterations
      << ", " << size << ") :       " << vector_index_traversal_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();    
    clock_t quick_vector_index_traversal_time=
      time_test_index_traversal< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_index_traversal< quick_vector<struct foo<int> > > ("
      << iterations << ", " << size << ") : "
      << quick_vector_index_traversal_time << "\n";
    POST_ADD(qv);
    PRE_SETUP();    

    clock_t vector_random_access_time=
      time_test_random_access< vector<struct foo<int> > > (iterations, size);
    cout << "time_test_random_access< vector<struct foo<int> > > (" << iterations
      << ", " << size << ") :       " << vector_random_access_time << "\n";
    POST_ADD(stlv);
    PRE_SETUP();    
    clock_t quick_vector_random_access_time=
      time_test_random_access< quick_vector<struct foo<int> > >(iterations, size);
    cout << "time_test_random_access< quick_vector<struct foo<int> > > ("
      << iterations << ", " << size << ") : "
      << quick_vector_random_access_time << "\n";
    POST_ADD(qv);
    PRE_SETUP();

    cout << endl;

    cout << "Construct count (total)= " << foo<int>::construct_count << endl;
    cout << "Destruct count (total)=  " << foo<int>::destruct_count << endl;
    cout << "Assign count (total)=    " << foo<int>::assign_count << endl;
    cout << "-------------------------" << endl;
    cout << "Construct count (stlv)=  " << constructs_stlv << endl;
    cout << "Construct count (qv)=    " << constructs_qv << endl;
    cout << "Destruct count (stlv)=   " << destructs_stlv << endl;
    cout << "Destruct count (qv)=     " << destructs_qv << endl;
    cout << "Assign count (stlv)=     " << assigns_stlv << endl;
    cout << "Assign count (qv)=       " << assigns_qv << endl;
  }
