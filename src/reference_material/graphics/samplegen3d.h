#if    !defined(KH_SAMPLEGEN3D_H)
#define         KH_SAMPLEGEN3D_H

/*
  samplegen3d.h

  A set of classes to generate random samples.
 */

/*
  Revision history:
  19jan1999 Copied this file from the 2d samplegen.  Renamed all classes in it
            to make sure that they don't conflict with other versions that I
	    have.  
  19jan1999 Added the regular_sample_3d class.
  20jan1999 Added the jitter_sample_3d class.
            Added the poisson_sample_3d class.
	    Changed everything from using a pair of doubles to using my coord2 class.
  08Apr2000 Changed the include of random to be in utils, and coord2 to be in graph_math
  25May2001 Changed this class to be templated, moving the members back in here
            from the samplegen3d.cpp file.  
 */

#include <vector>
#include <utils/random.h>
#include <graph_math/coord2.h>

using std::vector;

template <class T>
class sample_generator_3d
{
public:
  sample_generator_3d(const Random<T>& r = default_random<T>());
  virtual ~sample_generator_3d() { delete rand_gen; }
  
  virtual vector<coord2<T> > get_samples(int num_samples) = 0;
  virtual void get_samples(int num_samples, void(*pf)(const coord2<T>&)) = 0;
  const Random<T>& get_rand_gen() const { return *rand_gen; }  
protected:
  coord2<T> next_rand();
private:
  Random<T>* rand_gen;
};

template <class T>
class random_sample_3d: public sample_generator_3d<T>
{
public:
  random_sample_3d(const Random<T>& r = default_random<T>());
  virtual ~random_sample_3d() {}
  virtual vector<coord2<T> > get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(const coord2<T>&));
};

template <class T>
class regular_sample_3d: public sample_generator_3d<T>
{
public:
  regular_sample_3d(const Random<T>& r = default_random<T>());
  virtual ~regular_sample_3d() {}
  virtual vector<coord2<T> > get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(const coord2<T>&));
};

/* Basically the same as the regular samples, but they are randomized within the block */
template <class T>
class jitter_sample_3d: public sample_generator_3d<T>
{
public:
  jitter_sample_3d(const Random<T>& r = default_random<T>());
  virtual ~jitter_sample_3d() {}
  virtual vector<coord2<T> > get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(const coord2<T>&));
};

/* A class for Poisson Disc sampling */
template <class T>
class poisson_sample_3d: public sample_generator_3d<T>
{
public:
  poisson_sample_3d(const Random<T>& r = default_random<T>(),
		    T distance = 0.1);
  virtual ~poisson_sample_3d() {}
  virtual vector<coord2<T> > get_samples(int num_samples);
  virtual void get_samples(int num_samples, void(*pf)(const coord2<T>&));
private:
  T distance_between_samples;
};



/* sample_generator stuff */
template <class T>
sample_generator_3d<T>::sample_generator_3d(const Random<T>& r):
  rand_gen(r.clone_new())
{
}

template <class T>
coord2<T> sample_generator_3d<T>::next_rand()
{
  return(coord2<T>(rand_gen->next(), rand_gen->next()));
}

/* sample_generator stuff */
template <class T>
random_sample_3d<T>::random_sample_3d(const Random<T>& r):
  sample_generator_3d<T>(r)
{
}

template <class T>
vector<coord2<T> > random_sample_3d<T>::get_samples(int num_samples) 
{
  vector<coord2<T> > v;
  int i;

  for(i = 0; i < num_samples; ++i)
  { v.push_back(next_rand());
  }
  return(v);
}

template <class T>
void random_sample_3d<T>::get_samples(int num_samples, void(*pf)(const coord2<T>&)) 
{
  int i;
  for(i = 0; i < num_samples; ++i)
  { pf(next_rand());
  }
}

/* regular_generator stuff */
template <class T>
regular_sample_3d<T>::regular_sample_3d(const Random<T>& r):
  sample_generator_3d<T>(r)
{
}

template <class T>
vector<coord2<T> > regular_sample_3d<T>::get_samples(int num_samples) 
{
  int x, y;
  int sample_width = (int)sqrt(num_samples);
  vector<coord2<T> > v;

  for(y = 0; y < sample_width; ++y)
  {
    for(x = 0; x < sample_width; ++x)
    {
      v.push_back(coord2<T>(x/T(sample_width-NEAR_ONE), y/T(sample_width-NEAR_ONE)));
    }
  }
  return(v);
}

template <class T>
void regular_sample_3d<T>::get_samples(int num_samples, void(*pf)(const coord2<T>&)) 
{
  int x, y;
  int sample_width = (int)sqrt(num_samples);

  for(y = 0; y < sample_width; ++y)
  {
    for(x = 0; x < sample_width; ++x)
    {
      pf(coord2<T>(x/T(sample_width-NEAR_ONE), y/T(sample_width-NEAR_ONE)));
    }
  }
}

/* jitter_sample stuff */
template <class T>
jitter_sample_3d<T>::jitter_sample_3d(const Random<T>& r):
  sample_generator_3d<T>(r)
{
}

template <class T>
vector<coord2<T> > jitter_sample_3d<T>::get_samples(int num_samples) 
{
  int x, y;
  int sample_width = (int)sqrt(T(num_samples));
  vector<coord2<T> > v;
  
  for(y = 0; y < sample_width; ++y)
  {
    for(x = 0; x < sample_width; ++x)
    {
      coord2<T> p = next_rand();
      v.push_back(coord2<T>((x+p.x())/T(sample_width),
			    (y+p.y())/T(sample_width)));
    }
  }
  return(v);
}

template <class T>
void jitter_sample_3d<T>::get_samples(int num_samples,
				      void(*pf)(const coord2<T>&)) 
{
  int x, y;
  int sample_width = (int)sqrt(T(num_samples));
  for(y = 0; y < sample_width; ++y)
  {
    for(x = 0; x < sample_width; ++x)
    {
      coord2<T> p = next_rand();
      pf(coord2<T>((x+p.x())/T(sample_width),
		   (y+p.y())/T(sample_width)));
    }
  }
}

/* stuff for poisson disc sampling */
template <class T>
poisson_sample_3d<T>::poisson_sample_3d(const Random<T>& r, T distance):
  sample_generator_3d<T>(r), distance_between_samples(distance)
{
}

template <class T>
vector<coord2<T> > poisson_sample_3d<T>::get_samples(int num_samples) 
{
  vector<coord2<T> > samples(num_samples);
  int samples_gathered = 0;
  register int current_sample;
  register int bad_attempts = 0;
  register int restarts = 0;
  coord2<T> next_point;


  while(samples_gathered < num_samples)
  {
    next_point = next_rand();
    samples[samples_gathered] = next_point;
    for(current_sample = 0; current_sample < samples_gathered; ++current_sample)
    {
      if(length(next_point - samples[current_sample]) < distance_between_samples)
      {
	++bad_attempts;
	break; /* Samples too close, can't use it. */
      }
    }
    /* If it made it all the way through the for loop, use the sample. */
    if(current_sample == samples_gathered)
    {
      ++samples_gathered;
      bad_attempts = 0;
    }
    else if(bad_attempts > num_samples)
    {
      samples_gathered = 0;
      ++restarts;
      if(restarts > num_samples/2)
      {
	distance_between_samples *= 0.9; // decrease the distance
	restarts = 0;
	printf("Lowering distance\n");
	fflush(stdout);
      }
    }
  }
  return(samples);  
}

template <class T>
void poisson_sample_3d<T>::get_samples(int num_samples, void(*pf)(const coord2<T>&)) 
{
  std::vector<coord2<T> > v = get_samples(num_samples);
  for(typename std::vector<coord2<T> >::iterator vp = v.begin();
      vp != v.end();
      ++vp)
  {
    pf(*vp);
  }
}

#endif /* !defined(KH_SAMPLEGEN3D_H) */




