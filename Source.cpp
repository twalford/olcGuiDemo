#include "olcConsoleGameEngine.h"

class olcWindow
{
private:
	struct Node
	{
		short x = 0;
		short y = 0;
	};

public:
	olcWindow()
	{
	}

	~olcWindow() 
	{
	}

	short x = 3;
	short y = 3;
	short width = 30;
	short height = 20;
	short headerSize = 3;
	
	Node nodeIn;
	Node nodeOut;

	void repositionNodes()
	{
		nodeIn.x = this->x - 1;
		nodeIn.y = this->y + height / 2;
		nodeOut.x = this->x + width + 1;
		nodeOut.y = this->y + height / 2;
	}
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
			// Check if outside of screen
			if (s->x + s->width + panOffsetX < 0) // to the left
				DrawLine(0, max(s->y + panOffsetY, 0), 0, min(s->y + s->height + panOffsetY, scrHght_1), PIXEL_HALF, FG_GREEN);
			else if (s->x + panOffsetX > scrWdth_1) // to the right
				DrawLine(scrWdth_1, max(s->y + panOffsetY, 0), scrWdth_1, min(s->y + s->height + panOffsetY, scrHght_1), PIXEL_HALF, FG_GREEN);
			else if (s->y + s->height + panOffsetY < 0) // above
				DrawLine(s->x + panOffsetX, 0, s->x + s->width + panOffsetX, 0, PIXEL_HALF, FG_GREEN);
			else if (s->y + panOffsetY > scrHght_1) // below
				DrawLine(s->x + panOffsetX, scrHght_1, s->x + s->width + panOffsetX, scrHght_1, PIXEL_HALF, FG_GREEN);

			// Try to draw the whole window
			if (std::next(s) == ws.end())
				DrawWindow(*s, PIXEL_SOLID, FG_WHITE); 
			else
			{
				DrawWindow(*s, PIXEL_HALF, FG_GREY);
				DrawLink(*s, *std::next(s));
			}
		}
	}

	void DrawWindow(olcWindow &w, short glyph, short colour)
	{
		// Adjust window position for panning
		w.x += panOffsetX;
		w.y += panOffsetY;

		// Fill background of window
		Fill(w.x + 1, w.y + 1, w.x + w.width, w.y + w.height, PIXEL_SOLID, FG_BLACK);

		// Draw lines of window
		DrawLine(w.x, w.y, w.x, w.y + w.height, glyph, colour); //left
		DrawLine(w.x + w.width, w.y, w.x + w.width, w.y + w.height, glyph, colour); //right

		DrawLine(w.x + 1, w.y, w.x + w.width - 1, w.y, glyph, colour); //top
		DrawLine(w.x + 1, w.y + w.headerSize, w.x + w.width - 1, w.y + w.headerSize, glyph, colour); //header bottom
		DrawLine(w.x + 1, w.y + w.height, w.x + w.width - 1, w.y + w.height, glyph, colour); //bottom

		DrawNodes(w, PIXEL_SOLID, FG_RED, FG_YELLOW);
	}

	void DrawNodes(olcWindow &w, short glyph, short colourIn, short colourOut)
	{
		w.repositionNodes();
		// Node in (left)
		Draw(w.nodeIn.x, w.nodeIn.y - 1, glyph, colourIn);
		Draw(w.nodeIn.x - 1, w.nodeIn.y, glyph, colourIn);
		Draw(w.nodeIn.x, w.nodeIn.y + 1, glyph, colourIn);

		// Node out (right)
		Draw(w.nodeOut.x, w.nodeOut.y - 1, glyph, colourOut);
		Draw(w.nodeOut.x + 1, w.nodeOut.y, glyph, colourOut);
		Draw(w.nodeOut.x, w.nodeOut.y + 1, glyph, colourOut);
	}

	void DrawLink(olcWindow &w_a, olcWindow &w_b)
	{
		DrawLine(w_a.nodeOut.x,
			w_a.nodeOut.y,
			w_b.nodeIn.x + panOffsetX,
			w_b.nodeIn.y + panOffsetY,
			PIXEL_SOLID, FG_BLUE);
	}

public:
	

	std::vector<olcWindow> windows;
	olcWindow myFirstWindow;

	int scrWdth_1;
	int scrHght_1;

	short dragOffsetX = 0;
	short dragOffsetY = 0;
	short panOffsetX = 0;
	short panOffsetY = 0;

	const short maxPan = 1000;

	bool resizing = false;
	bool dragging = false;
	bool panning = false;

	bool OnUserCreate() override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		scrWdth_1 = ScreenWidth() - 1;
		scrHght_1 = ScreenHeight() - 1;

		windows.push_back(myFirstWindow);
		DrawWindows(windows);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (CheckMouseEvent())
		{
			Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
			DrawWindows(windows);
		}

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
		return  m_mousePosX - panOffsetX >= windows[i].x &&
				m_mousePosX - panOffsetX <= windows[i].x + windows[i].width &&
				m_mousePosY - panOffsetY >= windows[i].y &&
				m_mousePosY - panOffsetY <= windows[i].y + windows[i].height;
	}

	void CreateNewWindow()
	{
		olcWindow tempWnd;
		tempWnd.x = m_mousePosX - panOffsetX;
		tempWnd.y = m_mousePosY - panOffsetY;
		windows.push_back(tempWnd);

		windows.back().repositionNodes();
	}

	void ResizeActiveWindow()
	{
		if (m_mousePosY - panOffsetY - windows.back().y > windows.back().headerSize)
			windows.back().height = m_mousePosY - panOffsetY - windows.back().y;

		if (m_mousePosX - panOffsetX - windows.back().x > 10)
			windows.back().width = m_mousePosX - panOffsetX - windows.back().x;

		windows.back().repositionNodes();
	}

	void SetPositionActiveWindow()
	{
		windows.back().x = m_mousePosX - dragOffsetX;
		windows.back().y = m_mousePosY - dragOffsetY;

		windows.back().repositionNodes();
	}

	bool CheckMouseEvent()
	{
		bool foundWindow = false;
		bool deleteWindow = false;

		// Check right mouse release
		if (m_mouse[0].bReleased)
		{
			resizing = false;
			dragging = false;
			panning = false;
		}

		//// Dragging ////
		if (dragging)
		{
			SetPositionActiveWindow();
			return true;
		}

		//// Resizing ////
		if (resizing)
		{
			ResizeActiveWindow();
			return true;
		}

		//// Panning ////
		if (panning)
		{
			panOffsetX = max(min(m_mousePosX - dragOffsetX, maxPan),-maxPan);
			panOffsetY = max(min(m_mousePosY - dragOffsetY, maxPan),-maxPan);
			return true;
		}

		//// Right Mouse Button ////
		if (m_mouse[1].bPressed)
		{
			// Create new Window if there is none.
			if (windows.size() == 0)
			{
				CreateNewWindow();
				return true;
			}
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
						return true;
					}
					// Create a window if one isn't clicked.
					else if (i == 0)
					{
						CreateNewWindow();
						return true;
					}
				}
			}
		}

		//// Left mouse Button ////
		if (m_mouse[0].bPressed)
		{
			// Loop through the windows.
			for (int i = windows.size() - 1; i >= 0; i--)
			{
				// Check if clicked bottom right pixel.
				if (m_mousePosX - panOffsetX == windows[i].x + windows[i].width && m_mousePosY - panOffsetY == windows[i].y + windows[i].height)
				{
					// RESIZE
					SetWindowActive(i);
					resizing = true;
					return true;
				}

				// Check if click in header.
				if (m_mousePosX - panOffsetX >= windows[i].x &&
					m_mousePosX - panOffsetX <= windows[i].x + windows[i].width &&
					m_mousePosY - panOffsetY >= windows[i].y &&
					m_mousePosY - panOffsetY <= windows[i].y + windows[i].headerSize)
				{
					// DRAG
					dragOffsetX = m_mousePosX - windows[i].x;
					dragOffsetY = m_mousePosY - windows[i].y;
					SetWindowActive(i);
					dragging = true;
					return true;
				}

				// Check if clicked in any part of window.
				if (IsWithinWindow(i))
				{
					SetWindowActive(i);
					return true;
				}
			}

			// Panning
			dragOffsetX = m_mousePosX - panOffsetX;
			dragOffsetY = m_mousePosY - panOffsetY;
			panning = true;
			return true;
		}
		return false;
	}
};

int main()
{
	olcSynthGui demo;
	if (demo.ConstructConsole(200, 100, 6, 6))
		demo.Start();

	return 0;
}