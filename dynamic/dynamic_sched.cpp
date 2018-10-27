#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif
pthread_mutex_t mutex_output, mutex_getNext;
float output = 0;
int beginNext=0, endNext, endPrev=0, granularity, n;
struct data {
	int functionid;
	float a;
	float b;
  int intensity;
  string sync;
  int granularity;
};

bool done = false;
int getNextBegin(){
	int beginValue;
	pthread_mutex_lock(&mutex_getNext);
	beginValue = beginNext;
	beginNext = beginNext+granularity;
	if( beginNext >= n)
		done = true;	
	pthread_mutex_unlock(&mutex_getNext);
	return beginValue;
}

int getNextEnd(int begin){
	endNext = begin+granularity;	
	return endNext;
}

void* calculateNumericIntegration(void* p) {
	struct data *numericData = (struct data*)p;
	int functionid = numericData->functionid;
	float a = numericData->a;
	float b = numericData->b;
	int intensity = numericData->intensity;
	int granularity = numericData->granularity;	
	string sync = numericData->sync;
	float y = (b-a)/n;
	
	float threadVal=0;
	while(!done){
		float val1=0;
		float input;
		int begin= getNextBegin();		
		int end = getNextEnd(begin);
		if(sync.compare("thread")==0){
			for(int i=begin; i<end && i<n; ++i) {
				switch(functionid) {
					case 1 : 
						input = a + ((i+0.5)*y);
						threadVal += f1(input,intensity);
						break;
					case 2 : 
						input = a + ((i+0.5)*y);
						threadVal += f2(input,intensity);
						break;
					case 3 : 
						input = a + ((i+0.5)*y);
						threadVal += f3(input,intensity);
						break;
					case 4 : 
						input = a + ((i+0.5)*y);
						threadVal += f4(input,intensity);
						break;
					default : 
						break;
				}
			}
			
		}
		else if(sync.compare("iteration")==0){
			for(int i=begin; i<end && i<n; ++i) {
				switch(functionid) {
					case 1 :
						input = a + ((i+0.5)*y);
						val1 = f1(input,intensity);
						pthread_mutex_lock(&mutex_output);
						output += val1;
						pthread_mutex_unlock(&mutex_output);
						break;
					case 2 : 
						input = a + ((i+0.5)*y);
						val1 = f2(input,intensity);
						pthread_mutex_lock(&mutex_output);
						output += val1;
						pthread_mutex_unlock(&mutex_output);
						break;
					case 3 : 
						input = a + ((i+0.5)*y);
						val1 = f3(input,intensity);
						pthread_mutex_lock(&mutex_output);
						output += val1;
						pthread_mutex_unlock(&mutex_output);
						break;
					case 4 : 
						input = a + ((i+0.5)*y);
						val1 = f4(input,intensity);
						pthread_mutex_lock(&mutex_output);
						output += val1;
						pthread_mutex_unlock(&mutex_output);
						break;
					default : 
						break;
				}
			}
		}
		else if(sync.compare("chunk")==0){
			float chunk=0;
			for(int i=begin; i<end && i<n; ++i) {
			
				switch(functionid) {
					case 1 : 
						input = a + ((i+0.5)*y);
						chunk += f1(input,intensity);
						break;
					case 2 : 
						input = a + ((i+0.5)*y);
						chunk += f2(input,intensity);
						break;
					case 3 : 
						input = a + ((i+0.5)*y);
						chunk += f3(input,intensity);
						break;
					case 4 : 
						input = a + ((i+0.5)*y);
						chunk += f4(input,intensity);
						break;
					default : 
						break;
				}
			}
			pthread_mutex_lock(&mutex_output);
			output+=chunk;
			pthread_mutex_unlock(&mutex_output);
		}
	}
	if(sync.compare("thread")==0){
		pthread_mutex_lock(&mutex_output);
		output+=threadVal;
		pthread_mutex_unlock(&mutex_output);
	}		
	return NULL ;
}
	
int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }
	int functionid, intensity, nbthreads;
  float a,b;
  functionid = atoi(argv[1]);				// an integer to know which function to integrate
  a = atof(argv[2]);								// the lower bound of the integral
  b = atof(argv[3]);								// the upper bound of the integral
  n = atoi(argv[4]);								// an integer which is the no. of points to compute the approximation of the integral
  intensity = atoi(argv[5]);				// an integer which is the second parameter to give the the function to integrate
  nbthreads = atoi(argv[6]);				// number of threads
  std::string sync= argv[7];				// a string either iteration or thread or chunk that indicate which sync method to use
  granularity = atoi(argv[8]);			// an integer indicating how many iterations does one thread take at a time
  
  float b_a_by_n = (b-a)/n;					// Calculate value of (b-a)/n and store it in b_a_by_n
  pthread_t threads[nbthreads];
  struct data args[nbthreads];
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  for (int i=0; i<nbthreads; ++i) {
	  args[i].functionid=functionid;
	  args[i].a=a;
	  args[i].b=b;
	  args[i].intensity=intensity;
	  args[i].sync=sync;
	  args[i].granularity=granularity;
	  pthread_create(&threads[i], NULL, calculateNumericIntegration,  &args[i]);
	}
  for (int j=0; j<nbthreads; ++j) {
	  pthread_join(threads[j], NULL);
  }
  float result = b_a_by_n*output;				// Calculate value of F(x)*(b-a)/n where F(x) is the integration of f(x)
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout<<result<<endl; 
  std::cerr<<elapsed_seconds.count()<<std::endl;
  return 0;
}
