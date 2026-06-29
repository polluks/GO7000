void input_init();
char input_bus(char p1, char p2);
void input_set(int n, char state);
void input_set_bit(int n, int bit, int pressed);
void input_set_key(char row, char col, char pressed);
void input_set_restore();
extern char restore;
void input_char(char c, char pressed);
char input_read_p2(char p1, char p2);

#define KEY_0 0,0  /* row 0 */
#define KEY_1 0,1
#define KEY_2 0,2
#define KEY_3 0,3
#define KEY_4 0,4
#define KEY_5 0,5
#define KEY_6 0,6
#define KEY_7 0,7
#define KEY_8 1,0  /* row 1 */
#define KEY_9 1,1
#define KEY_YES 1,2
#define KEY_NO 1,3
#define KEY_SPACE 1,4
#define KEY_QUESTION 1,5
#define KEY_L 1,6
#define KEY_P 1,7
#define KEY_PLUS 2,0  /* row 2 */
#define KEY_W 2,1
#define KEY_E 2,2
#define KEY_R 2,3
#define KEY_T 2,4
#define KEY_U 2,5
#define KEY_I 2,6
#define KEY_O 2,7
#define KEY_Q 3,0  /* row 3 */
#define KEY_S 3,1
#define KEY_D 3,2
#define KEY_F 3,3
#define KEY_G 3,4
#define KEY_H 3,5
#define KEY_J 3,6
#define KEY_K 3,7
#define KEY_A 4,0  /* row 4 */
#define KEY_Z 4,1
#define KEY_X 4,2
#define KEY_C 4,3
#define KEY_V 4,4
#define KEY_B 4,5
#define KEY_M 4,6
#define KEY_PERIOD 4,7
#define KEY_MINUS 5,0  /* row 5 */
#define KEY_ASTERISK 5,1
#define KEY_SLASH 5,2
#define KEY_EQUALS 5,3
#define KEY_Y 5,4
#define KEY_N 5,5
#define KEY_DEL 5,6
#define KEY_RETURN 5,7
