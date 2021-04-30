#pragma once

#include <CtrlLib/CtrlLib.h>
#include <Painter/Painter.h>

using namespace Upp;


#define LAYOUTFILE <Fonter2/Fonter2.lay>
#include <CtrlCore/lay.h>



struct App : TopWindow {
	virtual void Paint(Draw& w);

	SplitterFrame  split;
	SplitterFrame  split2;
	ArrayCtrl      list;
	ArrayCtrl      list2;
	FrameBottom< WithCtrlLayout<StaticRect> > ctrl;
	FrameBottom< WithCtrl2Layout<StaticRect> > ctrl2;
	
	typedef App CLASSNAME;

	void DoPaint0(Painter& sw);
	void DoPaint(Painter& sw);
	void Print();
	void Benchmark();
	void Sync();
	void ToSlider(EditDouble *e, SliderCtrl *slider);
	void ToEdit(EditDouble *e, SliderCtrl *slider);
	void Pair(EditDouble& e, SliderCtrl& slider);
	void ToSlider();
	void Reset();
	void Serialize(Stream& s);
	void LocalTextFillStrokeGradient(Painter& sw);

	App();
	~App();
};



