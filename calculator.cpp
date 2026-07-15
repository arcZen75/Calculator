#include <windows.h>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <stdexcept>


using namespace std;

// =====================================================
//  EXPRESSION PARSER (recursive descent)
// =====================================================
string expr;
int pos;

double parseExpression();
double parseFactor();
double parseTerm();

void skip() {
    while (pos < expr.size() && expr[pos] == ' ') pos++;
}

double parseNumber() {
    skip();
    double number = 0;
    bool decimal = false;
    double frac = 0.1;

    if (expr[pos] == '-') {
        pos++;
        return -parseNumber();
    }

    while (pos < expr.size() && (isdigit(expr[pos]) || expr[pos] == '.')) {
        if (expr[pos] == '.') {
            decimal = true;
            pos++;
            continue;
        }
        if (!decimal) number = number * 10 + (expr[pos] - '0');
        else {
            number += frac * (expr[pos] - '0');
            frac /= 10;
        }
        pos++;
    }
    return number;
}

double parseFunc() {
    skip();

    if (isalpha(expr[pos])) {
        string func = "";
        while (pos < expr.size() && isalpha(expr[pos])) func += expr[pos++];

        skip();
        if (expr[pos] == '(') pos++;

        double val = parseExpression();
        pos++;

        if (func == "sin") return sin(val);
        if (func == "cos") return cos(val);
        if (func == "tan") return tan(val);
        if (func == "log") return log10(val);
        if (func == "ln")  return log(val);
        if (func == "sqrt") return sqrt(val);

        throw std::runtime_error("Unknown function");

    }
    return parseNumber();
}

double parseFactor() {
    skip();

    if (expr[pos] == '(') {
        pos++;
        double val = parseExpression();
        pos++;
        return val;
    }
    return parseFunc();
}

double parsePower() {
    double base = parseFactor();
    skip();

    while (expr[pos] == '^') {
        pos++;
        double exp = parseFactor();
        base = pow(base, exp);
    }
    return base;
}

double parseTerm() {
    double val = parsePower();
    skip();

    while (expr[pos] == '*' || expr[pos] == '/') {
        char op = expr[pos++];
        double next = parsePower();

        if (op == '*') val *= next;
        else val /= next;

        skip();
    }
    return val;
}

double parseExpression() {
    double val = parseTerm();
    skip();

    while (expr[pos] == '+' || expr[pos] == '-') {
        char op = expr[pos++];
        double next = parseTerm();
        if (op == '+') val += next;
        else val -= next;

        skip();
    }
    return val;
}

double eval(string input) {
    expr = input;
    pos = 0;
    return parseExpression();
}

// =====================================================
//  GLOBALS
// =====================================================
HWND hInput, hResult;
vector<string> historyList;

// Colors (Nether Theme)
COLORREF BACKGROUND = RGB(15, 0, 0);
COLORREF BUTTON_RED = RGB(176, 0, 0);
COLORREF TEXT_COLOR = RGB(255, 255, 255);

// =====================================================
//  BUTTON IDS
// =====================================================
#define BTN_0 100
#define BTN_1 101
#define BTN_2 102
#define BTN_3 103
#define BTN_4 104
#define BTN_5 105
#define BTN_6 106
#define BTN_7 107
#define BTN_8 108
#define BTN_9 109

#define BTN_ADD 110
#define BTN_SUB 111
#define BTN_MUL 112
#define BTN_DIV 113
#define BTN_LP 114
#define BTN_RP 115
#define BTN_POW 116

#define BTN_SIN 120
#define BTN_COS 121
#define BTN_TAN 122
#define BTN_SQRT 123
#define BTN_LOG 124
#define BTN_LN 125

#define BTN_CLEAR 130
#define BTN_EQ 131
#define BTN_HISTORY 132

// =====================================================
//  HISTORY WINDOW
// =====================================================
LRESULT CALLBACK HistoryProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
    case WM_CREATE:
        for (auto &h : historyList)
            CreateWindow("STATIC", h.c_str(), WS_VISIBLE | WS_CHILD,
                10, 10 + (&h - &historyList[0]) * 20, 450, 20, hwnd, NULL, NULL, NULL);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

// =====================================================
//  MAIN WINDOW PROCEDURE
// =====================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)w;
        SetTextColor(hdc, TEXT_COLOR);
        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)CreateSolidBrush(BACKGROUND);
    }

    case WM_CREATE:
    {
        hInput = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            20, 20, 350, 25,
            hwnd, NULL, NULL, NULL);

        hResult = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
            20, 50, 350, 25,
            hwnd, NULL, NULL, NULL);

        // BUTTON CREATOR
        auto make = [&](int id, const char* text, int x, int y) {
            CreateWindow("BUTTON", text,
                WS_VISIBLE | WS_CHILD,
                x, y, 60, 35,
                hwnd, (HMENU)id, NULL, NULL);
            };

        int x = 20, y = 90;

        make(BTN_7, "7", x, y);
        make(BTN_8, "8", x+70, y);
        make(BTN_9, "9", x+140, y);
        make(BTN_DIV, "/", x+210, y);

        y += 40;
        make(BTN_4, "4", x, y);
        make(BTN_5, "5", x+70, y);
        make(BTN_6, "6", x+140, y);
        make(BTN_MUL, "*", x+210, y);

        y += 40;
        make(BTN_1, "1", x, y);
        make(BTN_2, "2", x+70, y);
        make(BTN_3, "3", x+140, y);
        make(BTN_SUB, "-", x+210, y);

        y += 40;
        make(BTN_0, "0", x, y);
        make(BTN_LP, "(", x+70, y);
        make(BTN_RP, ")", x+140, y);
        make(BTN_ADD, "+", x+210, y);

        y += 40;
        make(BTN_POW, "^", x, y);
        make(BTN_SIN, "sin", x+70, y);
        make(BTN_COS, "cos", x+140, y);
        make(BTN_TAN, "tan", x+210, y);

        y += 40;
        make(BTN_SQRT, "sqrt", x, y);
        make(BTN_LOG, "log", x+70, y);
        make(BTN_LN, "ln", x+140, y);
        make(BTN_HISTORY, "Hist", x+210, y);

        y += 40;
        make(BTN_CLEAR, "C", x, y);
        make(BTN_EQ, "=", x+70, y);

        break;
    }

    case WM_COMMAND:
    {
        char buffer[512];
        GetWindowText(hInput, buffer, 512);
        string cur = buffer;

        int id = LOWORD(w);

        auto add = [&](string s) {
            SetWindowText(hInput, (cur + s).c_str());
            };

        if (id >= BTN_0 && id <= BTN_9) add(to_string(id - BTN_0));
        else if (id == BTN_ADD) add("+");
        else if (id == BTN_SUB) add("-");
        else if (id == BTN_MUL) add("*");
        else if (id == BTN_DIV) add("/");
        else if (id == BTN_LP) add("(");
        else if (id == BTN_RP) add(")");
        else if (id == BTN_POW) add("^");
        else if (id == BTN_SIN) add("sin(");
        else if (id == BTN_COS) add("cos(");
        else if (id == BTN_TAN) add("tan(");
        else if (id == BTN_SQRT) add("sqrt(");
        else if (id == BTN_LOG) add("log(");
        else if (id == BTN_LN) add("ln(");

        else if (id == BTN_CLEAR) {
            SetWindowText(hInput, "");
            SetWindowText(hResult, "");
        }

        else if (id == BTN_EQ) {
            try {
                double r = eval(cur);
                string out = to_string(r);
                SetWindowText(hResult, out.c_str());

                historyList.push_back(cur + " = " + out);
            }
            catch (...) {
                SetWindowText(hResult, "Error");
            }
        }

        else if (id == BTN_HISTORY) {
            WNDCLASS wc = { 0 };
            wc.lpfnWndProc = HistoryProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = "HistWin";
            RegisterClass(&wc);

            CreateWindow("HistWin", "History",
                WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                400, 100, 500, 500,
                hwnd, NULL, NULL, NULL);
        }

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, w, l);
}

// =====================================================
//  WINMAIN
// =====================================================
int WINAPI WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int n) {
    WNDCLASS wc = { 0 };
    wc.hbrBackground = CreateSolidBrush(BACKGROUND);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = h;
    wc.lpszClassName = "CalcWin";
    wc.lpfnWndProc = WndProc;

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("CalcWin", "Minecraft Nether Calculator",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        100, 100, 420, 600,
        NULL, NULL, h, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
