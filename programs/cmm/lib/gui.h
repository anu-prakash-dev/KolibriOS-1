	#ifndef INCLUDE_GUI_H
#define INCLUDE_GUI_H
#print "[include <gui.h>]\n"

#ifndef INCLUDE_KOLIBRI_H
#include "../lib/kolibri.h"
#endif

#ifndef INCLUDE_STRING_H
#include "../lib/strings.h"
#endif

#ifndef INCLUDE_RGB_H
#include "../lib/patterns/rgb.h"
#endif

:void DrawRectangle(dword x,y,w,h,color1)
{
	if (w<=0) || (h<=0) return;
	DrawBar(x,y,w,1,color1);
	DrawBar(x,y+h,w,1,color1);
	DrawBar(x,y,1,h,color1);
	DrawBar(x+w,y,1,h+1,color1);
}

:void DrawWideRectangle(dword x,y,w,h,boder,color1)
{
	if (w<=0) || (h<=0) return;
	DrawBar(x, y, w, boder, color1);
	DrawBar(x, y+h-boder, w, boder, color1);
	DrawBar(x, y+boder, boder, h-boder-boder, color1);
	DrawBar(x+w-boder, y+boder, boder, h-boder-boder, color1);
}

:void DrawRectangle3D(dword x,y,w,h,color1,color2)
{
	if (w<=0) || (h<=0) return;
	DrawBar(x,y,w+1,1,color1);
	DrawBar(x,y+1,1,h-1,color1);
	DrawBar(x+w,y+1,1,h,color2);
	DrawBar(x,y+h,w,1,color2);
}

:void DrawCaptButton(dword x,y,w,h,id,color_b, color_t,text)
{
	dword tx = -strlen(text)*8+w/2+x;
	dword ty = h/2-7+y;

	if (id>0) DefineButton(x,y,w,h,id,color_b);
	WriteText(tx+1,ty+1,0x90,MixColors(color_b,0,230),text);
	WriteText(tx,ty,0x90,color_t,text);
}

:void WriteTextCenter(dword x,y,w,color_t,text)
{
	WriteText(-strlen(text)*6+w/2+x+1,y,0x80,color_t,text);
}

:void DrawCircle(int x, y, r, color)
{
	int i;
	float px=0, py=r, ii = r * 3.1415926 * 2;
	FOR (i = 0; i < ii; i++)
	{
        PutPixel(px + x, y - py, color);
        px = py / r + px;
        py = -px / r + py;
	}
}

:unsigned char checkbox_flag[507] = {
	0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 
	0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xD4, 
	0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xDC, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 
	0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 
	0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xDC, 
	0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xBC, 0x04, 
	0xFC, 0xBC, 0x04, 0xFC, 0xBC, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 
	0xB4, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xC8, 
	0x04, 0xFC, 0xBC, 0x04, 0xFC, 0xBC, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xB4, 0x04, 0xF9, 0x98, 0x04, 
	0xFC, 0xF2, 0xD2, 0xFC, 0xF2, 0xD2, 0xFA, 0xA5, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 
	0xB4, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xB4, 0x04, 0xFC, 0xB4, 
	0x04, 0xF9, 0x98, 0x04, 0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xF6, 0xFC, 0xF9, 0xAF, 0xF9, 0x98, 0x04, 
	0xFC, 0xB4, 0x04, 0xFC, 0xD4, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xF2, 0xD2, 0xFC, 0xF2, 0xD2, 0xF9, 
	0x98, 0x04, 0xFC, 0xB4, 0x04, 0xF9, 0x98, 0x04, 0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xF6, 0xF7, 0xE0, 
	0x95, 0xBC, 0x64, 0x04, 0xF9, 0x98, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xD4, 0x04, 0xF9, 0x98, 0x04, 
	0xFD, 0xFD, 0xF6, 0xFF, 0xFF, 0xFF, 0xFC, 0xDC, 0x2C, 0xF9, 0x98, 0x04, 0xFF, 0xFF, 0xFF, 0xFD, 
	0xFD, 0xF6, 0xF7, 0xE0, 0x95, 0xBC, 0x64, 0x04, 0xF9, 0x98, 0x04, 0xFA, 0x9F, 0x04, 0xFC, 0xA9, 
	0x04, 0xFC, 0xD4, 0x04, 0xF9, 0x98, 0x04, 0xF7, 0xE0, 0x95, 0xFD, 0xFD, 0xF6, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xF6, 0xF7, 0xE0, 0x95, 0xBC, 0x64, 0x04, 0xF9, 0x98, 0x04, 0xFA, 
	0x9F, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xD4, 0x04, 0xF9, 0x98, 0x04, 0xBC, 0x64, 
	0x04, 0xF7, 0xE0, 0x95, 0xFD, 0xFD, 0xF6, 0xFD, 0xFD, 0xF6, 0xF7, 0xE0, 0x95, 0xBC, 0x64, 0x04, 
	0xF9, 0x98, 0x04, 0xFA, 0x9F, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 
	0xC8, 0x04, 0xFA, 0xA5, 0x04, 0xF9, 0x98, 0x04, 0xBC, 0x64, 0x04, 0xF7, 0xE0, 0x95, 0xF7, 0xE0, 
	0x95, 0xBC, 0x64, 0x04, 0xF9, 0x98, 0x04, 0xFA, 0x9F, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 
	0xFC, 0xA9, 0x04, 0xFA, 0x9F, 0x04, 0xFC, 0xC8, 0x04, 0xFC, 0xA9, 0x04, 0xFA, 0x9F, 0x04, 0xF9, 
	0x98, 0x04, 0xBC, 0x64, 0x04, 0xBC, 0x64, 0x04, 0xF9, 0x98, 0x04, 0xFA, 0x9F, 0x04, 0xFC, 0xA9, 
	0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFA, 0x9F, 0x04, 0xFA, 0x9F, 0x04, 0xFC, 0xC8, 0x04, 
	0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFA, 0x9F, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xFA, 
	0x9F, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFC, 0xA9, 0x04, 0xFA, 0x9F, 0x04, 0xFA, 0x9F, 
	0x04, 0xF9, 0x98, 0x04, 0xFC, 0xB4, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 
	0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 
	0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04, 0xF9, 0x98, 0x04
};

:void CheckBox(dword x,y,bt_id, text, is_checked)
{
	byte w=14, h=14;
	DefineButton(x-1, y-1, strlen(text)*8 + w + 17, h+2, bt_id+BT_HIDE+BT_NOFRAME, 0);
	EDI = system.color.work;
	WriteText(x+w+8, h / 2 + y -7, 0xD0, system.color.work_text, text);
	DrawRectangle(x, y, w, h, system.color.work_graph);
	if (is_checked == 0)
	{
		DrawRectangle3D(x+1, y+1, w-2, h-2, 0xDDDddd, 0xffffff);
		DrawBar(x+2, y+2, w-3, h-3, 0xffffff);
	} 
	else if (is_checked == 1)
	{
		DrawWideRectangle(x+1, y+1, w-1, h-1, 2, 0xffffff);
		_PutImage(x+1, y+1, 13, 13, #checkbox_flag);
	}
	else if (is_checked == 2) //not active
	{
		DrawWideRectangle(x+1, y+1, w-1, h-1, 2, 0xffffff);
		DrawBar(x+3, y+3, w-5, h-5, 0x888888);
	}
	DrawRectangle3D(x-1,y-1,w+2,h+2,system.color.work_dark,system.color.work_light);
}

:void MoreLessBox(dword x,y, bt_id_more, bt_id_less, value, text)
{
	#define VALUE_FIELD_W 34
	#define SIZE 18
	dword value_text = itoa(value);

	DrawRectangle(x, y, VALUE_FIELD_W, SIZE, system.color.work_graph);
	DrawRectangle3D(x+1, y+1, VALUE_FIELD_W-2, SIZE-2, 0xDDDddd, 0xffffff);
	DrawBar(x+2, y+2, VALUE_FIELD_W-3, SIZE-3, 0xffffff);
	WriteText( -strlen(value_text)+3*8 + x+6, SIZE / 2 + y -6, 0x90, system.color.work_text, value_text);

	DrawCaptButton(VALUE_FIELD_W + x,     y, SIZE, SIZE, bt_id_more, system.color.work_button, system.color.work_button_text, "+");
	DrawCaptButton(VALUE_FIELD_W + x + SIZE, y, SIZE, SIZE, bt_id_less, system.color.work_button, system.color.work_button_text, "-");
	EDI = system.color.work;
	WriteText(x+VALUE_FIELD_W+SIZE+SIZE+10, SIZE / 2 + y -7, 0xD0, system.color.work_text, text);
	DrawRectangle3D(x-1,y-1,VALUE_FIELD_W+SIZE+SIZE+2,SIZE+2,system.color.work_dark,system.color.work_light);
}

:void DrawEditBox(dword edit_box_pointer)
{
	dword x,y,w,h,bg;
	ESI = edit_box_pointer;
	x = ESI.edit_box.left;
	y = ESI.edit_box.top;
	w = ESI.edit_box.width+1;
	if (ESI.edit_box.flags & 100000000000b) bg = 0xCACACA; else bg = 0xFFFfff;
	h = 15;
	DrawRectangle(x-1, y-1, w+2, h+2, bg);
	DrawRectangle3D(x-2, y-2, w+2, h+2, 0xDDDddd, bg);
	DrawRectangle(x-3, y-3, w+6, h+6, system.color.work_graph);
	DrawRectangle3D(x-4, y-4, w+8, h+8, system.color.work_dark, system.color.work_light);
	edit_box_draw  stdcall (edit_box_pointer);
}

:void DrawProgressBar(dword st_x, st_y, st_w, st_h, col_fon, col_border, col_fill, col_text, progress_percent)
{
	int progress_w;
	static int fill_old;
	    
	//if (progress_percent<=0) {DrawBar(st_x,st_y, st_x + st_w + fill_old + 15,st_h+1, col_fon); fill_old=0; return;}
	if (progress_percent<=0) || (progress_percent>=100) return;
	
	DrawRectangle(st_x, st_y, st_w,st_h, col_border);
	DrawRectangle3D(st_x+1, st_y+1, st_w-2,st_h-2, 0xFFFfff, 0xFFFfff);

	if (progress_percent>0) && (progress_percent<=100)
	{
		progress_w = st_w - 3 * progress_percent / 100;
		DrawBar(st_x+2, st_y+2, progress_w, st_h-3, col_fill);
		DrawBar(st_x+2+progress_w, st_y+2, st_w-progress_w-3, st_h-3, 0xFFFfff);
	}
}

:void DrawLink(dword x,y,font_type,btn_id, inscription)
{
	int w;
	WriteText(x,y,font_type,0x4E00E7,inscription);
	if (font_type==0x80) w = strlen(inscription)*6; else w = strlen(inscription)*7;
	DefineButton(x-1,y-1,w,10,btn_id+BT_HIDE,0);
	DrawBar(x,y+8,w,1,0x4E00E7);
}

:void PutShadow(dword x,y,w,h,skinned,strength)
{
	proc_info wForm;
	dword shadow_buf, skin_height;
	shadow_buf = mem_Alloc(w*h*3);
 	GetProcessInfo(#wForm, SelfInfo);
	CopyScreen(shadow_buf, 5*skinned+x+wForm.left, GetSkinHeight()*skinned+y+wForm.top, w, h);
	ShadowImage(shadow_buf, w, h, strength);
	_PutImage(x,y,w,h,shadow_buf);
	mem_Free(shadow_buf);
}

:void DrawPopup(dword x,y,w,h,skinned, col_work,col_border)
{
	DrawRectangle(x,y,w,h,col_border);
	DrawBar(x+1,y+1,w-1,1,0xFFFfff);
	DrawBar(x+1,y+2,1,h-2,0xFFFfff);
	if (col_work!=-1) DrawBar(x+2,y+2,w-2,h-2,col_work);
	DrawPopupShadow(x,y,w,h-1,skinned);
}

:void DrawPopupShadow(dword x,y,w,h,skinned)
{
	PutShadow(w+x+1,y,1,h+2,skinned,2);
	PutShadow(w+x+2,y+1,1,h+2,skinned,1);
	PutShadow(x,y+h+2,w+2,1,skinned,2);
	PutShadow(x+1,y+h+3,w+1,1,skinned,1);
}

:void GrayScaleImage(dword color_image, w, h)
{
	dword i,gray,to,rr,gg,bb;
	to = w*h*3 + color_image;
	for (i = color_image; i < to; i+=3)
	{
		rr = DSBYTE[i];
		gg = DSBYTE[i+1];
		bb = DSBYTE[i+2];
		gray = rr*rr;
		gray += gg*gg;
		gray += bb*bb;
		gray = sqrt(gray) / 3;
		DSBYTE[i] = DSBYTE[i+1] = DSBYTE[i+2] = gray;
	}
}

:void ShadowImage(dword color_image, w, h, strength)
{
	dword col, to;
	strength = 10 - strength;
	to = w*h*3 + color_image;
	for ( ; color_image < to; color_image++)
	{
		col = strength * DSBYTE[color_image] / 10;
		DSBYTE[color_image] = col;
	}
}

:void WriteTextLines(dword x,y,byte fontType, dword color, text_pointer, line_h)
{
	dword next_word_pointer = strchr(text_pointer, '\n');
	if (next_word_pointer) WriteTextLines(dword x, y+line_h, byte fontType, dword color, next_word_pointer+2, line_h);
	ESBYTE[next_word_pointer] = NULL;
	WriteText(dword x, y, byte fontType, dword color, text_pointer);
	ESBYTE[next_word_pointer] = '\n';
}

//this function increase falue and return it
//useful for list of controls which goes one after one
struct incn
{
	dword n;
	dword inc(dword _addition);
};

dword incn::inc(dword _addition)
{
	n+=_addition;
	return n;
}


/*=========================================================
==
==                   TABS
==
/========================================================*/

#define TAB_PADDING 25
#define TAB_HEIGHT 25

:struct _tabs
{
	int active_tab;
	void draw();
	int click();
} tabs;

:void _tabs::draw(dword x,y, but_id, text)
{
	dword col_bg, col_text;
	dword w=strlen(text)*8+TAB_PADDING, h=TAB_HEIGHT;
	y -= h;

	if (but_id==active_tab)
	{
		col_bg=system.color.work_button;
		col_text=system.color.work_button_text;
	}
	else
	{
		col_bg=system.color.work;
		col_text=system.color.work_text;
	} 
	DrawCaptButton(x,y, w-1,h+1, but_id, col_bg, col_text, text);
}

:int _tabs::click(int N)
{
	if (N==active_tab) return false;
	active_tab = N;
	return true;
}



#endif