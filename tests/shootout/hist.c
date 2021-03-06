/*
 * hist.c
 *
 * Processes data from stdin, generating a histogram and some statistics
 * about the data.  Details of input format are given below.  Uses
 * the math library.
 *
 * To compile on most machines:
 *
 * cc hist.c -o hist -lm
 *
 * Author: Michael Hicks, Univerisity of Pennsylvania
 *
 * This program may be freely distributed under the GNU public license
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void USAGE(char *name) 
{ 
  fprintf(stderr,"usage : %s [-n num_bins | -f fraction_from_med] [-v]\n",
	  name); 
  fprintf(stderr,"  By default, the output consists of\n");
  fprintf(stderr,"  testname mean covariance median low-quartile hi-quartile scaled-semi-interquartile-range\n");
  fprintf(stderr,"  Passing -v prints these results graphically as a histogram.\n");
  exit(1); 
}

/* Datastructure used for histogram */
struct bin {
  double max;
  int hits;
};

#define MAX_POINTS 100

/*
 * print_stats
 *
 * Graphically prints the histogram, with the average and standard
 * deviation depicted along the side.  Median, low and high quartiles
 * are denoted by M, L, H, respectively.  Prints out the covariance
 * and the scaled Semi-Interquartile Range.
 *
 */
void print_stats(char *heapname, double min, struct bin *bins, int nbins,
                 int npoints, double var, double mean, double med, double SIQR)
{
  int i, j, m, qmed = npoints/2 + 1;
  int qlow = qmed - qmed/2;
  int qhi = qmed + qmed/2;
  double stddev = sqrt(var);
  double low = mean - stddev;
  double hi = mean + stddev;
#define START     0
#define DID_LOW   1
#define DID_MEAN  2
#define DID_HI    3
  int state = START;

  /* Header stats */
  printf("%s\n",heapname);
  printf("Covariance = %2.10lf",(stddev/mean));
  printf("  Scaled SIQR = %2.10lf\n",(SIQR/med));
  printf("  %lf [min]\n",min);

  /* Print histogram.  Uses state machine to print
     the spread of the average and standard deviation along
     the left side. */
  for (i=0,m=0; i<nbins; i++) {
    switch (state) {
      case START:
        if (bins[i].max >= low) {
          if (bins[i].max >= mean) {
            printf("A ");
            if (bins[i].max >= hi)
              state = DID_HI;
            else
              state = DID_MEAN;
          }
          else {
            printf("+ ");
            state = DID_LOW;
          }
        }
        else
          printf("  ");
        break;

      case DID_LOW:
        if (bins[i].max >= mean) {
          printf("A ");
          if (bins[i].max >= hi)
            state = DID_HI;
          else
            state = DID_MEAN;
        }
        else
          printf("| ");
        break;

      case DID_MEAN:
        if (bins[i].max >= hi) {
          printf("+ ");
          state = DID_HI;
        }
        else
          printf("| ");
        break;

      default:
        printf("  ");
        break;
    }

    /* print the bin value */
    printf("%lf : ",bins[i].max);

    /* print the bin contents */
    for (j=0; j<bins[i].hits; j++) {
      m++;
      if (m == qmed)
        printf("M");
      else if (m == qlow)
        printf("L");
      else if (m == qhi)
        printf("H");
      else
        printf("*");
    }
    printf("\n");
  }
}

/*
 * calc_stats
 *
 * Given the array of points, creates the histogram by storing the
 * hits in the bins.  Also calculates the mean, vairance and 
 * semi-interquartile range.
 *
 */
void calc_stats(struct bin *bins, int nbins, double *points, int npoints,
                double *var, double *mean, double *SIQR)
{
  int i, j, med = npoints/2 + 1;
  int qlow = med - med/2;
  int qhi = med + med/2;
  *mean = 0;
  *var = 0;

  /* Create the histogram */
  for (i=0; i<npoints; i++) {
    double point = points[i];
    *mean += points[i];
    for (j=0; j<nbins; j++)
      if (point < bins[j].max) {
        bins[j].hits++;
        break;
      }
    if (j == nbins) /* biggest point won't be added in loop */
      bins[j-1].hits++;
  }

  /* Find the mean and variance */
  (*mean) /= (double)npoints;

  for (i=0; i<npoints; i++) {
    double point = points[i];
    (*var) += (point - *mean) * (point - *mean);
  }
  (*var) /= (double)(npoints-1);

  /* Find the Semi-Interquartile Range; ASSUMES points[] sorted! */
  *SIQR = (points[qhi-1] - points[qlow-1]) / (double)2;
}

/*
 * init_bins
 *
 * based on the the range of the spread and the number of bins
 * to create, figures out the delta between each bin and initializes
 * the bin datastructures.
 *
 */
void init_bins(struct bin *bins, int nbins, double max, double min)
{
  double delta = (max - min) / (double)nbins;
  int i;

  bins[0].max = min + delta;
  bins[0].hits = 0;

  for (i=1; i<nbins; i++) {
    bins[i].max = bins[i-1].max + delta;
    bins[i].hits = 0;
  }
  bins[nbins-1].max = max;
}

int comp_doub(void *a, void *b)
{
  double ad = *(double *)a, bd = *(double*)b;
  if (ad<bd) return -1;
  else if (ad>bd) return 1;
  else return 0;
}

/*
 * get_max_min_med
 *
 * Given the points to examine, figures out the maximum, minimum,
 * and median of those points.  As a side-effect, the points array
 * is sorted.
 *
 */
void get_max_min_med(double *points, int npoints, double *max, double *min,
                     double *med)
{
  int i;

  qsort((void *)points,npoints,sizeof(double),comp_doub);

  *max = points[npoints-1];
  *min = points[0];
  *med = points[npoints/2];
}
  
/*
 * main
 *
 * reads from standard input lines of the format
 *
 * title1 val1_1 val1_2 val1_3 ...
 * title2 val2_1 val2_2 val2_3 ...
 * ...
 *
 * for each line, statistics are generated about the vals present,
 * including a histogram, the average, median, etc.
 *
 * For the histogram, the number of bins can be set up either explicitly
 * by setting the # with -n, or implicitly by specifying the size of the
 * bins as a fraction of the median value with -f
 *
 */
 

int main(int argc, char *argv[])
{
  char buffer[2000];
  FILE *infp = NULL;
  int nbins = 20, idx = 0;
  char *heapname, *point;
  double max, min, delta;
  struct bin *bins = NULL;
  double points[MAX_POINTS];
  double var, mean, SIQR, med, bindiff = 0.005;
  char **arg;
  int verbose = 0;

  /* get arguments */
  arg = argv + 1;
  while (*arg != NULL) {
    if ((*arg)[0] == '-') {
      switch((*arg)[1]) {
        case 'i':
          arg++;
	  if (infp != NULL) fclose(infp);
	  infp = fopen(*arg,"r");
	  if (infp == NULL) {
	    fprintf(stderr,"Could not open input file \"%s\"\n",arg);
	    USAGE(argv[0]);
	  }
	  break;

        case 'n':
          arg++;
          bindiff = 0.0; /* disable runtime divvying of bin sizes */
          nbins = atoi(*arg);
          bins = (struct bin *)calloc(nbins, sizeof(struct bin));
          if (bins == NULL) {
            fprintf(stderr,"Calloc failed\n");
            exit(1);
          }
          break;

        case 'f':
          arg++;
          sscanf(*arg,"%lf",&bindiff);
          break;

        case 'v':
	  verbose=1;
	  break;

        case '?':
          USAGE(argv[0]);

        default:
          fprintf(stderr,"Unknown option %s\n",*arg);
          USAGE(argv[0]);
     }
     arg++;
   }
   else
     break;
  }

  if (infp == NULL) infp = stdin;

  /* read/process input one line at a time;
     store the input points into the points array */ 
  while (fgets(buffer,sizeof(buffer),infp) != NULL) {
    heapname =  strtok(buffer," ");
    point = strtok(NULL," ");
    idx = 0;
    while (point != NULL) {
      if (idx == MAX_POINTS) {
        fprintf(stderr,"Can't have more than %d input points!\n",MAX_POINTS);
        exit(1);
      }
      if (sscanf(point,"%lf",&points[idx++]) != 1) {
	point = NULL;
	idx--;
      }
      point = strtok(NULL," ");
    }

    get_max_min_med(points,idx,&max,&min,&med);

    /* calculate/create # of bins if using fraction_from_med determination */
    if (bindiff != 0.0) {
      double delta = med * bindiff;
      nbins = ((max-min) / delta) + 1;

      if (bins != NULL)
        free(bins);
      bins = (struct bin *)calloc(nbins, sizeof(struct bin));
      if (bins == NULL) {
        fprintf(stderr,"Calloc failed\n");
        exit(1);
      }
    }

    init_bins(bins,nbins,max,min);
    calc_stats(bins,nbins,points,idx,&var,&mean,&SIQR);
    if (verbose)
      print_stats(heapname,min,bins,nbins,idx,var,mean,med,SIQR);
    else {
      double stddev, lowquart, hiquart;
      int qmed, qlow, qhi;
      stddev = sqrt(var);
      qlow = idx / 4;
      qhi = 3 * idx / 4;
      lowquart = points[qlow];
      hiquart = points[qhi];
      printf("%s %lf %lf %lf %lf %lf %lf\n",
	     heapname, mean, stddev/mean, med, lowquart, hiquart, SIQR);
    }
  }
  fclose(infp);

  return 0;
}
