#define _CRT_SECURE_NO_WARNINGS
#include "pipes.h"

char *FFMPEG_BIN = "ffmpeg";
char *FFPLAY_BIN = "ffplay";
char *FFPROBE_BIN = "ffprobe";

//char *FFMPEG_BIN = "C:\\Portable\\ffmpeg\\ffmpeg.exe";
//char *FFPLAY_BIN = "C:\\Portable\\ffmpeg\\ffplay.exe";
//char *FFPROBE_BIN = "C:\\Portable\\ffmpeg\\ffprobe.exe";

FILE *getVidIStream(char *filename) {
	char command[512];
	//Read yuv422p
	_snprintf_s((char*)&command, 512*sizeof(char), 512, "%s  -ss 0 -i %s -f image2pipe -pix_fmt yuv422p -vcodec rawvideo -loglevel quiet -", FFMPEG_BIN, filename);
	//Read bgr24
	//_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s  -ss 0 -i %s -f image2pipe -pix_fmt bgr24 -vcodec rawvideo -loglevel quiet -", FFMPEG_BIN, filename);
	printf("\nGetVidIStream: %s", command);
	return _popen(command, "rb");
}

FILE *getVidOStream(int width, int height) {
	char command[512];
	_snprintf_s((char*)&command, 512*sizeof(char), 512, "%s  -f rawvideo -pix_fmt rgb24 -s %ix%i -r 24 -an  -", FFPLAY_BIN, width, height);
	return _popen(command, "wb");
}
FILE *getVidOFStream(char* filename, int width, int height) {
	char command[512];
	//Write yuv422p
	_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -y -f rawvideo -pix_fmt yuv422p -r 24 -s %i:%i  -i - -s %i:%i -an -vcodec rawvideo -pix_fmt yuv422p -shortest  %s", FFMPEG_BIN, width, height, width, height, filename);
	//Write from bgr24
	//_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -y -f rawvideo -pix_fmt bgr24 -r 24 -s %i:%i  -i - -s %i:%i -an -vcodec rawvideo -pix_fmt yuv422p -shortest  %s", FFMPEG_BIN, width, height, width, height, filename);
	//Write 1 png picture
	//_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -f rawvideo -pix_fmt bgr24  -r 24 -s 240:135 -y -i - -s %i:%i  -frames:v 1  %s", FFMPEG_BIN, width, height, filename);
	printf("\nGetVidOFStream: %s", command);
	return _popen(command, "wb");
}

FILE *getTestPNG(char* filename, int width, int height) {
	char command[512];
	//Write yuv422p
	//_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -y -f rawvideo -pix_fmt yuv422p -r 24 -s %i:%i  -i - -s %i:%i -an -vcodec rawvideo -pix_fmt yuv422p -shortest  %s", FFMPEG_BIN, width, height, width, height, filename);
	//Write from bgr24
	//_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -y -f rawvideo -pix_fmt bgr24 -r 24 -s %i:%i  -i - -s %i:%i -an -vcodec rawvideo -pix_fmt yuv422p -shortest  %s", FFMPEG_BIN, width, height, width, height, filename);
	//Write 1 png picture
	_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -f rawvideo -pix_fmt gray -r 24 -s %d:%d -y -i - -s %i:%i  -frames:v 1  %s", FFMPEG_BIN, width, height, width, height, filename);
	printf("\nGetTestPNG: %s", command);
	return _popen(command, "wb");
}

FILE *getVidOFMPEG4(char* filename, int width, int height) {
	char command[512];
	//Write yuv422p
	_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -y -f rawvideo -pix_fmt gray -r 24 -s %i:%i  -i - -s %i:%i -an -vcodec rawvideo -pix_fmt gray -shortest  %s", FFMPEG_BIN, width, height, width, height, filename);
	printf("\nGetVidOFStream: %s", command);
	return _popen(command, "wb");
}

int getVideoContext(char* filename, int &width, int &height, int &num_of_frames, int &framerate, int &format) {
	//Function for getting necessary info about video file - width, height, duration in frames(number of frames), framerate)
	char command[512];
	int framerate_numerator = 0, framerate_denominator = 0;
	FILE * pPipe;
	_snprintf_s((char*)&command, 512 * sizeof(char), 512, "%s -v error -count_frames -select_streams v:0  -show_entries stream=avg_frame_rate,height,width,nb_read_frames -of default=nokey=1:noprint_wrappers=1 %s ", FFPROBE_BIN, filename);
	if ((pPipe = _popen(command, "rt")) == NULL)
		exit(1);
	/* while (fgets(text_buffer, 128, pPipe) != NULL)
	{
		attribute_count++;
		switch (attribute_count)
		{
		case 1:
			width = atoi(text_buffer);
			break;
		case 2:
			height = atoi(text_buffer);
			break;
		case 3:
			scanf(text_buffer, "%d/%d", framerate_numerator, framerate_denominator);
			if ((framerate_numerator != 0) && (framerate_denominator != 0))
			{
				framerate = ((double)framerate_numerator / (double)framerate_denominator);
				if (framerate < 24)
					printf("Framerate is unacceptable: %i/%i\n", framerate_numerator, framerate_denominator);
			}
			else  printf("Framerate is unacceptable: %i/%i\n", framerate_numerator, framerate_denominator);
			break;
		case 4:
			num_of_frames = atoi(text_buffer);
			break;
		}

	}*/
		
	fscanf(pPipe, "%d\n%d\n%d/%d\n%d", &width, &height, &framerate_numerator, &framerate_denominator, &num_of_frames);
	
	if ((framerate_numerator != 0) && (framerate_denominator != 0))
	{
		framerate = (framerate_numerator / framerate_denominator);
		/*if (framerate < 24)
			printf("Framerate is unacceptable: %i/%i\n", framerate_numerator, framerate_denominator);*/
	}
	else  printf("Framerate is unacceptable: %i/%i\n", framerate_numerator, framerate_denominator);

	format = 8;
	printf("Total frames = %i \n", num_of_frames);
	printf("Frame width = %i \n", width);
	printf("Frame height = %i \n", height);
	printf("Framerate = %i \n", framerate);
	_pclose(pPipe);
	return 0;
}

/*int main() {
	
	FILE* in = getVidIStream("C:\\Movies\\Test_2min_bgr24_s.avi");
	//FILE* out = getVidOStream(240, 135);
	//FILE* out = getVidOFStream("C:\\Movies\\my_output_%02d.png", 240, 135);
	FILE* out = getVidOFStream("C:\\Movies\\my_output.avi", 240, 135);

	int  total_frames = 0;
	int count_frames = 0;
	if (in == NULL || out == NULL) {
		printf("Couldn't open pipes: %i %i\n", in, out);
		exit(0);
	}

	total_frames=getVideoContext("C:\\Movies\\Test_2min_bgr24_s.avi");
	//printf( "Total frames = %i \n", total_frames);
	unsigned int stride = 240 * 135 * 3;
	u_char * readBuffer = (u_char*)malloc(stride);

	while (count_frames<total_frames) {
		fread(readBuffer, 1, stride, in);
		fwrite(readBuffer, 1, stride, out);
		Sleep(50);
		//printf(": %i %i\n", in, out);
		count_frames++;
	}

	_pclose(in);
	_pclose(out);
	system("pause");
}
*/