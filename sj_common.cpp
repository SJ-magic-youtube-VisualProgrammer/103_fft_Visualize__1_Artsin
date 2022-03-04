/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

const float _PI = 3.1415;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = nullptr;


/************************************************************
func
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double sj_max(double a, double b)
{
	if(a < b)	return b;
	else		return a;
}

/******************************
******************************/
bool checkIf_ContentsExist(char* ret, char* buf)
{
	if( (ret == NULL) || (buf == NULL)) return false;
	
	string str_Line = buf;
	Align_StringOfData(str_Line);
	vector<string> str_vals = ofSplitString(str_Line, ",");
	if( (str_vals.size() == 0) || (str_vals[0] == "") ){ // no_data or exist text but it's",,,,,,,".
		return false;
	}else{
		return true;
	}
}

/******************************
******************************/
void Align_StringOfData(string& s)
{
	size_t pos;
	while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
******************************/
void print_separatoin()
{
	printf("---------------------------------\n");
}

/******************************
******************************/
void ClearFbo(ofFbo& fbo)
{
	fbo.begin();
		ofClear(0, 0, 0, 255);
	fbo.end();
}

/******************************
******************************/
float toRad(float val){
	return val * 3.1415 / 180.0;
}

/******************************
******************************/
float toDeg(float val){
	return val * 180.0 / 3.1415;
}

/******************************
******************************/
float get_val_top_of_artsin_window(){
	if(Gui_Global->ArtSin2D_BarHeight == 0) return 1e4;
	
	const float Window_H = 200.0;
	return 1.0 * Window_H / Gui_Global->ArtSin2D_BarHeight;
}

/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	Group_Audio.setup("Audio");
		Group_Audio.add(b_Audio_Start.setup("Start", false));
		Group_Audio.add(b_Audio_Stop.setup("Stop", false));
		Group_Audio.add(b_Audio_Reset.setup("Reset", false));
	gui.add(&Group_Audio);
	
	Group_FFT.setup("FFT");
		Group_FFT.add(FFT__SoftGain.setup("FFT__SoftGain", 1.0, 1.0, 5.0));
		Group_FFT.add(FFT__k_smooth.setup("FFT__k_smooth", 0.95, 0.8, 1.0));
		Group_FFT.add(FFT__dt_smooth_2.setup("FFT__dt_smooth_2", 167, 10, 300));
		Group_FFT.add(FFT__b_Window.setup("FFT__b_Window", true));
	gui.add(&Group_FFT);
	
	Group_ArtSin.setup("ArtSin");
		Group_ArtSin.add(ArtSin_Band_min.setup("ArtSin_Band_min", 1.0, 1.0, 255.0));
		Group_ArtSin.add(ArtSin_Band_max.setup("ArtSin_Band_max", 1.0, 1.0, 255.0));
		Group_ArtSin.add(ArtSin_PhaseMap_k.setup("ArtSin_PhaseMap_k", 1.0, 0.0, 2.0));
		Group_ArtSin.add(b_ArtSin_abs.setup("b_ArtSin_abs", false));
		Group_ArtSin.add(b_Window_artSin.setup("b_Window_artSin", false));
		Group_ArtSin.add(Tukey_alpha.setup("Tukey_alpha", 0.3, 0.0, 1.0));
	gui.add(&Group_ArtSin);
	
	Group_ArtSin2D.setup("ArtSin2D");
		Group_ArtSin2D.add(b_Draw_ArtSin2D.setup("ArtSin2D:b_Draw", true));
		Group_ArtSin2D.add(ArtSin2D_BarHeight.setup("ArtSin2D:BarHeight", 200, 0.0, 1000));
		{
			ofColor initColor = ofColor(255, 255, 255, 140);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_ArtSin2D.add(col_ArtSin2D.setup("ArtSin2D:col", initColor, minColor, maxColor));
		}
	gui.add(&Group_ArtSin2D);
	
	Group_img.setup("img");
		Group_img.add(img_alpha.setup("img:alpha", 20, 0.0, 255.0));
	gui.add(&Group_img);
	
	/********************
	********************/
	gui.minimizeAll();
}

