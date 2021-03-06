/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CHECKHEADER_SLIB_UI_EDIT_VIEW
#define CHECKHEADER_SLIB_UI_EDIT_VIEW

#include "definition.h"

#include "view.h"

namespace slib
{
	
	class SLIB_EXPORT EditView : public View
	{
		SLIB_DECLARE_OBJECT
		
	public:
		EditView();

		~EditView();
		
	public:
		String getText();
		
		virtual void setText(const String& text, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		Alignment getGravity();
		
		virtual void setGravity(Alignment align, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		String getHintText();
		
		virtual void setHintText(const String& str, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		sl_bool isReadOnly();
		
		virtual void setReadOnly(sl_bool flag, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		virtual sl_bool isMultiLine();
		
		virtual void setMultiLine(sl_bool flag, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		Color getTextColor();
		
		virtual void setTextColor(const Color& color, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		Color getHintTextColor();
		
		virtual void setHintTextColor(const Color& color, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		UIReturnKeyType getReturnKeyType();
		
		void setReturnKeyType(UIReturnKeyType type);
		
		UIKeyboardType getKeyboardType();
		
		void setKeyboardType(UIKeyboardType type);
		
		void setAutoCapitalizationType(UIAutoCapitalizationType type);
		
		UIAutoCapitalizationType getAutoCaptializationType();
		
		sl_bool isAutoDismissKeyboard();
		
		void setAutoDismissKeyboard(sl_bool flag);
		
	protected:
		void onMeasureLayout(sl_bool flagHorizontal, sl_bool flagVertical, const UIRect& currentFrame) override;
		
		void onDraw(Canvas* canvas) override;

		void onClick(UIEvent* ev) override;
		
	public:
		SLIB_PROPERTY(AtomicFunction<String(EditView*, String)>, OnChange)
		
		SLIB_PROPERTY(AtomicFunction<void(EditView*)>, OnReturnKey)
		
		SLIB_PROPERTY(AtomicFunction<void(EditView*)>, OnDoneEdit)
		
	protected:
		virtual String onChange(const String& newValue);
		
		virtual void onReturnKey();
		
		virtual void onDoneEdit();
		
	public:
		Ref<ViewInstance> createNativeWidget(ViewInstance* parent) override;
		
		void dispatchKeyEvent(UIEvent* ev) override;
		
		virtual String dispatchChange(const String& newValue);
		
		virtual void dispatchReturnKey();
		
		virtual void dispatchDoneEdit();
		
	private:
		void _getText_NW();
		
		void _setText_NW(const String& text);
		
		void _setTextAlignment_NW(Alignment align);
		
		void _setHintText_NW(const String& str);
		
		void _setReadOnly_NW(sl_bool flag);
		
		void _setMultiLine_NW(sl_bool flag);
		
		void _setTextColor_NW(const Color& color);
		
		void _setHintTextColor_NW(const Color& color);
		
		void _setReturnKeyType_NW(UIReturnKeyType type);
		
		void _setKeyboardType_NW(UIKeyboardType type);
		
		void _setAutoCapitalizationType_NW(UIAutoCapitalizationType type);
		
		void _setFont_NW(const Ref<Font>& font) override;
		
		void _setBorder_NW(sl_bool flag) override;
		
		void _setBackgroundColor_NW(const Color& color) override;
		
		String _onChangeEditViewNative(EditView* ev, const String& text);
		
		void _onReturnKeyEditViewNative(EditView* ev);
		
		void _onDoneEditViewNative(EditView* ev);

		void _onDoneEditViewNativeButton(View* view);

		void _onCloseWindowEditViewNative(Window* window, UIEvent* ev);

	protected:
		AtomicString m_text;
		Alignment m_textAlignment;
		AtomicString m_hintText;
		sl_bool m_flagReadOnly;
		sl_bool m_flagMultiLine;
		Color m_textColor;
		Color m_hintTextColor;
		UIReturnKeyType m_returnKeyType;
		UIKeyboardType m_keyboardType;
		UIAutoCapitalizationType m_autoCapitalizationType;
		sl_bool m_flagAutoDismissKeyboard;
		
		Ref<Window> m_windowEdit;
		Ref<EditView> m_editViewNative;
		
	};
	
	class PasswordView : public EditView
	{
		SLIB_DECLARE_OBJECT
		
	public:
		PasswordView();
		
		~PasswordView();

	public:
		sl_bool isMultiLine() override;
		
		virtual void setMultiLine(sl_bool flag, UIUpdateMode mode = UIUpdateMode::Redraw) override;
		
	public:
		Ref<ViewInstance> createNativeWidget(ViewInstance* parent) override;
		
	protected:
		void onDraw(Canvas* canvas) override;
		
	};
	
	class TextArea : public EditView
	{
		SLIB_DECLARE_OBJECT
		
	public:
		TextArea();
		
		~TextArea();

	public:
		sl_bool isMultiLine() override;
		
		void setMultiLine(sl_bool flag, UIUpdateMode mode = UIUpdateMode::Redraw) override;
		
	public:
		Ref<ViewInstance> createNativeWidget(ViewInstance* parent) override;
		
	};

}

#endif
