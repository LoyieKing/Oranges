#include "header\font.h"
#include "header\video.h"
#include "header\type.h"
#include "header\keyboard.h"
#include "header\string.h"

#define CURSOR '\x1b'

#define MAX_PER_LINE (SCREEN_WIDTH / FONT_WIDTH)
#define MAX_LINE (SCREEN_HEIGHT / FONT_HEIGHT)

int text_x = 0;
int text_y = 0;

int text_len = 0;
char text_buffer[MAX_PER_LINE * MAX_LINE];
bool screen_fill[MAX_PER_LINE][MAX_LINE] = {0};

bool CapsLockPressed = false;
bool LShiftPressed = false;
bool RShiftPressed = false;

#define ShiftPressed (LShiftPressed | RShiftPressed)
#define Uppercase (CapsLockPressed ^ ShiftPressed)

#define CaseUpper(low, upper) \
    case low:                 \
        InputChar(upper);     \
        break;

void SetChar(char c, int x, int y);
void CleanChar(int x, int y);
void InputChar(char c);
void InputEnter();
void Backspace();
void UpdateStateUI();
bool EmptyBlock(int x, int y);

void RunMain()
{
    SetChar(CURSOR, 0, 0);
    while (true)
    {
        UpdateStateUI();
        CopyToVBE(videoBuffer);
        uint16 key = WaitForKey();
        if (key == 0)
        {
            break;
        }
        char *keyname = KeyCodes[key].KeyName;
        if (strcmp(keyname, "CapsLock") == 0 && KeyCodes[key].Pressed)
        {
            CapsLockPressed = ~CapsLockPressed;
            continue;
        }
        if (strcmp(keyname, "LeftShift") == 0)
        {
            LShiftPressed = KeyCodes[key].Pressed;
            continue;
        }
        if (strcmp(keyname, "RightShift") == 0)
        {
            RShiftPressed = KeyCodes[key].Pressed;
            continue;
        }
        if (KeyCodes[key].Pressed)
        {
            if (strlen(keyname) == 1)
            {
                char c = keyname[0];
                if (c >= 'A' && c <= 'Z')
                {
                    if (Uppercase)
                    {
                        InputChar(c);
                        continue;
                    }
                    else
                    {
                        InputChar(c + 0x20);
                        continue;
                    }
                }
                if (!ShiftPressed)
                {
                    InputChar(c);
                    continue;
                }

                switch (c)
                {
                    CaseUpper('`', '~');
                    CaseUpper('1', '!');
                    CaseUpper('2', '@');
                    CaseUpper('3', '#');
                    CaseUpper('4', '$');
                    CaseUpper('5', '%');
                    CaseUpper('6', '^');
                    CaseUpper('7', '&');
                    CaseUpper('8', '*');
                    CaseUpper('9', '(');
                    CaseUpper('0', ')');
                    CaseUpper('-', '_');
                    CaseUpper('=', '+');
                    CaseUpper('[', '{');
                    CaseUpper(']', '}');
                    CaseUpper('\\', '|');
                    CaseUpper(';', ':');
                    CaseUpper('\'', '"');
                    CaseUpper(',', '<');
                    CaseUpper('.', '>');
                    CaseUpper('/', '?');
                default:
                    InputChar(c);
                }

                continue;
            }
            else if (keyname[1] == '(') //1(keypad)
            {
                InputChar(keyname[0]);
            }
            else if ((strcmp(keyname, "Enter") == 0 || strcmp(keyname, "Enter(KeyPad)") == 0))
            {
                InputEnter();
            }
            else if (strcmp(keyname, "Space") == 0)
            {
                InputChar(' ');
            }
            else if (strcmp(keyname, "Backspace") == 0)
            {
                Backspace();
            }
        }
    }
}

void SetChar(char c, int x, int y)
{
    if (screen_fill[x][y])
    {
        CleanChar(x, y);
    }
    DrawChar(c, FONT_WIDTH * x, FONT_HEIGHT * y, videoBuffer, COLOR_WHITE);
    screen_fill[x][y] = true;
}
void CleanChar(int x, int y)
{
    screen_fill[x][y] = false;
    DrawBlock(FONT_WIDTH * x, FONT_HEIGHT * y, FONT_WIDTH, FONT_HEIGHT, videoBuffer, COLOR_BLACK);
}

void InputChar(char c)
{
    text_buffer[text_len++] = c;
    SetChar(c, text_x, text_y);
    text_x++;
    if (text_x >= MAX_PER_LINE)
    {
        text_x = 0;
        text_y++;
    }
    SetChar(CURSOR, text_x, text_y);
}

void InputEnter()
{
    CleanChar(text_x, text_y);
    text_buffer[text_len++] = '\n';
    text_x = 0;
    text_y++;
    SetChar(CURSOR, text_x, text_y);
}

void Backspace()
{
    text_len--;
    if (text_x == 0)
    {
        if (text_y == 0)
        {
            text_len = 0;
            return;
        }
        if (screen_fill[MAX_PER_LINE - 1][text_y - 1])
        {
            if (text_buffer[text_len] == '\n')
                return;
            CleanChar(text_x, text_y);
            text_y--;
            text_x = MAX_PER_LINE - 1;
            SetChar(CURSOR, text_x, text_y);
            return;
        }
        CleanChar(text_x, text_y);
        text_y--;
        int i;
        for (i = MAX_PER_LINE - 1; i > 0; i--)
        {
            if (screen_fill[i - 1][text_y])
                break;
        }
        text_x = i;
        SetChar(CURSOR, text_x, text_y);
        return;
    }
    CleanChar(text_x, text_y);
    text_x--;
    SetChar(CURSOR, text_x, text_y);
}

void UpdateStateUI()
{
    DrawBlock(0, SCREEN_HEIGHT - FONT_HEIGHT, SCREEN_WIDTH, FONT_HEIGHT, videoBuffer, COLOR_BLUE);
    DrawString(Uppercase ? "CAPS" : "caps", FONT_WIDTH, SCREEN_HEIGHT - FONT_HEIGHT, videoBuffer, COLOR_WHITE);
    DrawChar(ShiftPressed ? '\x18' : '\x19', FONT_WIDTH * 6, SCREEN_HEIGHT - FONT_HEIGHT, videoBuffer, COLOR_WHITE);
    DrawNumber(text_len, SCREEN_WIDTH - FONT_WIDTH * 6, SCREEN_HEIGHT - FONT_HEIGHT, videoBuffer, COLOR_WHITE);
}

bool EmptyBlock(int x, int y)
{
    for (int i = 0; i < FONT_HEIGHT; i++)
    {
        for (int j = 0; j > FONT_WIDTH; j++)
        {
            if (videoBuffer[SCREEN_WIDTH * (y + i) + (x + j)] != COLOR_BLACK)
                return false;
        }
    }
    return true;
}

#undef CaseUpper