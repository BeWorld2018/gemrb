/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 */

#ifndef TEXTAREA_H
#define TEXTAREA_H

#include "GUI/Control.h"
#include "GUI/ScrollView.h"
#include "GUI/TextSystem/Font.h"
#include "GUI/TextSystem/GemMarkup.h"
#include "GUI/TextSystem/TextContainer.h"

#include "RGBAColor.h"
#include "exports.h"

#include <vector>

namespace GemRB {

// TextArea flags, keep these in sync too
// the control type is intentionally left out
#define IE_GUI_TEXTAREA_AUTOSCROLL   1
#define IE_GUI_TEXTAREA_HISTORY      2	// message window
#define IE_GUI_TEXTAREA_EDITABLE     4

typedef std::pair<int, String> SelectOption;

/**
 * @class TextArea
 * Widget capable of displaying long paragraphs of text.
 * It is usually scrolled with a ScrollBar widget
 */

class GEM_EXPORT TextArea : public Control {
private:
	/** Draws the Control on the Output Display */
	void DrawSelf(Region drawFrame, const Region& clip);
	void SizeChanged(const Size& /*oldSize*/);
	
	class SpanSelector : public TextContainer {
		struct OptSpan : public TextContainer {
			OptSpan(const Region& frame, Font* font, Holder<Palette> pal)
			: TextContainer(frame, font, pal) {}
			
			// forward OnMouseLeave to superview (SpanSelector) as a mouse over
			void OnMouseLeave(const MouseEvent& me, const DragOp*) {
				assert(superView);
				superView->OnMouseOver(me);
			}
		};
	private:
		TextArea& ta;
		TextContainer* hoverSpan, *selectedSpan;
		size_t size;
		
	private:
		void ClearHover();
		TextContainer* TextAtPoint(const Point&);
		TextContainer* TextAtIndex(size_t idx);
		
	public:
		SpanSelector(TextArea& ta, const std::vector<const String*>&, bool numbered);
		
		size_t NumOpts() const { return size;};
		void MakeSelection(size_t idx);
		TextContainer* Selection() const { return selectedSpan; }
		
		bool CanLockFocus() const { return false; }
		
		void OnMouseOver(const MouseEvent& /*me*/);
		void OnMouseUp(const MouseEvent& /*me*/, unsigned short Mod);
		void OnMouseLeave(const MouseEvent& /*me*/, const DragOp*);
	};

public:
	TextArea(const Region& frame, Font* text);
	TextArea(const Region& frame, Font* text, Font* caps,
			 Color hitextcolor, Color initcolor, Color lowtextcolor);

	bool IsOpaque() const { return false; }

	/** Sets the Actual Text */
	void SetText(const String& text);
	/** Clears the textarea */
	void ClearText();

	/** Appends a String to the current Text */
	void AppendText(const String& text);
	/** Inserts a String into the current Text at pos */
	// int InsertText(const char* text, int pos);

	/** Per Pixel scrolling */
	void ScrollToY(int y, short lineduration);

	ieDword LineCount() const;
	ieWord LineHeight() const;

	void SetSelectOptions(const std::vector<SelectOption>&, bool numbered,
						  const Color* color, const Color* hiColor, const Color* selColor);
	/** Set Selectable */
	void SetSelectable(bool val);
	void SetAnimPicture(Sprite2D* Picture);

	/** Returns the selected text */
	String QueryText() const;
	/** Marks textarea for redraw with a new value */
	void UpdateState(unsigned int optIdx);

private: // Private attributes
	// dialog and listbox handling
	std::vector<ieDword> values;
	const Content* dialogBeginNode;
	// dialog options container
	SpanSelector* selectOptions;
	// standard text display container
	TextContainer* textContainer;
	ScrollView scrollview;

	/** Fonts */
	Font* finit, * ftext;
	GemMarkupParser parser;

	enum PALETTE_TYPE {
		PALETTE_NORMAL = 0,	// standard text color
		PALETTE_OPTIONS,	// normal palette for selectable options (dialog/listbox)
		PALETTE_HOVER,		// palette for hovering options (dialog/listbox)
		PALETTE_SELECTED,	// selected list box/dialog option.
		PALETTE_INITIALS,	// palette for finit. used only is some cases.

		PALETTE_TYPE_COUNT
	};
	Holder<Palette> palettes[PALETTE_TYPE_COUNT];

private: //internal functions
	void Init();
	void SetPalette(const Color*, PALETTE_TYPE);

	void UpdateRowCount(int h);
	void UpdateScrollview();

	int TextHeight() const;
	int OptionsHeight() const;
	int ContentHeight() const;

public: //Events
	struct Action {
		// !!! Keep these synchronized with GUIDefines.py !!!
		static const Control::Action Change = Control::ValueChange; // text change event (keyboard, etc)
		static const Control::Action Select = ACTION_CUSTOM(0); // selection event such as dialog or a listbox
	};

	/** Key Press Event */
	bool OnKeyPress(const KeyboardEvent& Key, unsigned short Mod);
	/** Mousewheel scroll */
	bool OnMouseWheelScroll(const Point& delta);

	void SetFocus();

	void ClearSelectOptions();
};

}

#endif
