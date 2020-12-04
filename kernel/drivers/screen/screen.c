#include "screen.h"
#include "../../port_io.h"
int print(ScreenDriver*, char*);
int get_cursor();
void clear_screen();
void set_cursor(int);
int get_screen_offset(int, int);
int print_char(char, int, int, char);
int print_at (ScreenDriver* driver, char* string, int col, int row, char attribute);


void init_screen_driver(ScreenDriver* driver) {
	(*driver).print = &print;
	(*driver).clear_screen = &clear_screen;
}

int print(ScreenDriver* driver, char* string) {
	return print_at(driver, string, -1, -1, WHITE_ON_BLACK);
}

int print_at(ScreenDriver *driver, char* string, int col, int row, char attribute) {
	int out = 0, i = 0;
	if (col >= 0 && row >= 0) {
		set_cursor(get_screen_offset(col, row));
	}
	while (!string[i]) {
		out += print_char(string[i++], col, row, attribute);
	}
	return out;
}

int print_char(char character, int col, int row, char attribute) {
	unsigned char *vidmem = (unsigned char*)VIDEO_MEMORY;
	int offset;
	if (!attribute) {
		attribute = WHITE_ON_BLACK;
	}
	if (col >= 0 && row >= 0) {
		offset = get_screen_offset(col, row);
	}
	else {
		offset = get_cursor();
	}
	if (character == '\n') {
		offset = get_screen_offset(79, row);
	}
	else {
		vidmem[offset] = character;
		vidmem[offset + 1] = attribute;
	}
	offset += 2;
	//offset = handle_scrolling(offset);
//	set_cursor(offset);
	return 1;
}

void set_cursor(int offset) {
	port_byte_out(IO_PORT_SCREEN_REG_CTRL, CURSOR_LOC_LOW_REG);
	port_byte_out(IO_PORT_SCREEN_REG_DATA, (offset & 0xFF));
	port_byte_out(IO_PORT_SCREEN_REG_CTRL, CURSOR_LOC_HIGH_REG);
	port_byte_out(IO_PORT_SCREEN_REG_DATA, (offset >> 8) & 0xFF);
}

int get_screen_offset(int col, int row) {
	return 2*(MAX_COLS*row + col);
}

int get_cursor() {
	port_byte_out(IO_PORT_SCREEN_REG_CTRL, CURSOR_LOC_HIGH_REG);
	int cursor = port_byte_in(IO_PORT_SCREEN_REG_DATA) << 8;
	port_byte_out(IO_PORT_SCREEN_REG_CTRL, CURSOR_LOC_LOW_REG);
	cursor += port_byte_in(IO_PORT_SCREEN_REG_DATA);
	return cursor * 2;
}


void clear_screen() {
	int row = 0;
	int col = 0;
	for (; row < MAX_ROWS; row++) {
		for(col = 0; col < MAX_COLS; col++) {
			print_char(' ', col, row, WHITE_ON_BLACK);
		}
	}
	set_cursor(get_screen_offset(0,0));
}
