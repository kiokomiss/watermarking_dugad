#define _CRT_SECURE_NO_WARNINGS 

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAPALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <time.h>

#include <fstream>

#include "wm.h"
#include "wingetopt.h"
#include "pipes.h"
#include "wavelet.h"
#include "dwt.h"
#include "dwt_util.h"

#define MAXPATHLEN 255

using namespace std;

void usage_gen_dugad_sig(void){

	fprintf(stderr, "usage: -p generate [-a n] [-e n] [-f n] [-F file] [-l n] [-n n] [-o file] [-s file] [-S n] [-t n] [-T n]\n\n");
	fprintf(stderr, "\t-a n\t\talpha factor (default 0.2)\n");
	fprintf(stderr, "\t-d n\t\tdeviation (default 1.0)\n");
	fprintf(stderr, "\t-e n\t\twavelet filtering method (default 2)\n");
	fprintf(stderr, "\t-f n\t\tfilter number (default 1)\n");
	fprintf(stderr, "\t-F file\t\tfilter definition file (default 'filter.dat')\n");
	fprintf(stderr, "\t-h\t\tprint usage\n");
	fprintf(stderr, "\t-l n\t\tdecomposition levels (default 3)\n");
	fprintf(stderr, "\t-m n \t\tmean value (default 0.0)\n");
	fprintf(stderr, "\t-n n\t\twatermark length (default 1000)\n");
	fprintf(stderr, "\t-o file\t\toutput file\n");
	fprintf(stderr, "\t-s file\t\tsignature to embed in input image\n");
	fprintf(stderr, "\t-S n\t\tseed\n");
	fprintf(stderr, "\t-t n\t\tcasting threshold (default 40.0)\n");
	fprintf(stderr, "\t-T n\t\tdetection threshold (default 50.0)\n");
	exit(0);
}

void usage_wm_dugad_e(void) {
	fprintf(stderr, "usage: -p embed [-a n] [-e n] [-f n] [-F file] [-h] [-l n] [-o file] [-t n] [-v n] -s file file\n\n");
	fprintf(stderr, "\t-a n\t\talpha factor/embedding strength\n");
	fprintf(stderr, "\t-e n\t\twavelet filtering method\n");
	fprintf(stderr, "\t-f n\t\tfilter number\n");
	fprintf(stderr, "\t-F file\t\tfilter definition file\n");
	fprintf(stderr, "\t-h\t\tprint usage\n");
	fprintf(stderr, "\t-l n\t\tdecomposition levels\n");
	fprintf(stderr, "\t-o file\t\toutput (watermarked) file\n");
	fprintf(stderr, "\t-s file\t\tsignature to embed in input image\n");
	fprintf(stderr, "\t-t n\t\tcasting threshold\n");
	fprintf(stderr, "\t-v n\t\tverbosity level\n");
	exit(0);
}

void usage_wm_dugad_d(void) {
	fprintf(stderr, "usage: -p extract [-a n] [-e n] [-f n] [-F file] [-h] [-l n] [-n n] [-o file] [-v n] [-t n] -s file file\n\n");
	fprintf(stderr, "\t-a n\t\talpha factor\n");
	fprintf(stderr, "\t-e n\t\twavelet filtering method\n");
	fprintf(stderr, "\t-f n\t\tfilter number\n");
	fprintf(stderr, "\t-F file\t\tfilter definition file\n");
	fprintf(stderr, "\t-h\t\tprint usage\n");
	fprintf(stderr, "\t-l n\t\tdecomposition levels\n");
	fprintf(stderr, "\t-n n\t\twatermark length\n");
	fprintf(stderr, "\t-o file\t\tfile for extracted watermark\n");
	fprintf(stderr, "\t-s file\t\toriginal signature file\n");
	fprintf(stderr, "\t-t n\t\tdetection threshold\n");
	fprintf(stderr, "\t-v n\t\tverbosity level\n");
	exit(0);
}

void usage_cmp_dugad_sig(void) {
	fprintf(stderr, "usage: -p compare [-h] [-s file] [-C] [-o file] [-v] file\n\n");
	fprintf(stderr, "\t-C\t\toutput correlation only\n");
	fprintf(stderr, "\t-h\t\tprint usage\n");
	fprintf(stderr, "\t-o file\t\toutput file\n");
	fprintf(stderr, "\t-s file\t\tignored\n");
	exit(0);
}

void usage_main(void) {

	fprintf(stderr, "Usage: Dugad_pipe_filter -p <operation> <options>\n");
	fprintf(stderr, "  Operations:\n");
	fprintf(stderr, "    generate  : generate an appropriate signature file for the corresponding embedding/detection algorithm\n");
	fprintf(stderr, "    embed  : perform embedding the watermark\n");
	fprintf(stderr, "    extract : extract the embedded signature from video siquence\n");
	fprintf(stderr, "    compare  : compare the original signature against the extracted signature\n");
	fprintf(stderr, "  Operation-specific options:\n");
	usage_gen_dugad_sig();
	usage_wm_dugad_e();
	usage_wm_dugad_d();
	usage_cmp_dugad_sig();
}

typedef enum {
	op_generate,
	op_embed,
	op_extract,
	op_compare
}OPERATION;

OPERATION g_Operation;

char *progname;



int gen_dugad_sig(int argc, char **argv)
{
	/* Generate .dat file  with pseudo random sequence of n digitals,
	which is used as watermark for embedding
	*/
	FILE *out = stdout;
	FILE *sig = NULL;

	char output_name[MAXPATHLEN] = "C:\\WM_test\\Dugad_test.sig";
	char signature_name[MAXPATHLEN];

	char oPeration[MAXPATHLEN] = "generate";
	int c;
	int n = 1000;
	int s = 0;
	int e = 2;
	int f = 1;
	int l = 3;
	char F[MAXPATHLEN] = "C:\\WM_test\\filter.dat";
	double a = 0.2;
	double t1 = 40.0;
	double t2 = 50.0;
	double m = 0.0;
	double d = 1.0;

	progname = argv[0];

	while ((c = getopt(argc, argv, "p:a:b:d:e:f:F:h?l:m:n:o:s:S:t:T:")) != EOF) {
		switch (c) {
		case 'p':
			strcpy(oPeration, optarg);
			if (strcmp("generate", optarg)) {
				fprintf(stderr, "%s: The operation  %s is incompatible  \n", progname, oPeration);
				exit(1);
			}
			break;
		case 'a':
			a = atof(optarg);
			if (a <= 0.0) {
				fprintf(stderr, "%s: alpha factor %f out of range\n", progname, a);
				exit(1);
			}
			break;
		case 'd':
			d = atof(optarg);
			if (d <= 0.0) {
				fprintf(stderr, "%s: deviation %f out of range\n", progname, d);
				exit(1);
			}
			break;

		case 'e':
			e = atoi(optarg);
			if (e < 0) {
				fprintf(stderr, "%s: wavelet filtering method %d out of range\n", progname, e);
			}
			break;
		case 'f':
			f = atoi(optarg);
			if (f <= 0) {
				fprintf(stderr, "%s: filter number %d out of range\n", progname, f);
				exit(1);
			}
			break;
		case 'F':
			strcpy(F, optarg);
			break;
		case 'h':
		case '?':
			usage_gen_dugad_sig();
			break;
		case 'l':
			l = atoi(optarg);
			if (l <= 0) {
				fprintf(stderr, "%s: decomposition level %d out of range\n", progname, l);
				exit(1);
			}
			break;
		case 'm':
			m = atof(optarg);
			break;
		case 'n':
			n = atoi(optarg);
			if (n < 1 || n > 32000) {
				fprintf(stderr, "%s: watermark length %d out of range\n", progname, n);
				exit(1);
			}
			break;
		case 'o':
			if ((out = fopen(optarg, "w")) == NULL) {
				fprintf(stderr, "%s: unable to open output file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(output_name, optarg);
			break;
		case 's':
			if ((sig = fopen(optarg, "r")) == NULL) {
				fprintf(stderr, "%s: unable to open signature file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(signature_name, optarg);
			break;
		case 'S':
			s = atoi(optarg);
			break;
		case 't':
			t1 = atof(optarg);
			if (t1 <= 0.0) {
				fprintf(stderr, "%s: casting threshold %f out of range\n", progname, t1);
				exit(1);
			}
			break;
		case 'T':
			t2 = atof(optarg);
			if (t2 <= 0.0) {
				fprintf(stderr, "%s: detection threshold %f out of range\n", progname, t2);
				exit(1);
			}
			break;
		}
	}

	argc -= optind;
	argv += optind;

	/*if (argc > 1) {
		usage_gen_dugad_sig();
		exit(1);
	}
	*/

	if (sig) {
		char line[32];
		fgets(line, sizeof(line), sig);
		if (strspn(line, "DGSG") >= 4) {
			if (n == 0)
				fscanf(sig, "%d\n", &n);
			else
				fscanf(sig, "%*d\n");
			if (l == 0)
				fscanf(sig, "%d\n", &l);
			else
				fscanf(sig, "%*d\n");
			if (a == 0.0)
				fscanf(sig, "%lf\n", &a);
			else
				fscanf(sig, "%*f\n");
			if (t1 == 0.0)
				fscanf(sig, "%lf\n", &t1);
			else
				fscanf(sig, "%*f\n");
			if (t2 == 0.0)
				fscanf(sig, "%lf\n", &t2);
			else
				fscanf(sig, "%*f\n");
			if (e < 0)
				fscanf(sig, "%d\n", &e);
			else
				fscanf(sig, "%*d\n");
			if (f == 0)
				fscanf(sig, "%d\n", &f);
			else
				fscanf(sig, "%*d\n");
			if (!strcmp(F, ""))
				fscanf(sig, "%[^\n\r]\n", F);
			else
				fscanf(sig, "%*[^\n\r]\n");
		}
		else {
			fprintf(stderr, "%s: invalid signature file %s\n", progname, signature_name);
			exit(1);
		}
	}


	out = fopen(output_name, "w");

	if (s)
		srand(s);
	else
		srand(time(NULL)); //original code: srand(time(NULL) * getpid());

	fprintf(out, "DGSG\n");
	fprintf(out, "%d\n", n);
	fprintf(out, "%d\n", l);
	fprintf(out, "%f\n", a);
	fprintf(out, "%f\n", t1);
	fprintf(out, "%f\n", t2);
	fprintf(out, "%d\n", e);
	fprintf(out, "%d\n", f);
	fprintf(out, "%s\n", F);

	n >>= 1;
	while (n > 0) {
		double x;
		double x1, x2;

		/*
		* Algorithm P (Polar method for normal deviates),
		* Knuth, D., "The Art of Computer Programming", Vol. 2, 3rd Edition, p. 122
		*/

		do {
			x1 = 2.0 * ((rand() & RAND_MAX) / ((double)RAND_MAX + 1.0)) - 1.0;
			x2 = 2.0 * ((rand() & RAND_MAX) / ((double)RAND_MAX + 1.0)) - 1.0;
			x = x1 * x1 + x2 * x2;
		} while (x >= 1.0);
		x1 *= sqrt((-2.0) * log(x) / x);
		x2 *= sqrt((-2.0) * log(x) / x);

		fprintf(out, "%f\n", m + d * x1);
		fprintf(out, "%f\n", m + d * x2);

		n--;
	}
	cout << "Signature file name: " << output_name;
	fclose(out);

	//else fprintf(stderr, "%s: unable to open output file\n", progname);

	return 0;
}

void wm_subband(Image s, double *w, int n, double a, double t1)
{
	// embeds a watermark of n normally distributed values into a coefficients > threshold t1 of a subband
	int i, count_coeff=0;

	for (i = 0; i < s->width * s->height; i++)
	{
		//printf("abs(data[i])=%f \t", fabs(s->data[i]));
		if (fabs(s->data[i]) > t1)
		{
			//printf("abs(data[i])=%f \t", fabs(s->data[i]));
			s->data[i] += (a * fabs(s->data[i]) * w[i % n]);
			++count_coeff;
		}
	}
		//printf("\nCount_coeff= %d", count_coeff);
         
}

void wm_subband_d(Image s, double *w, int n, double t2, int m[], double z[], double v[], int count_line) {
	int i;

	for (i = 0; i < s->width * s->height; i++)
		if (s->data[i] > t2) {
			z[count_line] += (s->data[i] * w[i % n]);
			v[count_line] += fabs(s->data[i]);
			m[count_line] ++;
		}
}

void wm_dugad_e(int argc, char **argv)
{
	unsigned int time1 = GetTickCount();
	FILE *in = stdin;
	FILE *out = stdout;
	FILE *sig = NULL;
	FILE *outPNG = NULL;

	char oPeration[MAXPATHLEN] = "embed";
	char output_name[MAXPATHLEN] = "(stdout)";
	char input_name[MAXPATHLEN] = "(stdin)";
	char signature_name[MAXPATHLEN];
	char PNGfilename[MAXPATHLEN] = "C:\\WM_test\\Test\\Test_PNG_horisontal_before.png";
	
	int i, c;
	int n;

	double alpha = 0.0;
	double t1 = 0.0;

	int level = 0;
	int filter = 0;
	int method = -1;
	char filter_name[MAXPATHLEN] = "";

	int verbose = 0;

	Image_tree dwts, s, s2;
	gray maxval = 255;
	int rows, cols, format = 8;//assume format to be a bitdepth
	int framerate = 0, num_of_frames = 0;
	int  total_frames = 0, total_planes = 0;
	int count_frames = 0, count_planes = 0;
	double *watermark;

	progname = argv[0];

	while ((c = getopt(argc, argv, "p:a:e:f:F:h?l:o:s:t:v:")) != EOF) {
		switch (c) {
		case 'p':
			strcpy(oPeration, optarg);
			if (strcmp("embed", optarg)) {
				fprintf(stderr, "%s: The operation  %s is incompatible  \n", progname, oPeration);
				exit(1);
			}
			break;
		case 'a':
			alpha = atof(optarg);
			if (alpha <= 0.0) {
				fprintf(stderr, "%s: alpha factor %f out of range\n", progname, alpha);
				exit(1);
			}
			break;
		case 'e':
			method = atoi(optarg);
			if (method < 0) {
				fprintf(stderr, "%s: wavelet filtering method %d out of range\n", progname, method);
				exit(1);
			}
			break;
		case 'f':
			filter = atoi(optarg);
			if (filter <= 0) {
				fprintf(stderr, "%s: filter number %d out of range\n", progname, filter);
				exit(1);
			}
			break;
		case 'F':
			strcpy(filter_name, optarg);
			break;
		case 'h':
		case '?':
			usage_main();
			break;
		case 'l':
			level = atoi(optarg);
			if (level <= 0) {
				fprintf(stderr, "%s: decomposition level %d out of range\n", progname, level);
				exit(1);
			}
			break;
		case 'o':
			if ((out = fopen(optarg, "wb")) == NULL) {
				fprintf(stderr, "%s: unable to open output file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(output_name, optarg);
			break;
		case 's':
			if ((sig = fopen(optarg, "r")) == NULL) {
				fprintf(stderr, "%s: unable to open signature file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(signature_name, optarg);
			break;
		case 't':
			t1 = atof(optarg);
			if (t1 <= 0.0) {
				fprintf(stderr, "%s: casting threshold %f out of range\n", progname, t1);
				exit(1);
			}
			break;
		case 'v':
			verbose = atoi(optarg);
			if (verbose < 0) {
				fprintf(stderr, "%s: verbosity level %d out of range\n", progname, verbose);
				exit(1);
			}
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 1) {
		usage_wm_dugad_e();
		exit(1);
	}

	if (argc == 1 && *argv[0] != '-') {
		if ((in = fopen(argv[0], "rb")) == NULL) {
			fprintf(stderr, "%s: unable to open input file %s\n", progname, argv[0]);
			exit(1);
		}
		else
			strcpy(input_name, argv[0]);
	}

	if (sig) {
		char line[32];
		fgets(line, sizeof(line), sig);
		if (strspn(line, "DGSG") >= 4) {
			fscanf(sig, "%d\n", &n);
			if (level == 0)
				fscanf(sig, "%d\n", &level);
			else
				fscanf(sig, "%*d\n");
			if (alpha == 0.0)
				fscanf(sig, "%lf\n", &alpha);
			else
				fscanf(sig, "%*f\n");
			if (t1 == 0.0)
				fscanf(sig, "%lf\n", &t1);
			else
				fscanf(sig, "%*f\n");
			fscanf(sig, "%*f\n");
			if (method < 0)
				fscanf(sig, "%d\n", &method);
			else
				fscanf(sig, "%*d\n");
			if (filter == 0)
				fscanf(sig, "%d\n", &filter);
			else
				fscanf(sig, "%*d\n");
			if (!strcmp(filter_name, ""))
				fscanf(sig, "%[^\n\r]\n", filter_name);
			else
				fscanf(sig, "%*[^\n\r]\n");
		}
		else {
			fprintf(stderr, "%s: invalid signature file %s\n", progname, signature_name);
			exit(1);
		}
	}
	else {
		fprintf(stderr, "%s: signature file not specified, use -s file option\n", progname);
		exit(1);
	}

	watermark = (double *)malloc(n * sizeof(double));
	for (i = 0; i < n; i++)
		fscanf(sig, "%lf\n", &watermark[i]);
	fclose(sig);


	getVideoContext(input_name, cols, rows, total_frames, framerate, format);

	in = getVidIStream(input_name);
	out = getVidOFStream(output_name, cols, rows);
	
	if (in == NULL || out == NULL) {
		printf("Couldn't open pipes: %i %i\n", in, out);
		exit(0);
	}

	unsigned int pixels_in_plane = cols* rows;
	unsigned int stride = cols* rows * 2;

	total_planes = total_frames * 3;
	total_planes = 120;
	init_dwt(cols/2, rows, filter_name, filter, level, method);

	FILE *outDWTHH = NULL;
	char *filenameDWTHH = "C:\\WM\\TEST_Dugad\\Test_dugad_DWTHH.avi";
	FILE *outDWTHL = NULL;
	char *filenameDWTHL = "C:\\WM\\TEST_Dugad\\Test_dugad_DWTHL.avi";
	FILE *outDWTLH = NULL;
	char *filenameDWTLH = "C:\\WM\\TEST_Dugad\\Test_dugad_DWTLH.avi";
	// for yuv422p
	while (count_planes < total_planes) 
	{

		if (((count_planes + 1) % 3) != 1) //if (true) 
		{
			//embed signature in U plane
			gray * image_UV = (gray*)malloc(pixels_in_plane / 2 * sizeof(gray));

			fread(image_UV, pixels_in_plane / 2, sizeof(gray), in);
			//printf("\n FDWT start working...");
			// wavelet transform
			dwts = fdwt(image_UV);

		/*	for (i = 0, s2 = dwts; i < 3; i++, s2 = s2->coarse)
			{
				if ((i == 0)&&(((count_planes + 1) % 3) == 2))
				{
					Image s1_horizontal, s1_diagonal,s1_vertical;
					s1_horizontal = get_absolute_image_scaled(s2->horizontal->image);
					//s1_diagonal = get_absolute_image_scaled(s2->diagonal->image);
					//s1_vertical = get_absolute_image_scaled(s2->vertical->image);
					gray* s1_horizontal_gray = (gray*)malloc(s1_horizontal->size * sizeof(gray));
					//gray* s1_diagonal_gray = (gray*)malloc(s1_diagonal->size * sizeof(gray));
					//gray* s1_vertical_gray = (gray*)malloc(s1_vertical->size * sizeof(gray));
					image_to_gray(s1_horizontal, s1_horizontal_gray);
					//image_to_gray(s1_diagonal, s1_diagonal_gray);
					//image_to_gray(s1_vertical, s1_vertical_gray);
					//printf("\ns1_horizontal->size=%d \n", s1_horizontal->size);
					//printf("\ns2->horizontal->image->size=%d \n", s2->horizontal->image->size);
					//outPNG = getTestPNG(PNGfilename, s1_horizontal->width, s1_horizontal->height);
					//fwrite(s1_horizontal_gray, s1_horizontal->size, sizeof(gray), outPNG);
					//fflush(outPNG);
					outDWTHH = getVidOFMPEG4(filenameDWTHH, s1_horizontal->width, s1_horizontal->height);
				    //outDWTHL = getVidOFMPEG4(filenameDWTHL, s1_diagonal->width, s1_diagonal->height);
					//outDWTLH = getVidOFMPEG4(filenameDWTLH, s1_vertical->width, s1_vertical->height);
					fwrite(s1_horizontal_gray, s1_horizontal->size, sizeof(gray), outDWTHH);
					//fwrite(s1_diagonal_gray, s1_diagonal->size, sizeof(gray), outDWTHL);
					//fwrite(s1_vertical_gray, s1_vertical->size, sizeof(gray), outDWTLH);
					fflush(outDWTHH);
					//fflush(outDWTLH);
					//fflush(outDWTHL);
					free_image(s1_horizontal);
					//free_image(s1_diagonal);
					//free_image(s1_vertical);
					free(s1_horizontal_gray);
					//free(s1_diagonal_gray);
					//free(s1_vertical_gray);
				}
			} */
			
			// embed watermark in all subbands of a decomposition level
				for (i = 0, s = dwts; i < 3; i++, s = s->coarse) {
					//s->horizontal->image = get_absolute_image_scaled(s->horizontal->image);
					//s->vertical->image = get_absolute_image_scaled(s->vertical->image);
					//s->diagonal->image = get_absolute_image_scaled(s->diagonal->image);
					wm_subband(s->horizontal->image, watermark, n, alpha, t1);
					wm_subband(s->vertical->image, watermark, n, alpha, t1);
					wm_subband(s->diagonal->image, watermark, n, alpha, t1);
				}
				//printf("\n IDWT start working...");
				idwt(dwts, image_UV);

				fwrite(image_UV, pixels_in_plane / 2, sizeof(gray), out);
				free(image_UV);

			}
		
		else 
		  {
		
			gray * image = (gray*)malloc(pixels_in_plane * sizeof(gray));

			fread(image, pixels_in_plane, sizeof(gray), in);
			
			fwrite(image, pixels_in_plane, sizeof(gray), out);
			free(image);
		  }
		count_planes++;
		printf("\nCount_planes=%d ", count_planes);
		//((count_planes) % 3) == 0) ? (count_frames++) : (count_frames);)
		fflush(out);
		fflush(in);
		
	}
	_pclose(in);
	_pclose(out);
	_pclose(outDWTHH);
	_pclose(outDWTHL);
	_pclose(outDWTLH);

	free(watermark);

	unsigned int time2 = GetTickCount();
	unsigned int time_count = (time2 - time1) / 1000;
	printf("Execution time=%d ", time_count);
	//system("pause");

	exit(0);
}

void wm_dugad_d(int argc, char **argv)
{
	unsigned int time1 = GetTickCount();
	FILE *in = stdin;
	FILE *out = stdout;
	FILE *sig = NULL;

	char oPeration[MAXPATHLEN] = "extract";
	char signature_name[MAXPATHLEN];
	char output_name[MAXPATHLEN] = "(stdout)";
	char input_name[MAXPATHLEN] = "(stdin)";

	int c, i;
	int n = 0;
	int method = -1;
	int level = 0;
	int filter = 0;
	char filter_name[MAXPATHLEN] = "";

	double alpha = 0.0;
	double t2 = 0.0;

	gray in_maxval = 255;
	int rows, cols, format = 8;//assume format to be a bitdepth
	int row;
	int framerate = 0, num_of_frames = 0;
	int  total_frames = 0, total_planes = 0;
	int count_frames = 0, count_planes = 0;

	double *watermark;

	Image_tree dwts, s;

	int verbose = 0;

	progname = argv[0];

	while ((c = getopt(argc, argv, "p:a:e:f:F:h?l:n:o:s:v:t")) != EOF) {
		switch (c) {
		case 'p':
			strcpy(oPeration, optarg);
			if (strcmp("extract", optarg)) {
				fprintf(stderr, "%s: The operation  %s is incompatible  \n", progname, oPeration);
				exit(1);
			}
			break;
		case 'a':
			alpha = atof(optarg);
			if (alpha <= 0.0) {
				fprintf(stderr, "%s: alpha factor %f out of range\n", progname, alpha);
				exit(1);
			}
			break;
		case 'e':
			method = atoi(optarg);
			if (method < 0) {
				fprintf(stderr, "%s: wavelet filtering method %d out of range\n", progname, method);
				exit(1);
			}
			break;
		case 'f':
			filter = atoi(optarg);
			if (filter <= 0) {
				fprintf(stderr, "%s: filter number %d out of range\n", progname, filter);
				exit(1);
			}
			break;
		case 'F':
			strcpy(filter_name, optarg);
			break;
		case 'h':
		case '?':
			usage_main();
			break;
		case 'l':
			level = atoi(optarg);
			if (level <= 0 || level > 20) {
				fprintf(stderr, "%s: decomposition level %d out of range\n", progname, level);
				exit(1);
			}
			break;

		case 'n':
			n = atoi(optarg);
			if (n < 1 || n > 32000) {
				fprintf(stderr, "%s: watermark length %d out of range\n", progname, n);
				exit(1);
			}
			break;
		case 'o':
			if ((out = fopen(optarg, "w+")) == NULL) {
				fprintf(stderr, "%s: unable to open output file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(output_name, optarg);
			break;
		case 's':
			if ((sig = fopen(optarg, "r")) == NULL) {
				fprintf(stderr, "%s: unable to open signature file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(signature_name, optarg);
			break;
		case 't':
			t2 = atof(optarg);
			if (t2 <= 0.0) {
				fprintf(stderr, "%s: detection threshold %f out of range\n", progname, t2);
				exit(1);
			}
			break;
		case 'v':
			verbose = atoi(optarg);
			if (verbose < 0) {
				fprintf(stderr, "%s: verbosity level %d out of range\n", progname, verbose);
				exit(1);
			}
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 1) {
		usage_wm_dugad_d();
		exit(1);
	}

	if (argc == 1 && *argv[0] != '-') {
		if ((in = fopen(argv[0], "rb")) == NULL) {
			fprintf(stderr, "%s: unable to open input file %s\n", progname, argv[0]);
			exit(1);
		}
		else
			strcpy(input_name, argv[0]);
	}

	if (sig) {
		char line[32];
		fgets(line, sizeof(line), sig);
		if (strspn(line, "DGSG") >= 4) {
			fscanf(sig, "%d\n", &n);
			if (level == 0)
				fscanf(sig, "%d\n", &level);
			else
				fscanf(sig, "%*d\n");
			if (alpha == 0.0)
				fscanf(sig, "%lf\n", &alpha);
			else
				fscanf(sig, "%*f\n");
			fscanf(sig, "%*f\n");
			if (t2 == 0.0)
				fscanf(sig, "%lf\n", &t2);
			else
				fscanf(sig, "%*f\n");
			if (method < 0)
				fscanf(sig, "%d\n", &method);
			else
				fscanf(sig, "%*d\n");
			if (filter == 0)
				fscanf(sig, "%d\n", &filter);
			else
				fscanf(sig, "%*d\n");
			if (!strcmp(filter_name, ""))
				fscanf(sig, "%[^\n\r]\n", filter_name);
			else
				fscanf(sig, "%*[^\n\r]\n");
		}
		else {
			fprintf(stderr, "%s: invalid signature file %s\n", progname, signature_name);
			exit(1);
		}
	}
	else {
		fprintf(stderr, "%s: signature file not specified, use -s file option\n", progname);
		exit(1);
	}

	watermark = (double*)malloc(n * sizeof(double));
	for (int i = 0; i < n; i++)
		fscanf(sig, "%lf\n", &watermark[i]);
	fclose(sig);

	getVideoContext(input_name, cols, rows, total_frames, framerate, format);

	in = getVidIStream(input_name);

	if (in == NULL || out == NULL) {
		printf("Couldn't open pipes: %i %i\n", in, out);
		exit(0);
	}

#ifdef POLLEN_STUFF
#include "pollen_stuff.c"
#endif
#ifdef PARAM_STUFF
#include "param_stuff.c"
#endif
	unsigned int pixels_in_plane = cols* rows;
	unsigned int stride = cols* rows * 3;
	unsigned int const max_lines = 60; // max number of levels =20, number of subbund=3, so max number of lines =20*3
	//total_frames = 2;
	total_planes = total_frames * 3;
	//total_planes = 6;
	init_dwt(cols/2, rows, filter_name, filter, level, method);
	//printf("\nDWT initialized\n");

	int num_line = 3 * level;
	int count_line = 0;
	int m[max_lines], no_zero_planes=0, m_previous=0;
	double z[max_lines];
	double v[max_lines];
	// Set zero value for vectors m, z, v
	for (i = 0; i < max_lines; i++)
	{
		m[i] = 0;
		z[i] = 0.00;
		v[i] = 0.00;
		//printf(" \n m[%d]=%d \t z[%d]=%f \t v[%d]=%f \n", count_line, m[count_line], count_line, z[count_line], count_line, v[count_line]);
	}

	fprintf(out, "DGWM\n");
	fprintf(out, "%d\n", level);
	fprintf(out, "%f\n", alpha);

	while (count_planes < total_planes) {

		if (((count_planes + 1) % 3) != 1)
		{
			gray * image = (gray*)malloc(pixels_in_plane/2 * sizeof(gray));
			fread(image, pixels_in_plane/2, sizeof(gray), in);
			// wavelet transform
			dwts = fdwt(image);

			for (i = 0, s = dwts; i < level; i++, s = s->coarse)
			{
				count_line = (i*level);
				wm_subband_d(s->horizontal->image, watermark, n, t2, m, z, v, count_line);
				//printf("%d %f %f\n", m[count_line], z[count_line], v[count_line]);

				count_line = (i*level) + 1;
				wm_subband_d(s->vertical->image, watermark, n, t2, m, z, v, count_line);
				//printf("%d %f %f\n", m[count_line], z[count_line], v[count_line]);

				count_line = (i*level) + 2;
				wm_subband_d(s->diagonal->image, watermark, n, t2, m, z, v, count_line);
				//printf("%d %f %f\n", m[count_line], z[count_line], v[count_line]);

			}
			(m[count_line] > m_previous) ? ++no_zero_planes, m_previous = m[count_line] : no_zero_planes;
			//printf("\nCount_planes=%d   \t No_zero_planes=%d  ", count_planes, no_zero_planes);
			//printf("%d %f %f\n", m[count_line], z[count_line], v[count_line]);
			idwt(dwts, image);
			free(image);
		}
    
		count_planes++;
		//((count_planes) % 3) == 0) ? (count_frames++) : (count_frames);)
		
		fflush(in);
	}

	for (i = 0; i < num_line; i++)
	{

		(m[i] != 0) ? m[i] /= (no_zero_planes) : m[i];
		(z[i] != 0) ? z[i] /= (no_zero_planes) : z[i];
		(v[i] != 0) ? v[i] /= (no_zero_planes) : v[i];
		fprintf(out, "%d %f %f\n", m[i], z[i], v[i]);
		fflush(out);
	}
	_pclose(in);
	_pclose(out);

	free(watermark);
	unsigned int time2 = GetTickCount();
	unsigned int time_count = (time2 - time1) / 1000;
	printf("Execution time=%d ", time_count);
	//system("pause");
	exit(0);
}

void cmp_dugad_sig(int argc, char **argv) {

	FILE *in = stdin;
	FILE *out = stdout;

	char oPeration[MAXPATHLEN] = "compare";
	char output_name[MAXPATHLEN] = "(stdout)";
	char input_name[MAXPATHLEN] = "(stdin)";

	int c, i, n, ok;
	int levels;
	double alpha;
	double diff;
	char line[32];

	int correlation_only = 0;
	int verbose = 0;

	progname = argv[0];

	while ((c = getopt(argc, argv, "p:h?Co:v:s:")) != EOF) {
		switch (c) {
		case 'p':
			strcpy(oPeration, optarg);
			if (strcmp("compare", optarg)) {
				fprintf(stderr, "%s: The operation  %s is incompatible  \n", progname, oPeration);
				exit(1);
			}
			break;
		case 'h':
		case '?':
			usage_main();
			break;
		case 'C':
			correlation_only = 1;
			break;
		case 's':
			break;
		case 'o':
			if ((out = fopen(optarg, "w")) == NULL) {
				fprintf(stderr, "%s: unable to open output file %s\n", progname, optarg);
				exit(1);
			}
			strcpy(output_name, optarg);
			break;
		case 'v':
			verbose = atoi(optarg);
			if (verbose < 0) {
				fprintf(stderr, "%s: verbosity level %d out of range\n", progname, verbose);
				exit(1);
			}
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 1) {
		usage_main();
		exit(1);
	}

	if (argc == 1 && *argv[0] != '-') {
		if ((in = fopen(argv[0], "r")) == NULL) {
			fprintf(stderr, "%s: unable to open input file %s\n", progname, argv[0]);
			exit(1);
		}
		else
			strcpy(input_name, argv[0]);
	}

	fgets(line, sizeof(line), in);
	if (strspn(line, "DGWM") < 4) {
		fprintf(stderr, "%s: watermark file %s invalid\n", progname, input_name);
		exit(1);
	}

	fscanf(in, "%d\n", &levels);
	fscanf(in, "%lf\n", &alpha);

	n = 3 * levels;
	ok = 0;
	diff = 0.0;
	for (i = 0; i < levels; i++) {
		int m;
		double z, v;

		// HL subband
		fscanf(in, "%d %lf %lf\n", &m, &z, &v);
		if (verbose && !correlation_only) {
			if (m)
				fprintf(out, "%f %f\n", z / (double)m, (v * alpha) / (double)(1.0 * m));
			else
				fprintf(out, "0.0 0.0\n");
		}
		if (m) {
			ok += (z > v * alpha / (double) 1.0) ? 1 : 0;
			diff += ((z - v * alpha) / (double)(1.0 * m));
		}
		else
			n--;

		// LH subband
		fscanf(in, "%d %lf %lf\n", &m, &z, &v);
		if (verbose && !correlation_only) {
			if (m)
				fprintf(out, "%f %f\n", z / (double)m, (v * alpha) / (double)(1.0 * m));
			else
				fprintf(out, "0.0 0.0\n");
		}
		if (m) {
			ok += (z > v * alpha / (double) 1.0) ? 1 : 0;
			diff += ((z - v * alpha) / (double)(1.0 * m));
		}
		else
			n--;

		// HH subband
		fscanf(in, "%d %lf %lf\n", &m, &z, &v);
		if (verbose && !correlation_only) {
			if (m)
				fprintf(out, "%f %f\n", z / (double)m, (v * alpha) / (double)(1.0 * m));
			else
				fprintf(out, "0.0 0.0\n");
		}

		if (m) {
			ok += (z > v * alpha / (double) 1.0) ? 1 : 0;
			diff += ((z - v * alpha) / (double)(1.0 * m));
		}
		else
			n--;
	}

	if (!correlation_only)
		fprintf(out, "%d/%d, diff %f\n", ok, n, diff);
	fprintf(out, "%f\n", (double)ok / (double)n);

	fclose(out);
	fclose(in);

	exit(0);
}


int Parse_Params(int argc, char **argv) {

	if (argc < 3)
	{
		usage_main();
		return -1;
	}
	if (!strcmp(argv[2], "generate"))
	{
		g_Operation = op_generate;
	}
	else if (!strcmp(argv[2], "embed"))
	{
		g_Operation = op_embed;
	}
	else if (!strcmp(argv[2], "extract"))
	{
		g_Operation = op_extract;
	}
	else if (!strcmp(argv[2], "compare"))
	{
		g_Operation = op_compare;
	}
	else
	{
		fprintf(stderr, "ERROR: Unknown operation (%s)\n\n", argv[2]);
		return -1;
	}

	if (g_Operation == op_generate)
	{

		gen_dugad_sig(argc, argv);
		return 0;
	}
	else if (g_Operation == op_embed)
	{
		cout << "Embedding process...\n";
		wm_dugad_e(argc, argv);
		return 0;
	}
	else if (g_Operation == op_extract)
	{
		cout << "Extracting watermark...\n";
		wm_dugad_d(argc, argv);
		return 0;
	}
	else if (g_Operation == op_compare)
	{
		cout << "Comparing watermarks...\n";
		cmp_dugad_sig(argc, argv);
		return 0;
	}
	return 0;
}

int main(int argc, char **argv)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/*HANDLE hLogFile;

	TCHAR LogfilePath[30] = TEXT("C:\\WM_test\\Test\\log.txt");

	hLogFile = CreateFile(LogfilePath, GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, hLogFile);
	*/
	Parse_Params(argc, argv);

	/*_RPT0(_CRT_WARN, "file message\n");
	CloseHandle(hLogFile);*/
	return 0;
}

