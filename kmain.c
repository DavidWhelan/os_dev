
int column = 0;
int row = 0;
int letterPos = 0;
int cursorPos = 1;
char *fb = (char *) 0x00b8000;

/*writes char to specific cell. Frame buffer is at address 0x000B8000. there are 80 columns by 25 rows. Numbering starts at 0.
 *
 * Format follows this:
 * Bit:     | 15 14 13 12 11 10 9 8 | 7 6 5 4 | 3 2 1 0 |
 * Content: | ASCII                 | FG      | BG      |
 *
 * Colors are:
 * Color    Value   Color   Value   Color       Value   Color           Value
 * Black    0       Red         4   Dark grey   8       Light red       12
 * Blue     1       Magenta     5   Light blue  9       Light magenta   13
 * Green    2       Brown       6   Light green 10      Light brown     14
 * Cyan     3       Light grey  7   Light cyan  11      White           15
 * */
#define FB_WHITE  15
#define FB_BLACK   0
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[i] = c;
	fb[i+1] = ((fg & 0x0f) << 4) | (bg & 0x0f);
}

#define FB_COMMAND_PORT  0x3d4
#define FB_DATA_PORT     0x3d5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

#include "io.h"

void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT, ((pos >> 8) & 0x00ff));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, pos & 0x00ff);
}

int write(char *buf, unsigned int len)
{
	unsigned int i;
	for(i = 0; i<len; i++)
	{
		if(buf[i] == '\n')
		{
			letterPos = 80 + (letterPos - (letterPos % 80));
			continue;
		}
		if(letterPos >= 1999)
		{
			letterPos = 1920;
			char *fbm = (char *) 0x00b8000;
			char *fbe = (char *) 0x00b8000 + 160;
			for(int i = 0; i < 3840; i++)
			{
				fbm[i] = fbe[i];
			}
			letterPos = 1920;
		}
		fb_write_cell(letterPos * 2, buf[i], FB_BLACK, FB_WHITE);
		fb_move_cursor(cursorPos);
		letterPos++;
		//cursorPos++;
	}
	return 1;
}
void init()
{
	for(int i = 0; i<2000; i++)
	{
		fb_write_cell(i * 2, ' ', FB_BLACK, FB_BLACK);
	}
}

void kmain()
{
	init();
}
