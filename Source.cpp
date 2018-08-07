#include "olcConsoleGameEngine.h"

class olcWindow
{
public:
	olcWindow()
	{
	}

	short x = 3;
	short y = 3;
	short width = 30;
	short height = 20;
	short headerSize = 3;
};

class olcSynthGui : public olcConsoleGameEngine
{
public:
	olcSynthGui()
	{
		m_sAppName = L"OLC Gui Demo";
	}

private:
	void DrawWindows(std::vector<olcWindow> ws)
	{
		for (auto s = ws.begin(); s != ws.end(); s++) 
		{
			if (std::next(s) == ws.end())
				DrawWindow(*s, PIXEL_SOLID, FG_WHITE); 
			else
				DrawWindow(*s, PIXEL_HALF, FG_GREY);
		}
	}

	void DrawWindow(olcWindow &w, short glyph, short colour)
	{
		// Fill background of window
		Fill(w.x + 1, w.y + 1, w.x + w.width, w.y + w.height, PIXEL_SOLID, FG_BLACK);

		// Draw lines of window
		DrawLine(w.x, w.y, w.x + w.width, w.y, glyph, colour); //top
		DrawLine(w.x, w.y + w.headerSize, w.x + w.width, w.y + w.headerSize, glyph, colour); //header bottom
		DrawLine(w.x, w.y + w.height, w.x + w.width, w.y + w.height, glyph, colour); //bottom

		DrawLine(w.x, w.y, w.x, w.y + w.height, glyph, colour); //left
		DrawLine(w.x + w.width, w.y, w.x + w.width, w.y + w.height, glyph, colour); //right
	}

public:
	std::vector<olcWindow> windows;
	olcWindow myFirstWindow;

	short offsetX = 0;
	short offsetY = 0;
	bool resizing = false;
	bool dragging = false;

	bool OnUserCreate() override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		windows.push_back(myFirstWindow);
		DrawWindows(windows);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		CheckMouseEvent();
		DrawWindows(windows);

		return true;
	}

	//Rotates a window to back of vector
	void SetWindowActive(int i)
	{
		std::vector<olcWindow>::iterator iter = windows.begin();
		for (int c = 0; c < i; c++)
			iter++;
		std::rotate(iter, iter + 1, windows.end());
	}

	bool IsWithinWindow(int i)
	{
		return  m_mousePosX >= windows[i].x &&
				m_mousePosX <= windows[i].x + windows[i].width &&
				m_mousePosY >= windows[i].y &&
				m_mousePosY <= windows[i].y + windows[i].height;
	}

	void CreateNewWindow()
	{
		olcWindow tempWnd;
		tempWnd.x = m_mousePosX;
		tempWnd.y = m_mousePosY;
		windows.push_back(tempWnd);
	}

	void ResizeActiveWindow()
	{
		if (m_mousePosY - windows.back().y > windows.back().headerSize)
			windows.back().height = m_mousePosY - windows.back().y;

		if (m_mousePosX - windows.back().x > 10)
			windows.back().width = m_mousePosX - windows.back().x;
	}

	void SetPositionActiveWindow()
	{
		windows.back().x = m_mousePosX - offsetX;
		windows.back().y = m_mousePosY - offsetY;
	}

	void CheckMouseEvent()
	{
		bool foundWindow = false;
		bool deleteWindow = false;

		// Check right mouse release
		if (m_mouse[0].bReleased)
		{
			resizing = false;
			dragging = false;
		}

		//// Dragging ////
		if (dragging)
			SetPositionActiveWindow();

		//// Resizing ////
		if (resizing)
		{
			ResizeActiveWindow();
			return;
		}

		//// Right Mouse Button ////
		if (m_mouse[1].bPressed)
		{
			// Create new Window if there is none.
			if (windows.size() == 0)
				CreateNewWindow();
			// Otherwise loop through the windows.
			else
			{
				for (int i = windows.size() - 1; i >= 0; i--)
				{
					// Delete a window if one is clicked.
					if (IsWithinWindow(i))
					{
						SetWindowActive(i);
						windows.pop_back();
						return;
					}
					// Create a window if one isn't clicked.
					else if (i == 0)
						CreateNewWindow();
				}
			}
		}

		//// Left mouse Button ////
		if (m_mouse[0].bPressed)
		{
			// Return if no windows.
			if (windows.size() <= 0)
				return;

			// Otherwise loop through the windows.
			for (int i = windows.size() - 1; i >= 0; i--)
			{
				// Check if clicked bottom right pixel.
				if (m_mousePosX == windows[i].x + windows[i].width && m_mousePosY == windows[i].y + windows[i].height)
				{
					// RESIZE
					SetWindowActive(i);
					resizing = true;
					return;
				}

				// Check if click in header.
				if (m_mousePosX >= windows[i].x &&
					m_mousePosX <= windows[i].x + windows[i].width &&
					m_mousePosY >= windows[i].y &&
					m_mousePosY <= windows[i].y + windows[i].headerSize)
				{
					// DRAG
					offsetX = m_mousePosX - windows[i].x;
					offsetY = m_mousePosY - windows[i].y;
					SetWindowActive(i);
					dragging = true;
					return;
				}

				// Check if clicked in any part of window.
				if (IsWithinWindow(i))
				{
					SetWindowActive(i);
					return;
				}
			}
		}
	}
};

int main()
{
	olcSynthGui demo;
	if (demo.ConstructConsole(200, 100, 6, 6))
		demo.Start();

	return 0;
}