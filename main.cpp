#include "Fonter2.h"

#include <Theme/Theme.h>


int ViewPortWidth = 0; 

void App::LocalTextFillStrokeGradient(Painter& sw)
{
Font   fnt;
int TotalWidth = 0;
          
	// Get String from control	
	String stxt= ctrl2.TextSelection.Get();
	WString txt= stxt.ToWString();
	
	// Select font from list
	fnt.Face(( list2.GetCursor()) +1 );
	
	// Select font attributes from controls
	fnt.Height(~ctrl2.FontSize);
	fnt.Bold(ctrl2.FontBold);
	fnt.Italic(ctrl2.FontItalic);
	
	Size tsz = GetTextSize(txt, fnt);
	
    Color FillColorBegin = ctrl2.Color1.GetData();
    Color FillColorEnd   = ctrl2.Color2.GetData();
	  
	Color StrokeColorBegin = ctrl2.Color3.GetData();
    Color StrokeColorEnd = ctrl2.Color4.GetData();

    sw.Translate(10,10);

    // char per char
    for(int i=0;i<txt.GetLength();i++)
    {
        // Extract one char
        WString txt2 = txt.Mid(i,1);
        
        // and its size
        Size tsz2 = GetTextSize(txt2, fnt);
        tsz2.cx   = tsz2.cx   + (ctrl2.StrokeSize) + 2;
               
        // Handle line feed        
        if( (((wchar)txt2[0]) == 0x0d) )
        {
            sw.Translate(-TotalWidth,~ctrl2.FontSize);      
            TotalWidth = 0;
        }
        else
        {
            if( ((wchar)txt2[0]) > 0x20)
            {
        	  // Shadow
        	  if(ctrl2.FontShadow)
        	  	 sw.Text(1, 1, txt2, fnt)
        	  	 .Fill(0, 0, Color(0,0,0),  tsz2.cx, tsz2.cy, FillColorEnd)
        	  	 .Stroke(ctrl2.StrokeSize, 0, 0, StrokeColorBegin, tsz2.cx, 0, StrokeColorEnd);	  
        	  
        	  // Character
        	  sw.Text(0, 0, txt2, fnt)
                .Fill(0, 0, FillColorBegin, 0 + tsz2.cx, 0+ tsz2.cy, FillColorEnd) 
        	    .Stroke(ctrl2.StrokeSize, 0, 0, StrokeColorBegin, 0 + tsz2.cx, 0, StrokeColorEnd);	  
        	
        	 
              // Spacing to next character
        	  sw.Translate(tsz2.cx,0);
        	  
        	  // Update total width for return to new line
        	  TotalWidth += tsz2.cx;
        
              // for wrapping get Size of next char	  
              WString txt3 = txt.Mid(i+1,1);
        
              // and its size
              Size tsz3 = GetTextSize(txt3, fnt);
              tsz3.cx  += 2*ctrl2.StrokeSize;
                
        	  if (TotalWidth + tsz3.cx > ViewPortWidth)
                {
                sw.Translate(-TotalWidth,~ctrl2.FontSize);      
                TotalWidth = 0;
                }           
            }
        }
    }
}

void App::DoPaint0(Painter& sw)
{
//	sw.Translate(~ctrl.translate_x, ~ctrl.translate_y);
	sw.Rotate(~ctrl.rotate);
	//sw.Scale(~ctrl.scale, (double)~ctrl.scale * (double)~ctrl.scale_x);
//	sw.Scale(~ctrl2.scale, (double)~ctrl2.scale * (double)~ctrl2.scale);
	sw.Opacity(~ctrl.opacity);
	sw.LineCap(~ctrl.linecap);
	sw.LineJoin(~ctrl.linejoin);
	if(ctrl.transparent)
		sw.Clear(RGBAZero());
	else
		sw.Clear(White());
	
	LocalTextFillStrokeGradient(sw);
	
}

void App::DoPaint(Painter& sw)
{
	if(ctrl.painting) {
		PaintingPainter h(2000, 2000);
		DoPaint0(h);
		sw.Paint(h);
	}
	else
		DoPaint0(sw);
}

void App::Print()
{
	PaintingPainter sw(1000, 1000);
	DoPaint(sw);
	PrinterJob pb;
	if(pb.Execute())
		pb.GetDraw().DrawPainting(0, 0, 4000, 4000, sw);
}

void App::Benchmark()
{
	int time;
	int time0 = GetTickCount();
	int n = 0;
	for(;;) {
		time = GetTickCount();
		if(time - time0 > 1000) break;
		ImageBuffer ib(800, 600);
		BufferPainter sw(ib, ctrl.quality);
		DoPaint(sw);
		n++;
	}
	PromptOK("Benchmark: " + AsString(double(time - time0) / n) + " ms");
}

void App::Paint(Draw& w)
{
	Size sz = GetSize();
	
	ViewPortWidth = sz.cx; // HACK
	
	if(ctrl.transparent) {
		for(int y = 0; y < sz.cy; y += 32)
			for(int x = 0; x < sz.cx; x += 32)
				w.DrawRect(x, y, 32, 32, (x ^ y) & 32 ? Color(254, 172, 120) : Color(124, 135, 253));
	}
	ImageBuffer ib(sz);
	BufferPainter sw(ib, ctrl.quality);
	DoPaint(sw);
	w.DrawImage(0, 0, ib);
}

void App::Sync()
{
	
	Refresh();
}

void App::ToSlider(EditDouble *e, SliderCtrl *slider)
{
	double v = ~*e;
	*slider <<= fround(1000.0 * (v - e->GetMin()) / (e->GetMax() - e->GetMin()));
	Refresh();
}

void App::ToEdit(EditDouble *e, SliderCtrl *slider)
{
	int x = ~*slider;
	*e <<= x * (e->GetMax() - e->GetMin()) / 1000.0 + e->GetMin();
	Refresh();
}

void App::Pair(EditDouble& e, SliderCtrl& slider)
{
	e <<= THISBACK2(ToSlider, &e, &slider);
	slider <<= THISBACK2(ToEdit, &e, &slider);
	slider.MinMax(0, 1000);
}

void App::ToSlider()
{
	ToSlider(&ctrl.rotate, &ctrl.rotate_slider);
	ToSlider(&ctrl.scale, &ctrl.scale_slider);
	ToSlider(&ctrl2.scale, &ctrl2.scale_slider);
	ToSlider(&ctrl2.FontSize, &ctrl2.fontsize_slider);
	ToSlider(&ctrl2.StrokeSize, &ctrl2.strokesize_slider);	
	ToSlider(&ctrl.scale_x, &ctrl.scale_x_slider);
	ToSlider(&ctrl.translate_x, &ctrl.translate_x_slider);
	ToSlider(&ctrl.translate_y, &ctrl.translate_y_slider);
	ToSlider(&ctrl.opacity, &ctrl.opacity_slider);
}

void App::Reset()
{
	ctrl.rotate <<= ctrl.translate_x <<= ctrl.translate_y <<= 0;
	ctrl.scale <<= ctrl.scale_x <<= ctrl.opacity <<= 1.0;
	ctrl2.scale <<= ctrl.opacity <<= 1.0;
	
	ctrl2.FontSize  <<= 128.0;
	ctrl2.Color1.SetData(Color(128,255,0));
	ctrl2.Color2.SetData(Color(0,255,0));
	ctrl2.Color3.SetData(Color(0,0,0));
	ctrl2.Color4.SetData(Color(0,0,0));
	
	ctrl2.StrokeSize  <<= 1.0;
	
	//  NEHE character set.
	ctrl2.TextSelection.SetData("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!`?'.,;:()[]{}<>|/@\\^$-%+=#_&~*");
	
	// short text
//	ctrl2.TextSelection.SetData("ABCDEFGHIJK");
	ctrl.painting = false;
	ctrl.quality = MODE_ANTIALIASED;
	ctrl.linejoin <<= LINEJOIN_MITER;
	ctrl.linecap <<= LINECAP_BUTT;
	ToSlider();
}

void App::Serialize(Stream& s)
{
	s
		% ctrl.rotate % ctrl.rotate_slider
		% ctrl.scale % ctrl.scale_slider
		% ctrl.scale_x % ctrl.scale_x_slider
		% ctrl.translate_x % ctrl.translate_x_slider
		% ctrl.translate_y % ctrl.translate_y_slider
		% ctrl.opacity % ctrl.opacity_slider
		% ctrl.painting % ctrl.quality % ctrl.transparent
	;
}

App::App() {
	AddFrame(split);
	split.Left(list, HorzLayoutZoom(220));
	
	Sizeable().Zoomable();
	
	ctrl.Height(ctrl.GetLayoutSize().cy);
	CtrlLayout(ctrl);	
	list.InsertFrame(0, ctrl);
	Pair(ctrl.rotate, ctrl.rotate_slider);
	Pair(ctrl.scale, ctrl.scale_slider);
	Pair(ctrl.scale_x, ctrl.scale_x_slider);
	Pair(ctrl.translate_x, ctrl.translate_x_slider);
	Pair(ctrl.translate_y, ctrl.translate_y_slider);
	Pair(ctrl.opacity, ctrl.opacity_slider);
	ctrl.linecap.Add(LINECAP_BUTT, "Butt caps");
	ctrl.linecap.Add(LINECAP_SQUARE, "Square caps");
	ctrl.linecap.Add(LINECAP_ROUND, "Round caps");
	ctrl.linejoin.Add(LINEJOIN_MITER, "Miter joins");
	ctrl.linejoin.Add(LINEJOIN_ROUND, "Round joins");
	ctrl.linejoin.Add(LINEJOIN_BEVEL, "Bevel joins");
	ctrl.linecap   <<= ctrl.linejoin <<= ctrl.painting <<= ctrl.quality <<= ctrl.transparent <<= THISBACK(Sync);
	ctrl.reset     <<= THISBACK(Reset);
	ctrl.benchmark <<= THISBACK(Benchmark);
	ctrl.print     <<= THISBACK(Print);
	
	AddFrame(split2);
	split2.Right(list2,HorzLayoutZoom(220));
	
	list2.WhenSel = THISBACK(Sync);
	list2.AddColumn();
	list2.NoHeader();
	
	Font fnt;
	for(int i = 1; i < fnt.GetFaceCount(); i++)
		list2.Add(fnt.GetFaceName(i));
	

	Sync();
	Sizeable().Zoomable();
	ctrl2.Height(ctrl2.GetLayoutSize().cy);
	CtrlLayout(ctrl2);	
	list2.InsertFrame(1, ctrl2);
	Pair(ctrl2.scale, ctrl2.scale_slider);
	Pair(ctrl2.FontSize, ctrl2.fontsize_slider);
	Pair(ctrl2.StrokeSize, ctrl2.strokesize_slider);
	
	ctrl2.FontShadow <<= ctrl2.FontBold <<= ctrl2.FontItalic <<= THISBACK(Sync);
	ctrl2.Color1     <<= ctrl2.Color2   <<= THISBACK(Sync);
	ctrl2.Color3     <<= ctrl2.Color4   <<= THISBACK(Sync);
	ctrl2.StrokeSize <<= THISBACK(Sync);
	ctrl2.TextSelection <<= THISBACK(Sync);
	
	Reset();
	LoadFromFile(*this);
	Title("Fonter 2.0");
}


App::~App()
{
//	if(list.IsCursor())
//		SaveFile(ConfigFile("last"), list.GetKey());
//	StoreToFile(*this);
}

GUI_APP_MAIN
{
	//Ctrl::ShowRepaint(50); 
	Ctrl::SetSkin(ChStdSkin);
	
	Theme FTSkin;
	FTSkin.Load("Skulpture.zip");
	FTSkin.Apply();
	
    	
	App().Run();
}
