/* #include "pipe_framebuffer.h"
#include "pipe_filter.h"



u_int cFilter::m_frame_size = 0, cFilter::m_reserved = 0, cFilter::m_pipesize = 0;
cFilter::cFilter(AVPixelFormat pix_fmt, u_int width, u_int height, u_int round, u_int pipesize)
: m_width(width)
, m_height(height)
, m_round(round)
, m_pix_fmt(pix_fmt)
, m_mark(false)
{
m_frame_size = av_image_get_buffer_size(pix_fmt, width, height, round);
m_linesize_luma = av_image_get_linesize(pix_fmt, width, 0);
m_reserved = m_frame_size + 4 * pipesize - (m_frame_size % pipesize);
m_pipesize = pipesize;
m_buffer = (u_char *)malloc(m_reserved);
}
cFilter::~cFilter()
{
free(m_buffer);
}
void cFilter::set_frame_size(u_int frame_size)
{
if (m_reserved < frame_size)
{
u_int reserved = frame_size + 4 * m_pipesize - (m_frame_size % m_pipesize);
fprintf(stderr, "expanding reserved space %08x => %08x\n", m_reserved, reserved);
void * new_buffer = malloc(reserved);
memcpy(new_buffer, m_buffer, m_reserved);
free(m_buffer);
m_buffer = (u_char*)new_buffer;
m_reserved = reserved;
}
m_frame_size = frame_size;
}

u_int read_frame_number = 0;

__inline u_int _read_from_pipe_(cFrameBuffer &framebuffer, cFilter &filter)
{
u_int framesize;
framesize = framebuffer.read_frame_from_pipe(filter.get_buffer());
if (!framesize)
{
fprintf(stderr, "=== Read error ===\n");
return 0;
}
if (framesize != filter.get_frame_size())
{
fprintf(stderr, "%08d: correcting frame size %08x => %08x\n", read_frame_number, filter.get_frame_size(), framesize);
filter.set_frame_size(framesize);
}
read_frame_number++;
return 1;
}

__inline u_int _write_to_pipe_(u_char* buffer)
{
u_int frame_size = cFilter::get_frame_size();
DWORD nBytesWrite = 0;
for (; frame_size > 0;)
{
if (WriteFile(stdOut, buffer, frame_size, &nBytesWrite, NULL))
{
frame_size -= nBytesWrite;
buffer += nBytesWrite;
}
else
{
fprintf(stderr, "=== Write error ===\n");
return 0;
}
}
return 1;
}


u_int width = 960;
u_int height = 540;
u_int round = 16;
//AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
AVPixelFormat pix_fmt = AV_PIX_FMT_YUV420P,
//u_char * message = NULL;
//int message_length = 0;
//int global_offset = -1;

// Check params
if (!(width && height))
{
	fprintf(stderr, "ERROR: Zero frame size\n");
	exit(-1);
}
// Get pipe buffer size and calculate frame size
#ifdef WINDOWS
g_PipeBufferSize = 0x8000;
#else
g_PipeBufferSize = fcntl(stdIn, F_GETPIPE_SZ);
#endif
cFilter filter(pix_fmt, width, height, round, g_PipeBufferSize);
cFrameBuffer framebuffer;
framebuffer.(pix_fmt, width, height, 4);
fprintf(stderr, "Frame size (align=%d): 0x%08x\n", round, cFilter::get_frame_size());

if (g_Operation == op_burn)
{
	burn_message(framebuffer, filter, message, message_length);
	return 0;
}
else if (g_Operation == op_read)
{
	read_message(framebuffer, filter);
	return 0;
}

return 0;
}
*/
