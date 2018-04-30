# rainbrot
A fractal generator using the buddhabrot method to render various fractal formulae


Usage: rainbrot-gen [-v?] [-b BAILOUT] [-f {mandelbrot,ship,custom}]
            [-i ITER1,ITER2[,...]] [-r RUNS] [-s WIDTHxHEIGHT] [-t THREADS]
            [-w RE_MIN,IM_MIN,RE_MAX,IM_MAX] [-x SEED] [--bail=BAILOUT]
            [--function={mandelbrot,ship,custom}] [--iter=ITER1,ITER2[,...]]
            [--runs=RUNS] [--size=WIDTHxHEIGHT] [--threads=THREADS]
            [--window=RE_MIN,IM_MIN,RE_MAX,IM_MAX] [--seed=SEED] [--verbose]
            [--help] [--usage]

rainbrot -- A program to generate histogram of probabilities that certain
region of Gauss plane will be the solution of an iteration of a random sampled
point using a specified complex iterative equation.

  -b, --bail=BAILOUT         Maximal absolute value, which will cause a point
                             to be discarded after reaching it
  -f, --function={mandelbrot,ship,custom}
  -i, --iter=ITER1,ITER2[,...]   Bands of iteration depths
  -r, --runs=RUNS            Number of starting points to be iterated (O means
                             until stop via Ctrl+C)
  -s, --size=WIDTHxHEIGHT    Size of image in pixels
  -t, --threads=THREADS      Number of threads to run the iterator in
  -w, --window=RE_MIN,IM_MIN,RE_MAX,IM_MAX
                             Displayed area of the Gauss plane
  -x, --seed=SEED            Starting seed for the random number generator
  -v, --verbose              Produce verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.