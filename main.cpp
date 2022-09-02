#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <vector>
#include <stack>
#include <cassert>
#define BLACK RGB(0, 0, 0)
#define RED RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define BLUE RGB(0, 0, 255)

class Cell {
public:
	int digit = 0;
	int x = 0;
	int y = 0;
	COLORREF color = BLACK;

	void init(int x_, int y_) {
		this->x = x_;
		this->y = y_;
		this->sx = x_ * SIZE;
		this->sy = y_ * SIZE;
	}

	void onPiant(HDC hdc) {
		if ((this->color == RED) || (this->color == GREEN) || (this->color == BLUE)) {
			HPEN pen = CreatePen(PS_SOLID, 3, color);
			SelectObject(hdc, pen);
			Rectangle(hdc, sx, sy, sx + SIZE, sy + SIZE);
			DeleteObject(pen);
		}
		HFONT hf = CreateFont(40, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		SelectObject(hdc, hf);

		switch (digit) {
		case 1:
			TextOut(hdc, sx + 20, sy + 10, L"1", 1);
			break;
		case 2:
			TextOut(hdc, sx + 20, sy + 10, L"2", 1);
			break;
		case 3:
			TextOut(hdc, sx + 20, sy + 10, L"3", 1);
			break;
		case 4:
			TextOut(hdc, sx + 20, sy + 10, L"4", 1);
			break;
		case 5:
			TextOut(hdc, sx + 20, sy + 10, L"5", 1);
			break;
		case 6:
			TextOut(hdc, sx + 20, sy + 10, L"6", 1);
			break;
		case 7:
			TextOut(hdc, sx + 20, sy + 10, L"7", 1);
			break;
		case 8:
			TextOut(hdc, sx + 20, sy + 10, L"8", 1);
			break;
		case 9:
			TextOut(hdc, sx + 20, sy + 10, L"9", 1);
			break;
		}
		DeleteObject(hf);
	}

private:
	const int SIZE = 60;
	int sx = 0;
	int sy = 0;
};

class Grid {
public:
	bool solving = false;

	Cell* getCell(int x, int y) {
		if (x < 0 || x >= 9 || y < 0 || y >= 9)
			return nullptr;

		return &cells[9 * y + x];
	}

	void create() {
		cells.resize(81);

		Cell* c;
		for (int y = 0; y < 9; y++) {
			for (int x = 0; x < 9; x++) {
				c = getCell(x, y);
				c->init(x, y);
			}
		}
		c = nullptr;
		free(c);
	}

	void linePaint(HDC hdc, HPEN pen, int interval, int number) {
		POINT pt;
		SelectObject(hdc, pen);
		for (int i = 0; i < (number + 1); i++) {
			MoveToEx(hdc, 0, interval * i, &pt);
			LineTo(hdc, 540, interval * i);
			MoveToEx(hdc, interval * i, 0, &pt);
			LineTo(hdc, interval * i, 540);
		}
	}

	void onPaint(HDC hdc) {
		HPEN pen = CreatePen(PS_SOLID, 2, BLACK);
		linePaint(hdc, pen, 60, 9);
		DeleteObject(pen);
		HPEN thickPen = CreatePen(PS_SOLID, 4, BLACK);
		linePaint(hdc, thickPen, 180, 3);
		DeleteObject(thickPen);


		for (Cell& c : cells) {
			c.onPiant(hdc);
		}
	}

	void onClick(int x, int y) {
		if (this->select) {
			this->select->color = BLACK;
			this->select = nullptr;
		}

		if (x < 9 && y < 9 && x >= 0 && y >= 0) {
			this->select = getCell(x, y);
			this->select->color = BLUE;
		}
	}

	void moveClick(int x_move, int y_move) {
		if (this->select) {
			int x = this->select->x + x_move;
			if (x > 8) {
				x = 8;
			}
			else if (x < 0) {
				x = 0;
			}
			int y = this->select->y + y_move;
			if (y > 8) {
				y = 8;
			}
			else if (y < 0) {
				y = 0;
			}
			onClick(x, y);
		}
		else {
			return;
		}
	}

	bool check(Cell* c, int digit) {
		int x = c->x;
		int y = c->y;
		for (int col = 0; col < 9; col++) {
			int checkDigit = getCell(col, y)->digit;
			if (checkDigit == digit && digit != 0 && col != x)
				return false;
		}

		for (int row = 0; row < 9; row++) {
			int checkDigit = getCell(x, row)->digit;
			if (checkDigit == digit && digit != 0 && row != y)
				return false;
		}

		int _x = x / 3;
		int _y = y / 3;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				int checkDigit = getCell(_x * 3 + i, _y * 3 + j)->digit;
				if (checkDigit == digit && digit != 0 && (_x * 3 + i) != x && (_y * 3 + j) != y)
					return false;
			}
		}

		return true;
	}

	void write(int digit_) {
		if (this->select) {
			if (check(this->select, digit_)) {
				this->select->digit = digit_;
			}
		}
	}

	void clear() {
		this->select = nullptr;
		for (Cell& c : cells) {
			c.digit = 0;
			c.color = BLACK;
		}
	}

	void clearColor() {
		this->select = nullptr;
		for (Cell& c : cells) {
			c.color = BLACK;
		}
	}

	Cell* EmptyCell() {
		for (Cell& c : cells) {
			if (c.digit == 0) {
				return &c;
			}
		}
		return nullptr;
	}

	bool quickSolve() {
		Cell* c = EmptyCell();
		if (!c) {
			clearColor();
			return true;
		}
		int _digit;
		for (_digit = 1; _digit < 10; _digit++) {
			if (check(c, _digit)) {
				c->digit = _digit;
				if (quickSolve()) {
					c = nullptr;
					free(c);
					return true;
				}
			}
		}
		c->digit = 0;
		c = nullptr;
		free(c);
		return false;
	}

	void emptyStack() {
		while (!solveCells.empty()) {
			solveCells.pop();
		}
	}

	void solve(HWND hWnd) {
		if (solveBack) {
			if (solveCells.empty()) {
				this->solveBack = false;
				KillTimer(hWnd, 0);
				MessageBox(hWnd, L"Unsolvable Sudoku!", L"Sudoku Solver", MB_OK | MB_ICONERROR);
				this->solving = false;
				clearColor();
				return;
			}
		}

		Cell* c = EmptyCell();
		if (!c) {
			this->solveBack = false;
			emptyStack();
			KillTimer(hWnd, 0);
			MessageBeep(MB_OK);
			MessageBox(hWnd, L"Sudoku solved!", L"Sudoku", MB_OK);
			this->solving = false;
			clearColor();
			c = nullptr;
			free(c);
			return;
		}

		if (!this->solveBack) {
			solveCells.push(c);
			c->color = GREEN;
		}
		else {
			c = solveCells.top();
			this->solveBack = false;
		}
		int _digit;
		for (_digit = ++c->digit; _digit < 10; _digit++) {
			if (check(c, _digit)) {
				c->digit = _digit;
				c = nullptr;
				free(c);
				return;
			}
		}

		c->color = RED;
		c->digit = 0;
		solveCells.pop();
		this->solveBack = true;
		c = nullptr;
		free(c);
	}

	void haltSolve(HWND hWnd) {
		this->solving = false;
		this->solveBack = false;
		clearColor();
		Cell* c;
		while (!solveCells.empty()) {
			c = solveCells.top();
			solveCells.pop();
			c->digit = 0;
		}
		KillTimer(hWnd, 0);
		c = nullptr;
		free(c);
	}

private:
	std::vector<Cell> cells;
	std::stack<Cell*> solveCells;
	bool solveBack = false;
	Cell* select = nullptr;
};

Grid grid;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);
		HDC dcbuffer = CreateCompatibleDC(hdc);
		HBITMAP bmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(dcbuffer, bmp);
		FillRect(dcbuffer, &rect, (HBRUSH)(COLOR_WINDOW + 1));
		grid.onPaint(dcbuffer);
		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, dcbuffer, 0, 0, SRCCOPY);
		DeleteObject(bmp);
		DeleteDC(dcbuffer);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_TIMER: {
		grid.solve(hWnd);
		InvalidateRect(hWnd, nullptr, false);
		break;
	}

	case WM_LBUTTONDOWN: {
		if (grid.solving == false) {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			x = x / 60;
			y = y / 60;
			grid.onClick(x, y);
			InvalidateRect(hWnd, nullptr, false);
		}
		break;
	}

	case WM_KEYDOWN: {
		switch (wParam) {
		case VK_LEFT: {
			grid.moveClick(-1, 0);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_UP: {
			grid.moveClick(0, -1);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_DOWN: {
			grid.moveClick(0, 1);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_RIGHT: {
			grid.moveClick(1, 0);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x30: {
			grid.write(0);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x31: {
			grid.write(1);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x32: {
			grid.write(2);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x33: {
			grid.write(3);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x34: {
			grid.write(4);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x35: {
			grid.write(5);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x36: {
			grid.write(6);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x37: {
			grid.write(7);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x38: {
			grid.write(8);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case 0x39: {
			grid.write(9);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_BACK: {
			grid.write(0);
			InvalidateRect(hWnd, nullptr, false);
			break;
		}
		case VK_DELETE: {
			int ret = MessageBox(hWnd, L"Do you want to clear the board?", L"Sudoku", MB_OKCANCEL | MB_ICONWARNING);
			if (ret == IDOK) {
				if (grid.solving) {
					grid.haltSolve(hWnd);
				}
				grid.clear();
				InvalidateRect(hWnd, nullptr, false);
			}
			break;
		}

		case VK_RETURN: {
			if (grid.solving) {
				grid.haltSolve(hWnd);
			}
			if (grid.quickSolve()) {
				MessageBeep(MB_OK);
				MessageBox(hWnd, L"Sudoku solved!", L"Sudoku", MB_OK);
			}
			else {
				grid.clearColor();
				MessageBox(hWnd, L"Unsolvable Sudoku!", L"Sudoku", MB_OK | MB_ICONERROR);
			}
			InvalidateRect(hWnd, nullptr, false);
			break;
		}

		case VK_SPACE: {
			if (!grid.solving) {
				grid.solving = 1;
				grid.clearColor();
				SetTimer(hWnd, 0, 50, nullptr);
			}
			break;
		}
		}
		break;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}

	default: {
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	}

	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	grid.create();

	WNDCLASSEX wcex;

	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = (HICON)(LoadIcon(nullptr, IDI_APPLICATION));
	wcex.hIconSm = (HICON)(LoadIcon(nullptr, IDI_APPLICATION));
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"Sudoku_Window";
	wcex.lpfnWndProc = WndProc;

	RegisterClassEx(&wcex);

	HWND hwnd = CreateWindowEx(0,
		L"Sudoku_Window",
		L"Sudoku Solver",
		WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION,
		CW_USEDEFAULT, CW_USEDEFAULT,
		556, 616,
		nullptr,
		nullptr,
		hInstance,
		0);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
