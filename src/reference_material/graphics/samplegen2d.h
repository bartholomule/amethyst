#if    !defined(KH_SAMPLEGEN2D_H)
#define         KH_SAMPLEGEN2D_H

/*
  samplegen2d.h

  A set of classes to generate random samples.
 */

/*
  Revision history:
  17jan1999 Created this file.
  19jan1999 Added virtual destructors to the sample_generator and random_sample
            classes.
  19jan1999 Renamed and moved this file (old name was samplegen.h).  Renamed
            all classes in it to make sure that they don't conflict with other
	    versions that I will need later. 
  19jan1999 Added the regular_sample_2d class.
  20jan1999 Added the jitter_sample_2d class.
            Added the poisson_sample_2d class.
  25jan1999 Changed the include of random to look in the parent directory.
  08Apr2000 Changed the include of random to look in <utils/xxx>
  25May2001 Changed this class to be templated, moving the members back in here
            from the samplegen2d.cpp file.
 */

#include <vector>
#include <utils/random.h>

using std::vector;

template <class T>
class sample_generator_2d
{
public:
  sample_generator_2d(const Random<T>& r = default_random<T>());
  virtual ~sample_generator_2d() { delete rand_gen; }
  
  virtual vector<T> get_samples(int num_samples) = 0;
  virtual void get_samples(int num_samples, void(*pf)(T)) = 0;
  const Random<T>& get_rand_gen() const { return *rand_gen; }
protected:
  T next_rand();
private:
  Random<T>* rand_gen;
};

template <class T>
class random_sample_2d: public sample_generator_2d<T>
{
public:
  random_sample_2d(const Random<T>& r = default_random<T>());
  virtual ~random_sample_2d() {}
  virtual vector<T> get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(T));
};

template <class T>
class regular_sample_2d: public sample_generator_2d<T>
{
public:
  regular_sample_2d(const Random<T>& r = default_random<T>());
  virtual ~regular_sample_2d() {}
  virtual vector<T> get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(T));
};

template <class T>
class jitter_sample_2d: public sample_generator_2d<T>
{
public:
  jitter_sample_2d(const Random<T>& r = default_random<T>());
  virtual ~jitter_sample_2d() {}
  virtual vector<T> get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(T));
};


/* A class for Poisson sampling (the equiv of the disc sampling with fewer dimensions) */
template <class T>
class poisson_sample_2d: public sample_generator_2d<T>
{
public:
  poisson_sample_2d(const Random<T>& r = default_random<T>(),
		    T distance = 0.1);
  virtual ~poisson_sample_2d() {}
  virtual vector<T> get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(T));
private:
  T distance_between_samples;
};


/* sample_generator stuff */
template <class T>
sample_generator_2d<T>::sample_generator_2d(const Random<T>& r):
  rand_gen(r.clone_new())
{
}

template <class T>
T sample_generator_2d<T>::next_rand()
{
  return(rand_gen->next());
}

/* sample_generator stuff */
template <class T>
random_sample_2d<T>::random_sample_2d(const Random<T>& r):
  sample_generator_2d<T>(r)
{
}

template <class T>
vector<T> random_sample_2d<T>::get_samples(int num_samples)
{
  vector<T> v;
  int i;

  for(i = 0; i < num_samples; ++i)
  { v.push_back(next_rand());
  }
  return(v);
}

template <class T>
void random_sample_2d<T>::get_samples(int num_samples, void(*pf)(T))
{
  int i;
  for(i = 0; i < num_samples; ++i)
  { pf(next_rand());
  }
}

/* regular_generator stuff */
template <class T>
regular_sample_2d<T>::regular_sample_2d(const Random<T>& r):
  sample_generator_2d<T>(r)
{
}

template <class T>
vector<T> regular_sample_2d<T>::get_samples(int num_samples)
{
  int x;
  vector<T> v;
  for(x = 0; x < num_samples; ++x)
  {
    v.push_back(x/T(num_samples-NEAR_ONE));
  }
  return(v);
}

template <class T>
void regular_sample_2d<T>::get_samples(int num_samples, void(*pf)(T))
{
  int x;
  for(x = 0; x < num_samples; ++x)
  {
    pf(x/T(num_samples-NEAR_ONE));
  }
}

/* jitter sample stuff */
template <class T>
jitter_sample_2d<T>::jitter_sample_2d(const Random<T>& r):
  sample_generator_2d<T>(r)
{
}

template <class T>
vector<T> jitter_sample_2d<T>::get_samples(int num_samples)
{
  int x;
  vector<T> v;
  for(x = 0; x < num_samples; ++x)
  {
    v.push_back((x+next_rand())/T(num_samples));
  }
  return(v);
}

template <class T>
void jitter_sample_2d<T>::get_samples(int num_samples, void(*pf)(T))
{
  int x;
  for(x = 0; x < num_samples; ++x)
  {
    pf((x+next_rand())/T(num_samples));
  }
}

/* stuff for the poisson class */
template <class T>
poisson_sample_2d<T>::poisson_sample_2d(const Random<T>& r,
					T distance):
  sample_generator_2d<T>(r), distance_between_samples(distance)
{
}

template <class T>
vector<T> poisson_sample_2d<T>::get_samples(int num_samples)
{
  vector<T> samples(num_samples);
  int samples_gathered = 0;
  int current_sample;
  T next_point;

  while(samples_gathered < num_samples)
  {
    next_point = next_rand();
    samples[samples_gathered] = next_point;
    for(current_sample = 0; current_sample < samples_gathered; ++current_sample)
    {
      if(fabs(next_point - samples[current_sample]) < distance_between_samples)
	break; /* Samples too close, can't use it. */
    }
    /* If it made it all the way through the for loop, use the sample. */
    if(current_sample == samples_gathered)
      ++samples_gathered;
  }
  return(samples);  
}

template <class T>
void poisson_sample_2d<T>::get_samples(int num_samples, void(*pf)(T))
{
  std::vector<T> v = get_samples(num_samples);
  for(typename std::vector<T>::iterator vp = v.begin();
      vp != v.end();
      ++vp)
  {
    pf(*vp);
  }
}


#endif /* !defined(KH_SAMPLEGEN2D_H) */




