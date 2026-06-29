void double_width();
void spr_init();
void spr_load(const char *vdc);
void vdc_init();
char vdc_read(char addr);
void vdc_write(char addr, char val);
void vdc_render(void);
void display_init(void);
void *sid_addr();
extern char vdc[256];
extern char sram[32];
extern char spr_y[4], spr_x[4], spr_attr[4];
extern char cset[512];

#define VIC_BLACK      0,0,0
#define VIC_WHITE      255,255,255
#define VIC_RED        136,0,0
#define VIC_CYAN       170,255,238
#define VIC_PURPLE     204,68,204
#define VIC_GREEN      0,204,85
#define VIC_BLUE       0,0,170
#define VIC_YELLOW     238,238,119
#define VIC_ORANGE     221,136,85
#define VIC_BROWN      102,68,0
#define VIC_PINK       255,119,119
#define VIC_DARKGREY   51,51,51
#define VIC_GREY       119,119,119
#define VIC_LTGREEN    170,255,102
#define VIC_LTBLUE     0,136,255
#define VIC_LTGREY     187,187,187
