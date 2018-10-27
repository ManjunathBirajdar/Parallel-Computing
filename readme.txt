STATIC LOOP SCHEDULER:
A static loop scheduler is the simplest way to achieve some parallelism. We will use it to make the numerical
integration problem parallel.

Note that all the threads will need access to a pointer to the float that stores the sum of all the values.
To avoid race conditions, this variables needs to be placed under mutual exclusion and there are two ways
to do this:
• iteration-level mutual exclusion essentially puts the mutual exclusion on each access to the variable
• thread-level mutual exclusion makes each threads locally compute the sum of values in its own variable,
and only aggregates the value into the results once all the iterations have been computed.

Question: Write a static loop scheduler and use it to compute numerical integration. Navigate to the
static/ directory and use static sched.cpp as a template. Write the code so that it outputs the integral
value on stdout and the time it takes to make the computation on stderr.
The program should take the following command line parameters:
• functionid, same as in numerical integration assignment
• a, same as in numerical integration assignment
• b, same as in numerical integration assignment
• n, same as in numerical integration assignment
• intensity, same as in numerical integration assignment
• nbthreads, an integer indicating the number of threads that should perform the numerical integration
• sync, a string, either iteration or thread that indicate which synchronization method to use.

Question: Report time and speedup across a range of precision, intensity, and synchronization mode. Use
make test to test your code. Your code MUST pass the test before you can use make bench to start the
PBS jobs. Once they are complete, plot the charts with make plot.

DYNAMIC LOOP SCHEDULER:
A dynamic loop schedulers are essentially managed by distributing ranges of indices to threads when they
request them.
The worker threads execute a code that looks like
while (!loop.done()){
int begin, end;
loop.getnext(&begin, &end);
executeinnerloop(begin, end);
}
The implementation of a loop scheduler boils done to implementing the two functions done and getnext.
They can be easily implemented using mutual exclusion.
The size of the interval [begin;end] is called the granularity and is usually a parameter of the sched-
uler.

To compute numerical integration, note that the program needs to make sure that the result is correct,
to do so, you can enforce the mutual exclusion in three places:
• iteration, Within the most inner loop for each step of the numerical integration,
• chunk, By locally storing the value in execute inner loop and adding them to a shared variable once
per call.
• thread, By storing one value per thread, and aggregating to the shared variable once it is done.

Question: Write a dynamic loop scheduler to compute numerical integration. Navigate to the dynamic/
directory and use dynamic sched.cpp as a base code. Write the code so that it outputs the integral value
on stdout and the time it takes to make the computation on stderr.
The code should take the same parameter as the previous one except:
• sync can also take chunk as a synchronization method
• granularity, an integer indicating how many iterations does one thread take at a time.
Question: Report time and speedup across a range of precision, intensity, and synchronization mode. Use
make test to test your code. Your code MUST pass the test before you can use make bench to start the
PBS jobs. When complete, you should be able to plot with make plot. Note: you must run these commands
from within the dynamic/ directory.
