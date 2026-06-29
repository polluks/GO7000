void double_width();
void spr_init();
void spr_load(const char *vdc);
void vdc_init();
char vdc_read(char addr);
void vdc_write(char addr, char val);
extern char vdc[256];
extern char sram[32];
extern char spr_y[4], spr_x[4], spr_attr[4];
