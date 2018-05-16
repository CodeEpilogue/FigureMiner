#include<stdio.h>
#include<stdlib.h>
#include<easyx.h>
#include<conio.h>
#include<time.h>
#include<windows.h>
#include <Shlwapi.h>
#include "EasyXPng/EasyXPng.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "winmm.lib")

#define CMD_LEFT 1
#define CMD_RIGHT 2
#define CMD_DOWN 4
#define CMD_QUIT 8
#define Width 1280
#define Height 720
#define LWidth 135 
#define LHeight 188
#define SWidth 75 
#define SHeight 113
#define WButton 215
#define HButton 45
#define Xrate 7
#define MAX_DOLL 8

typedef struct object//用于描述矿工、钩子
{
	int x;//x坐标
	int y;//y坐标
	IMAGE_PNG *routePng;//png图片指针
}object;

typedef struct character//用于描述手办图片
{
	int value;//分值
	IMAGE_PNG* path;//png图片指针
} character;

typedef struct Note//用于描述弹幕
{
	character *charaDesc;//使用的手办图片、价值
	int xspeed;//x方向速度
	int yspeed;//y方向速度
	int x;//x坐标
	int y;//y坐标
	int appearfs;//出现的帧数
	int status;//该弹幕的状态
	Note *next;//形成链表
} Note;

typedef struct Rank//用于排行榜
{
	char ID[12];//用户ID
	int rank;//排名
	int Score;//积分
}Rank;

typedef struct PicSource
{
	object Catcher;//钩子
	object Miner;//矿工
	IMAGE *Background[5];//背景图片
	IMAGE *Choose;//选关界面背景
	IMAGE_PNG *Source[5][MAX_DOLL];//全部手办图片资源
	IMAGE_PNG *button[11];//游戏界面按钮
	IMAGE_PNG *wel_button[9];
	IMAGE_PNG *panel[5];
	/*********************
	panel[0] Continue
	panel[1] Exit
	panel[2] Retry
	*********************/
	IMAGE_PNG *GeneralSets[10];
	/*********************
	GeneralSets[0] Mark
	GeneralSets[1] Turn
	GeneralSets[2] next_stage
	GeneralSets[3] Next
	GeneralSets[4] Victory
	GeneralSets[5] Fail
	GeneralSets[6] return1
	GeneralSets[7] return2
	GeneralStes[8] Rank
	GeneralSets[9]
	*********************/
	IMAGE *Welcome[2];//欢迎界面背景贴图
	IMAGE *GAMEOVER;//失败界面
} PicSource;

typedef struct ButtonPhase//描述按钮状态
{
	bool ButtonSound;
	bool ButtonExit;
	int  PauseStatus;
	int  ChooseStatus;
	int  AchieveStatus;
	int  LibStauts;
} ButtonPhase;

void  welcome(int* status, char *ID, char *Path, PicSource* Pic);

void  Sourceload(char *Path, PicSource* Pic);
Note* CreCharacterList(int chapter, PicSource* Pic);

void  LogicCaculation(int *score, int *chapter, ButtonPhase* status, int *UpDown, int *deltaE, float *H_Time, int *H_Energy, int *fs,  Note *head, Note* OutPut, char *Path, int *isTurn, PicSource *Pic);
void  MouseMsgLogic(ButtonPhase *status, int chapter);
int   KBGetCommand();
void  KBDispatchCommand(int _cmd, int *UpDown, PicSource *Pic);
void  WhetherDown(int *UpDown, int *deltaE, Note* H_OutPut, int *chapter, PicSource* Pic);
int   WhetherUp(int *UpDown, Note* H_OutPu, int *chaptert, PicSource* Pic);
void  OnLeft(PicSource* Pic);
void  OnRight(PicSource* Pic);
void  isCatch(int *UpDown, Note* H_OutPut, int *chapter, PicSource* Pic);
void  Bar(int fs, int *deltaE, float *H_Time, int *H_Energy, int *Score, int *Continue, PicSource* Pic);
void  NodeSearch(Note* head, Note* H_OutPut, int fs, int chapter);
void  PicPosition(Note *H_OutPut, int *UpDown, int *H_Energy, int *Score, int fs, PicSource* Pic);

void  Draw(Note* H_OutPut, float H_Time, int H_Energy, int *UpDown, int *Score, int *chapter, ButtonPhase status, PicSource* Pic);

void  DelNoteList(Note* head);
void  Record(char *ID, int *Score, int chapter, char* Path);
void  Turn(int chapter, Note* head, Note* H_outPut, char* ID, int* Score, char* Path, PicSource* Pic);

void  Music(char *Path, int chapter);
int   GameOver(PicSource* Pic);
void  GameWin(char *ID, int *Score, char* Path, PicSource* Pic);

int   ButtonOperation(int x1, int y1, int x2, int y2);
void  Button(ButtonPhase *status, int *chapter, bool *isChoose, char* Path, Note *head, Note *H_OutPut, PicSource* Pic);
void  Button_Pause(int *PauseStatus, PicSource* Pic);
void  Button_Choose(int *ChooseStatus, int *chapter, bool *isChoose, PicSource* Pic);
void  Button_Achieve(int *AchieveStatus, int chapter, char* Path, PicSource* Pic);
void  Button_Lib(int *LibStauts, int chapter, PicSource* Pic);

void main()
{
	char Path[MAX_PATH];
	GetModuleFileName(NULL, Path, MAX_PATH);
	PathRemoveFileSpec(Path);
	PicSource* Pic = new PicSource();
	ButtonPhase *status = (ButtonPhase*)malloc(sizeof(ButtonPhase));
	
	initgraphEx();
	initgraph(Width, Height);
	int wel_status = 0;
	char ID[12] = "";
	welcome(&wel_status, ID, Path, Pic);
	Sourceload(Path, Pic);
	int chapter = 1;
	
	while (1)
	{
		//数据初始化
		int Score = 0;
		int fs = 0;
		int isContinue = 0;

		status->ButtonSound = false;
		status->ButtonExit = false;
		status->AchieveStatus = 0;
		status->ChooseStatus = 0;
		status->PauseStatus = 0;
		status->LibStauts = 0;

		Pic->Miner.x = (Width / 2) - 30;
		Pic->Miner.y = -10;
		Pic->Catcher.x = Pic->Miner.x + 4;
		Pic->Catcher.y = 40;

		bool isChoose = false;
		int UpDown = 0;
		int deltaE = 0;
		float H_Time = 5.0;
		int H_Energy = 5;
		
		Music(Path, chapter);

		Note* head = CreCharacterList(chapter, Pic);//chapter 1-2-1 -> head->next == NULL ???
		head->appearfs = -1;//标志新开链表
		Note* H_OutPut = (Note*)malloc(sizeof(Note));
		H_OutPut->next = NULL;

		while (1)//每关游戏主体
		{	
			if (H_Time < Height - 5)
			{
				LogicCaculation(&Score, &chapter, status, &UpDown, &deltaE, &H_Time, &H_Energy, &fs, head, H_OutPut, Path, &isContinue, Pic);
				Button(status, &chapter, &isChoose, Path, head, H_OutPut, Pic);
				if (isChoose == true)
				{
					DelNoteList(head);
					DelNoteList(H_OutPut);
					break;
				}
				if (isContinue == 1) {
					DelNoteList(head);
					DelNoteList(H_OutPut);
					break;
				}
				Draw(H_OutPut, H_Time, H_Energy, &UpDown, &Score, &chapter, *status, Pic);
				//if(status->ButtonSound == false)
				Sleep(12);
			}
			else
			{
				Turn(chapter, head, H_OutPut, ID, &Score, Path, Pic);
				Record(ID, &Score, chapter, Path);
				chapter++;
				break;
			}
			fs += 1;//总帧数
		}
	}
	free(status);
	_getch();
	closegraph();
}


void welcome(int *status, char *ID, char *Path, PicSource* Pic)
{
	char SourcePath[MAX_PATH];

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/Enter1.jpg");
	IMAGE *enter;
	enter = new IMAGE();
	loadimage(enter, SourcePath, Width, Height);//文本框输入头文字坐标（500，300）

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/OK0.png");
	Pic->wel_button[0] = new IMAGE_PNG();
	loadimage(Pic->wel_button[0], SourcePath, 160, 160);

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/OK1.png");
	Pic->wel_button[1] = new IMAGE_PNG();
	loadimage(Pic->wel_button[1], SourcePath, 160, 160);
	
	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/OK2.png");
	Pic->wel_button[2] = new IMAGE_PNG();
	loadimage(Pic->wel_button[2], SourcePath, 160, 160);

	int i = 0;
	int flag = 0;
	int sign = 0;
	MOUSEMSG msg;

	BeginBatchDraw();
	putimage(0, 0, enter);
	setlinecolor(BLACK);
	rectangle(505, 280, 787, 345);
	setfillcolor(WHITE);
	solidrectangle(506, 281, 786, 344);
	RECT r = { 505, 280, 787, 345 };
	putimage(820, 240, Pic->wel_button[0]);
	char tmp;
	while (1)
	{		
		
		if (i <= 10 && _kbhit())
		{
	
			tmp = _getch();
			if (i != 0 && tmp == '\b')
			{
				if (sign == 0)
				{
					i -= 1;
					ID[i] = '\0';
				}
				else ID[i] = '\0';
				sign = 0;
			}
			if (tmp != '\b')
			{
				ID[i] = tmp;
				i++;
				ID[i] = '\0';
			}
			settextcolor(GREEN);
			settextstyle(58, 25, _T("Consolas"), 0, 0, 0, false, false, false, false, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY);
			setbkmode(TRANSPARENT);
			solidrectangle(506, 281, 786, 344);
			drawtext(ID, &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);	
		}
		if (i == 11 && _kbhit())
		{
			tmp = _getch();
			if (tmp == '\b')
			{
				i -= 1;
				sign = 1;
			}
		}
		EndBatchDraw();
		while (MouseHit() == true)
		{
			msg = GetMouseMsg();
			if (msg.x >= 820 && msg.x <= 980 && msg.y >= 240 && msg.y <= 400)
			{
				putimage(820, 240, Pic->wel_button[1]);
				if (msg.uMsg == WM_LBUTTONUP && ID[0] != '\0')
				{
					flag = 1;
					break;
				}
				if (msg.mkLButton) {
					putimage(820, 240, Pic->wel_button[2]);
				}
			}
			else {
					putimage(820, 240, Pic->wel_button[0]);
			}
		}
		EndBatchDraw();
		if (flag == 1 ) break;
		Sleep(12); 
	}
	
	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/START0.jpg");
	Pic->Welcome[0] = new IMAGE();
	loadimage(Pic->Welcome[0], SourcePath, Width, Height);

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/START1.jpg");
	Pic->Welcome[1] = new IMAGE();
	loadimage(Pic->Welcome[1], SourcePath, Width, Height);

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/button1Green.png");
	Pic->wel_button[3] = new IMAGE_PNG;
	loadimage(Pic->wel_button[3], SourcePath, 200, 200);

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/button1Red.png");
	Pic->wel_button[4] = new IMAGE_PNG;
	loadimage(Pic->wel_button[4], SourcePath, 200, 200);

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/button2Green.png");
	Pic->wel_button[5] = new IMAGE_PNG;
	loadimage(Pic->wel_button[5], SourcePath, 200, 200);
	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/button2Red.png");
	Pic->wel_button[6] = new IMAGE_PNG;
	loadimage(Pic->wel_button[6], SourcePath, 200, 200);

	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/button3Green.png");
	Pic->wel_button[7] = new IMAGE_PNG;
	loadimage(Pic->wel_button[7], SourcePath, 200, 200);
	strcpy_s(SourcePath, Path);
	PathAppend(SourcePath, "Assets/Welcome/button3Red.png");
	Pic->wel_button[8] = new IMAGE_PNG;
	loadimage(Pic->wel_button[8], SourcePath, 200, 200);

	while (1)
	{
		MOUSEMSG msg;
		if (*status == 0) {
			putimage(0, 0, Pic->Welcome[0]);
		}
		else putimage(0, 0, Pic->Welcome[1]);
		bool isTurn = false;
		while (MouseHit())
		{
			msg = GetMouseMsg();
			if (msg.x >= 530 && msg.x <= 776 && msg.y >= 334 && msg.y <= 381)
			{
				*status = 1;
				if (msg.mkLButton) {
					isTurn = true;
				}
			}
			else
			{
				*status = 0;
			}
		}
		if (isTurn) break;		
		Sleep(12);
	}

}
void Sourceload(char *Path, PicSource* Pic)
{
	char CfgFilePath[MAX_PATH], ItemName[255], bkName[255], buName[255], RelativePath[MAX_PATH], AbsolutePath[MAX_PATH];
	strcpy_s(CfgFilePath, Path);
	PathAppend(CfgFilePath, "Assets/config/Source.ini");//Config ini路径

	Pic->Miner.routePng = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/miner.png");
	loadimage(Pic->Miner.routePng, AbsolutePath);
	Pic->Catcher.routePng = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Catcher.png");
	loadimage(Pic->Catcher.routePng, AbsolutePath, 55, 64);

	Pic->panel[0] = new IMAGE_PNG();//Pause Panel
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Continue.png");
	loadimage(Pic->panel[0], AbsolutePath);

	Pic->panel[1] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Exit.png");
	loadimage(Pic->panel[1], AbsolutePath);

	Pic->panel[2] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Retry.png");
	loadimage(Pic->panel[2], AbsolutePath);

	Pic->panel[3] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Library.png");
	loadimage(Pic->panel[3], AbsolutePath);

	Pic->panel[4] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Win.png");
	loadimage(Pic->panel[4], AbsolutePath);

	Pic->GeneralSets[0] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/Pic->GeneralSets/Mark.png");
	loadimage(Pic->GeneralSets[0], AbsolutePath);

	Pic->GeneralSets[1] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Turn.png");
	loadimage(Pic->GeneralSets[1], AbsolutePath);

	Pic->GeneralSets[2] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/next_stage.png");
	loadimage(Pic->GeneralSets[2], AbsolutePath);

	Pic->GeneralSets[3] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Next.png");
	loadimage(Pic->GeneralSets[3], AbsolutePath);

	Pic->GeneralSets[4] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Victory.png");
	loadimage(Pic->GeneralSets[4], AbsolutePath);

	Pic->GeneralSets[5] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Fail.png");
	loadimage(Pic->GeneralSets[5], AbsolutePath);

	Pic->GeneralSets[6] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/return1.png");
	loadimage(Pic->GeneralSets[6], AbsolutePath);

	Pic->GeneralSets[7] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/return2.png");
	loadimage(Pic->GeneralSets[7], AbsolutePath);

	Pic->GeneralSets[8] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Rank.png");
	loadimage(Pic->GeneralSets[8], AbsolutePath);

	Pic->GeneralSets[9] = new IMAGE_PNG();
	strcpy_s(AbsolutePath, Path);
	PathAppend(AbsolutePath, "Assets/GeneralSets/Choose.png");
	loadimage(Pic->GeneralSets[9], AbsolutePath);

	int chapters = GetPrivateProfileInt("Characters", "chapters", 0, CfgFilePath);
	for (int i = 1; i <= chapters; i++) {
		for (int j = 1; j <= MAX_DOLL; j++)
		{
			sprintf_s(ItemName, "image%d%d", i, j);
			GetPrivateProfileString("Characters", ItemName, "", RelativePath, MAX_PATH, CfgFilePath);
			Pic->Source[i - 1][j - 1] = new IMAGE_PNG();
			strcpy_s(AbsolutePath, Path);
			PathAppend(AbsolutePath, RelativePath);
			loadimage(Pic->Source[i - 1][j - 1], AbsolutePath, LWidth, LHeight);
		}
	}

	int cnt2 = GetPrivateProfileInt("Background", "cnt2", 0, CfgFilePath);
	for (int i = 1; i <= cnt2; i++)
	{
		sprintf_s(bkName, "background%d", i);
		GetPrivateProfileString("Background", bkName, "", RelativePath, MAX_PATH, CfgFilePath);
		Pic->Background[i - 1] = new IMAGE();
		strcpy_s(AbsolutePath, Path);
		PathAppend(AbsolutePath, RelativePath);
		loadimage(Pic->Background[i - 1], AbsolutePath, Width, Height);
	}

	int cnt3 = GetPrivateProfileInt("Button", "cnt3", 0, CfgFilePath);
	for (int i = 1; i <= cnt3; i++)
	{
		sprintf_s(buName, "button%d", i);
		GetPrivateProfileString("Button", buName, "", RelativePath, MAX_PATH, CfgFilePath);
		Pic->button[i - 1] = new IMAGE_PNG();
		strcpy_s(AbsolutePath, Path);
		PathAppend(AbsolutePath, RelativePath);
		loadimage(Pic->button[i - 1], AbsolutePath);
	}
}
Note* CreCharacterList(int chapter, PicSource* Pic)
{
	character *characters = NULL;
	Note *note_H, *note_T, *note_N;
	note_H = (Note *)malloc(sizeof(Note));
	note_H->next = NULL;
	note_T = note_H;
	char Path[MAX_PATH], CfgFilePath[MAX_PATH],ChapterName[MAX_PATH];
	GetModuleFileName(NULL, Path, MAX_PATH);
	PathRemoveFileSpec(Path);
	strcpy_s(CfgFilePath, Path);
	sprintf_s(ChapterName, "Assets/config/%d.ini", chapter);//
	PathAppend(CfgFilePath, ChapterName);
	int cnt_notes = GetPrivateProfileInt("General", "cnt_notes", 0, CfgFilePath);//
	int cnt_charas = GetPrivateProfileInt("General", "cnt_charas", 0, CfgFilePath);//
	char SectionName[255] = "";
	int charaId_temp = 0;

	characters = (character *)malloc(cnt_charas * sizeof(character));

	for (int i = 0; i < cnt_charas; i++)//待改
	{
		sprintf_s(SectionName, "Character%d", i + 1);//
		characters[i].value = GetPrivateProfileInt(SectionName, "value", 0, CfgFilePath);
		characters[i].path = Pic->Source[chapter - 1][i];
	}

	for (int i = 1; i <= cnt_notes; i++) {
		sprintf_s(SectionName, "note%d", i);
		note_N = (Note *)malloc(sizeof(Note));
		note_N->appearfs = GetPrivateProfileInt(SectionName, "appearfs", 0, CfgFilePath);
		charaId_temp = GetPrivateProfileInt(SectionName, "charaId", 0, CfgFilePath);
		note_N->charaDesc = &characters[charaId_temp - 1];
		note_N->xspeed = GetPrivateProfileInt(SectionName, "xspeed", 0, CfgFilePath);
		note_N->yspeed = 0;
		note_N->y = GetPrivateProfileInt(SectionName, "position", 0, CfgFilePath);
		note_N->x = -125;
		note_N->status = 0;
		note_N->next = NULL;
		note_T->next = note_N;
		note_T = note_N;
	}
	return note_H;
	
}

void LogicCaculation(int *Score, int *chapter, ButtonPhase *status, int *UpDown, int *deltaE, float *H_Time, int *H_Energy, int *fs, Note *head, Note *H_OutPut, char *Path, int *isTurn, PicSource *Pic)//待改结构体
{
	MouseMsgLogic(status, *chapter);
	NodeSearch(head, H_OutPut, *fs, *chapter);
	PicPosition(H_OutPut, UpDown, H_Energy, Score, *fs, Pic);
	KBDispatchCommand(KBGetCommand(), UpDown, Pic);
	WhetherDown(UpDown, deltaE, H_OutPut, chapter, Pic);
	Bar(*fs, deltaE, H_Time, H_Energy, Score, isTurn, Pic);
}
void MouseMsgLogic(ButtonPhase *status, int chapter)
{
	MOUSEMSG msg;
	while (MouseHit() == true)
	{
		msg = GetMouseMsg();
		if (msg.x >= 75 && msg.x <= 120 && msg.y >= 675 && msg.mkLButton == true && status->ButtonSound == false)
		{
			status->ButtonSound = true;
			mciSendString("stop mymusic", NULL, 0, NULL);
			//PlaySound(NULL, NULL, NULL);
		}
		else if (msg.x >= 75 && msg.x <= 120 && msg.y >= 675 && msg.mkLButton == true && status->ButtonSound == true)
		{
			status->ButtonSound = false;
			mciSendString("play mymusic", NULL, 0, NULL);
		}



		if (msg.x >= 125 && msg.x <= 125 + WButton && msg.y >= Height - HButton && status->PauseStatus == 0)
			status->PauseStatus = 1;
		else if (msg.x >= 125 && msg.x <= 125 + WButton && msg.y >= Height - HButton && (status->PauseStatus == 0 || status->PauseStatus == 1) && msg.mkLButton == true)
			status->PauseStatus = 2;
		if ((msg.x < 125 || msg.x > 125 + WButton || msg.y < Height - HButton) && status->PauseStatus == 1) status->PauseStatus = 0;


		if (msg.x >= 130 + WButton && msg.x <= 130 + 2 * WButton && msg.y >= Height - HButton && status->ChooseStatus == 0)
			status->ChooseStatus = 1;
		else if (msg.x >= 130 + WButton && msg.x <= 130 + 2 * WButton && msg.y >= Height - HButton && (status->ChooseStatus == 0 || status->ChooseStatus == 1) && msg.mkLButton == true)
			status->ChooseStatus = 2;
		if ((msg.x < 130 + WButton || msg.x > 130 + 2 * WButton || msg.y < Height - HButton) && status->ChooseStatus == 1) status->ChooseStatus = 0;


		if (msg.x >= 290 + 2 * WButton && msg.x <= 290 + 3 * WButton && msg.y >= Height - HButton && status->AchieveStatus == 0)
			status->AchieveStatus = 1;
		else if (msg.x >= 290 + 2 * WButton && msg.x <= 290 + 3 * WButton && msg.y >= Height - HButton && (status->AchieveStatus == 0 || status->AchieveStatus == 1) && msg.mkLButton == true)
			status->AchieveStatus = 2;
		if ((msg.x < 290 + 2 * WButton || msg.x > 290 + 3 * WButton || msg.y < Height - HButton) && status->AchieveStatus == 1) status->AchieveStatus = 0;

		if (msg.x >= 295 + 3 * WButton && msg.x <= 295 + 4 * WButton && msg.y >= Height - HButton && status->LibStauts == 0)
			status->LibStauts = 1;
		else if (msg.x >= 295 + 3 * WButton && msg.x <= 295 + 4 * WButton && msg.y >= Height - HButton && (status->LibStauts == 0 || status->LibStauts == 1) && msg.mkLButton == true)
			status->LibStauts = 2;
		if ((msg.x < 295 + 3 * WButton || msg.x > 295 + 4 * WButton || msg.y < Height - HButton) && status->LibStauts == 1) status->LibStauts = 0;



		if (msg.x >= 1160 && msg.x <= 1205 && msg.y >= 675 && msg.mkLButton == true && status->ButtonExit == false)
		{
			exit(0);
		}
	}
}
int  KBGetCommand()
{
	int c = 0;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)            c |= CMD_LEFT;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)           c |= CMD_RIGHT;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)            c |= CMD_DOWN;
	return c;
}
void KBDispatchCommand(int _cmd, int *UpDown, PicSource *Pic)
{
	if (_cmd & CMD_DOWN)
	{
		if (*UpDown == 0) {
			*UpDown = 1;
		}
	}
	if (_cmd & CMD_LEFT)		OnLeft(Pic);
	if (_cmd & CMD_RIGHT)		OnRight(Pic);
}

void isCatch(int *UpDown, Note* H_OutPut, int *chapter, PicSource* Pic)
{
	Note* p = H_OutPut->next;
	//sprintf_s(DebugInfo, "UpDown = %d", *UpDown);

	//具体判定待补 真真——大(l)；真假——小(s)；假假——无
	if (*UpDown == 1)//下降阶段（1）三点像素与其相同位置背景颜色不同时——judge[0] = true
	{
		DWORD*buffer;
		buffer = GetImageBuffer(Pic->Background[*chapter - 1]);
		COLORREF _judge[3], anti_judge[3];
		int i, j, k;
		i = (Pic->Catcher.y + 36) * Width + Pic->Catcher.x + 26;
		j = (Pic->Catcher.y + 46) * Width + Pic->Catcher.x + 3;
		k = (Pic->Catcher.y + 46) * Width + Pic->Catcher.x + 51;
		_judge[0] = BGR(*(buffer + i));
		_judge[1] = BGR(*(buffer + j));
		_judge[2] = BGR(*(buffer + k));

		//sprintf_s(DebugInfo, "judge = 0x%x, 0x%x, 0x%x\nantij = 0x%x, 0x%x, 0x%x", _judge[0], _judge[1], _judge[2], anti_judge[0], anti_judge[1], anti_judge[2]);

		anti_judge[0] = getpixel(Pic->Catcher.x + 26, Pic->Catcher.y + 36);
		anti_judge[1] = getpixel(Pic->Catcher.x + 3, Pic->Catcher.y + 46);
		anti_judge[2] = getpixel(Pic->Catcher.x + 51, Pic->Catcher.y + 46);
		if (((anti_judge[0] != _judge[0]) + (anti_judge[1] != _judge[1]) + (anti_judge[2] != _judge[2])) >= 2)
		{
			*UpDown = 3;
			
			while (p != NULL)
			{	
				int pi1 = (Pic->Catcher.x + 26 >= p->x && Pic->Catcher.x + 26 <= p->x + LWidth && Pic->Catcher.y + 36 >= p->y&&Pic->Catcher.y + 36 <= p->y + LHeight);
				int pi2 = (Pic->Catcher.x + 3 >= p->x && Pic->Catcher.x + 3 <= p->x + LWidth && Pic->Catcher.y + 46 >= p->y&&Pic->Catcher.y + 46 <= p->y + LHeight);
				int pi3 = (Pic->Catcher.x + 51 >= p->x && Pic->Catcher.x + 51 <= p->x + LWidth && Pic->Catcher.y + 46 >= p->y&&Pic->Catcher.y + 46 <= p->y + LHeight);
				if (pi1 + pi2 + pi3 >= 2) 
				{
					p->xspeed = 0;
					p->yspeed = -10;
					p->status = 2;
				}
				p = p->next;
			}
		}

	}
}

void WhetherDown(int *UpDown, int *deltaE, Note* H_OutPut, int *chapter, PicSource* Pic)
{
	if(*UpDown == 1)
	{	
		Pic->Catcher.y += (9 + *chapter);
		if (Pic->Catcher.y > 600)
		{
			Pic->Catcher.y = 610;
			*UpDown = 2;
		}
	}
		*deltaE += WhetherUp(UpDown, H_OutPut, chapter, Pic);
}
int  WhetherUp(int *UpDown, Note* H_OutPut, int *chapter, PicSource* Pic)
{
	isCatch(UpDown, H_OutPut, chapter, Pic);
	if(*UpDown == 3)// isCatch
	{
		
		if(Pic->Catcher.y > Pic->Miner.y + 50)
		{
			Pic->Catcher.y -= (9 + *chapter);
		}
		if (Pic->Catcher.y > Pic->Miner.y + 40 && Pic->Catcher.y < Pic->Miner.y + 50)
		{
			Pic->Catcher.y = Pic->Miner.y + 50;
			//*UpDown = 0;
		}
		if (Pic->Catcher.y == Pic->Miner.y + 50)
		{
			*UpDown = 0;
			return 200;
		}
	}
	else if (*UpDown == 2)// noCatch
	{
		
		if (Pic->Catcher.y > Pic->Miner.y + 50)
		{
			Pic->Catcher.y -= 10;
		}
		if (Pic->Catcher.y < Pic->Miner.y + 50 && Pic->Catcher.y >= Pic->Miner.y + 40)
		{
			Pic->Catcher.y = Pic->Miner.y + 50;
		}
		if (Pic->Catcher.y == Pic->Miner.y + 50)
			*UpDown = 0;
	}
	return 0;
}
void OnRight(PicSource* Pic)
{
	if (Pic->Miner.x <= 1160)
	{
		Pic->Miner.x += Xrate;
		Pic->Catcher.x += Xrate;
	}
}
void OnLeft(PicSource* Pic)
{
	if (Pic->Miner.x >= 60)
	{
		Pic->Miner.x -= Xrate;
		Pic->Catcher.x -= Xrate;
	}
}


void Bar(int fs, int *deltaE, float *H_Time, int *H_Energy, int *Score, int *isContinue, PicSource *Pic)
{
	if (fs % 79 == 0)
	{
		*H_Time += 14.2f;
	}
	if (*H_Time >= Height - 5) *H_Time = Height - 5;
	if (fs % 79 == 0)
	{
		*H_Energy += 90;
	}
	if (*H_Energy < 5)
		*H_Energy = 5;
		
	if (*H_Energy > Height - 5)
	{
		*H_Energy = Height - 5;
		*isContinue = GameOver(Pic);
	}
	
}

void NodeSearch(Note* head, Note* H_OutPut, int fs, int chapter)
{
		static Note* temp1 = head->next;
		if (head->appearfs == -1) {
			temp1 = head->next;
			head->appearfs = 0;
		}
		Note *Tmp;
		if ((temp1->appearfs <= fs) && temp1->status == 0)
		{
			temp1->status = 1;
			Tmp = (Note*)malloc(sizeof(Note));
			*Tmp = *temp1;
			Tmp->next = NULL;

			Note *T_OutPut = H_OutPut;
			while (T_OutPut->next != NULL) {
				T_OutPut = T_OutPut->next;
			}
			T_OutPut->next = Tmp;
			T_OutPut = Tmp;

			if (temp1->next == NULL) temp1 = temp1;
			else temp1 = temp1->next;
		}
}
void PicPosition(Note* H_OutPut, int *UpDown, int *H_Energy, int *Score, int fs, PicSource *Pic)
{
	Note *p = H_OutPut->next;
	Note *p_prev = H_OutPut;
	while (p != NULL) {
		if (p->x < Width && p->status == 1)
			p->x += p->xspeed;
		if (p->status == 2)
		{
			p->y += p->yspeed;
			p->x = Pic->Miner.x - 35;
		}
		if (p->x > Width)
		{
			p_prev->next = p->next;
			if (p->next == NULL)
			{
				*Score += (Height - 10 - *H_Energy) * 2;
				if (*Score <= 0) *Score = 0;
				*H_Energy = 5;
				free(p);
			}
			p = p_prev->next;
		}
		if (p != NULL) {
			if (p->y <= 10)
			{
				p_prev->next = p->next;
				if (p->next == NULL)
				{
					*Score += p->charaDesc->value;
					*Score += (Height - 10 - *H_Energy) * 2;
					*H_Energy = 5;
					free(p);
					break;
				}
				p = p_prev->next;
				*H_Energy -= 200;
				*Score += p->charaDesc->value;
			}
			else {
				p_prev = p;
				p = p->next;
			}
		}
	}
}
int  ButtonOperation(MOUSEMSG *msg, int x1, int y1, int x2, int y2)//用于同时判断多按钮
{
		if (msg->x >= x1 && msg->x <= x2 && msg->y >= y1 && msg->y <= y2 && msg->mkLButton == true)
		{
			FlushMouseMsgBuffer();
			return 2;
		}
	return 0;
}
int  ButtonOperation(int x1, int y1, int x2, int y2)//单一按钮
{
	MOUSEMSG msg;
	while (MouseHit())
	{
		msg = GetMouseMsg();
		if (msg.x >= x1 && msg.x <= x2 && msg.y >= y1 && msg.y <= y2 && msg.mkLButton == true)
		{
			FlushMouseMsgBuffer();
			 return 2;
		}
	}
	return 0;
}

void Button(ButtonPhase* status, int *chapter, bool *isChoose, char* Path, Note *head, Note *H_OutPut, PicSource* Pic)
{
	Button_Pause(&status->PauseStatus, Pic);
	Button_Choose(&status->ChooseStatus, chapter, isChoose, Pic);
	Button_Achieve(&status->AchieveStatus, *chapter, Path, Pic);
	Button_Lib(&status->LibStauts, *chapter, Pic);
}

void Button_Pause(int *PauseStatus, PicSource* Pic)
{
	if (*PauseStatus == 2)
	{
		mciSendString("stop mymusic", NULL, 0, NULL);
		while (1)
		{
			BeginBatchDraw();
			putimage(530, 210, Pic->panel[0]);
			EndBatchDraw();
			*PauseStatus -= ButtonOperation(530, 210, 750, 430); //msg.x >= 75 && msg.x <= 120 && msg.y >= 675
			if (*PauseStatus == 0)
			{
				mciSendString("play mymusic", NULL, 0, NULL);
				break;
			}
		}
	}
}
void Button_Choose(int *ChooseStatus, int *chapter, bool *isChoose, PicSource* Pic)
{
	/*********************
	Pic->GeneralSets[0] Mark
	Pic->GeneralSets[1] Turn
	Pic->GeneralSets[2] next_stage
	Pic->GeneralSets[3] Next
	Pic->GeneralSets[4] Victory
	Pic->GeneralSets[5] Fail
	Pic->GeneralSets[6] return1
	Pic->GeneralSets[7] return2
	*********************/
	int maybechapter = *chapter;
	MOUSEMSG msg;
	if (*ChooseStatus == 2)
	{
		while (1)
		{
			BeginBatchDraw();
			//cleardevice();
			putimage(0, 0, Pic->GeneralSets[9]);
			putimage(0, 0, Pic->GeneralSets[6]);
			switch (maybechapter)
			{
			case 1:
				putimage(1023, 81, Pic->Miner.routePng);
				break;
			case 2:
				putimage(1048, 560, Pic->Miner.routePng);
				break;
			case 3:
				putimage(685, 448, Pic->Miner.routePng);
				break;
			case 4:
				putimage(111, 239, Pic->Miner.routePng);
				break;
			case 5:
				putimage(468, 610, Pic->Miner.routePng);
				break;
			default:
				break;
			}
			EndBatchDraw();
			while (MouseHit()) {
				msg = GetMouseMsg();
				if (ButtonOperation(&msg, 1028, 86, 1078, 136) == 2)
				{
					BeginBatchDraw();
					cleardevice();
					putimage(0, 0, Pic->GeneralSets[9]);
					putimage(0, 0, Pic->GeneralSets[6]);
					putimage(1026, 84, Pic->Miner.routePng);
					maybechapter = 1;
					EndBatchDraw();
				}
				if (ButtonOperation(&msg, 1053, 565, 1103, 615) == 2)
				{
					BeginBatchDraw();
					cleardevice();
					putimage(0, 0, Pic->GeneralSets[9]);
					putimage(0, 0, Pic->GeneralSets[6]);
					putimage(1051, 563, Pic->Miner.routePng);
					maybechapter = 2;
					EndBatchDraw();
				}
				if (ButtonOperation(&msg, 690, 453, 740, 503) == 2)
				{
					BeginBatchDraw();
					cleardevice();
					putimage(0, 0, Pic->GeneralSets[9]);
					putimage(0, 0, Pic->GeneralSets[6]);
					putimage(688, 451, Pic->Miner.routePng);
					maybechapter = 3;
					EndBatchDraw();
				}
				if (ButtonOperation(&msg, 116, 244, 166, 304) == 2)
				{
					BeginBatchDraw();
					cleardevice();
					putimage(0, 0, Pic->GeneralSets[9]);
					putimage(0, 0, Pic->GeneralSets[6]);
					putimage(114, 242, Pic->Miner.routePng);
					maybechapter = 4;
					EndBatchDraw();
				}
				if (ButtonOperation(&msg, 468, 610, 523, 665) == 2)
				{
					BeginBatchDraw();
					cleardevice();
					putimage(0, 0, Pic->GeneralSets[9]);
					putimage(0, 0, Pic->GeneralSets[6]);
					putimage(471, 613, Pic->Miner.routePng);
					maybechapter = 5;
					EndBatchDraw();
				}
				if (ButtonOperation(&msg, 0, 0, 130, 130) == 2) *ChooseStatus = 0;
			}
			if (*ChooseStatus == 0)
			{
				if (maybechapter != *chapter)
				{
					*chapter = maybechapter;
					*isChoose = true;
					mciSendString("close mymusic", NULL, 0, NULL);
				}
				else *isChoose = false;
				BeginBatchDraw();
				cleardevice();
				putimage(0, 0, Pic->GeneralSets[9]);
				putimage(0, 0, Pic->GeneralSets[7]);
				EndBatchDraw();
				Sleep(300);
				break;
			}
			Sleep(12);
		}
	}
}
int  cmp(const void *a, const void *B)
{
	return ((Rank *)B)->Score - ((Rank *)a)->Score;
}
void Button_Achieve(int *AchieveStatus, int chapter, char* Path, PicSource* Pic)
{
	unsigned int maybechapter = chapter;
	MOUSEMSG msg;
	while (*AchieveStatus == 2)
	{
		FILE *fp;
		char RecordPath[MAX_PATH], add[255];
		strcpy_s(RecordPath, Path);
		sprintf_s(add, "Assets/record/%d.txt", maybechapter);
		PathAppend(RecordPath, add);
		fopen_s(&fp, RecordPath, "r");
		Rank rank[30];
		for (int t = 0; t < 30; t++)
		{
			strcpy_s(rank[t].ID, "Unknow");
			rank[t].Score = 0;
			rank[t].rank = t + 1;
		}
		char pre_rank[30][40];
		for (int t = 0; t < 30; t++)
		{
			strcpy_s(pre_rank[t], "Unknow\t0\n");
		}
		int cnt = 0;
		int num = 0, end = 0;
		while(!feof(fp))//预处理为Rank型
		{
			fgets(pre_rank[cnt], 30, fp);
			cnt++;
		}
		int mid = end = 0;
		int i = 0, j = 0;
		for (i = 0; i < cnt; i++)
		{
			for (j = 0; j < (int)strlen(pre_rank[i]); j++)
			{
				if (pre_rank[i][j] == '\t')
				{
					mid = j;
					break;
				}
			}
			for (j; j < (int)strlen(pre_rank[i]); j++)
			{
				if (pre_rank[i][j] == '\n')
				{
					end = j;
					break;
				}
			}
			for (j = 0; j <= mid - 1; j++)
			{
				rank[i].ID[j] = pre_rank[i][j];
			}
			rank[i].ID[j] = '\0';
			char temp[10];
			int num = 0;
			for (j = mid + 1; j <= end; j++)
			{
				temp[num] = pre_rank[i][j];
				num++;
			}
			rank[i].Score = atoi(temp);
		}
		//排序
		qsort(rank, 30, sizeof(Rank), cmp);
		for (i = 0; i < cnt; i++)
		{
			rank[i].rank = i + 1;
		}
		char text[5][40];
		for (i = 0; i < 5; i++)
		{
			if(strcmp(rank[i].ID, "Unkown") != 0)
				sprintf_s(text[i], " %d      %s", rank[i].rank, rank[i].ID);
		}
		char score[5][10];
		for (i = 0; i < 5; i++)
		{
			sprintf_s(score[i], "%d", rank[i].Score);
		}
		settextcolor(BLACK);
		settextstyle(50, 15, _T("Agency FB"), 0, 0, 0, false, false, false, false, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY);
		setbkmode(TRANSPARENT);
		RECT r0 = { 455, 225, 815, 285 }; RECT r01 = { 685, 225, 815, 285 };
		RECT r1 = { 455, 285, 815, 345 }; RECT r11 = { 685, 285, 815, 345 };
		RECT r2 = { 455, 345, 815, 405 }; RECT r21 = { 685, 345, 815, 405 };
		RECT r3 = { 455, 405, 815, 465 }; RECT r31 = { 685, 405, 815, 465 };
		RECT r4 = { 455, 465, 815, 525 }; RECT r41 = { 685, 465, 815, 525 };
		int flag1 = 0, flag2 = 0;
		while (1)
		{
			BeginBatchDraw();
			putimage(420, 130, Pic->GeneralSets[8]);
			drawtext(text[0], &r0, DT_LEFT | DT_SINGLELINE | DT_VCENTER); drawtext(score[0], &r01, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			drawtext(text[1], &r1, DT_LEFT | DT_SINGLELINE | DT_VCENTER); drawtext(score[1], &r11, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			drawtext(text[2], &r2, DT_LEFT | DT_SINGLELINE | DT_VCENTER); drawtext(score[2], &r21, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			drawtext(text[3], &r3, DT_LEFT | DT_SINGLELINE | DT_VCENTER); drawtext(score[3], &r31, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			drawtext(text[4], &r4, DT_LEFT | DT_SINGLELINE | DT_VCENTER); drawtext(score[4], &r41, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			EndBatchDraw();
			if (MouseHit())
			{
				msg = GetMouseMsg();
				*AchieveStatus -= ButtonOperation(&msg, 760, 520, 849, 609);
				if (ButtonOperation(&msg, 473, 154, 510, 195) == 2 && maybechapter > 1)
				{
					maybechapter -= 1;
					break;
				}

				if (ButtonOperation(&msg, 751, 154, 787, 195) == 2 && maybechapter < 5)
				{
					maybechapter += 1;
					break;
				}
				
			}
			if(*AchieveStatus == 0) break;
			Sleep(12);
		}
	}
}
void Button_Lib(int *LibStauts, int chapter, PicSource* Pic)
{
	if (*LibStauts == 2)
	{
		while (1)
		{
			BeginBatchDraw();
			putimage(0, 0, Pic->panel[3]);
			putimage(94, 74, Pic->Source[chapter - 1][0]);
			putimage(415, 74, Pic->Source[chapter - 1][1]);
			putimage(774, 74, Pic->Source[chapter - 1][2]);
			putimage(50, 470, Pic->Source[chapter - 1][3]);
			putimage(306, 470, Pic->Source[chapter - 1][4]);
			putimage(592, 470, Pic->Source[chapter - 1][5]);
			putimage(818, 470, Pic->Source[chapter - 1][6]);
			putimage(1074, 470, Pic->Source[chapter - 1][7]);
			EndBatchDraw();
			*LibStauts -= ButtonOperation(1152, 0, 1279, 129);
			if (*LibStauts == 0) break;
			Sleep(12);
		}
	}
}

void Draw(Note* H_OutPut, float H_Time, int H_Energy, int *UpDown, int *Score, int *chapter, ButtonPhase status, PicSource* Pic)
{
	
	BeginBatchDraw();
	cleardevice();
	putimage(0, 0, Pic->Background[*chapter - 1]);
	if (status.ButtonSound == false) putimage(75, Height - 45, Pic->button[0]);
	else putimage(75, Height - 45, Pic->button[1]);

	if (status.PauseStatus == 0) putimage(125, Height - HButton, Pic->button[2]);
	else if (status.PauseStatus == 1)putimage(125, Height - HButton, Pic->button[3]);

	if (status.ChooseStatus == 0) putimage(345, Height - HButton, Pic->button[4]);
	else if (status.ChooseStatus == 1) putimage(345, Height - HButton, Pic->button[5]);

	if (status.AchieveStatus == 0) putimage(720, Height - HButton, Pic->button[6]);
	else if (status.AchieveStatus == 1) putimage(720, Height - HButton, Pic->button[7]);

	if (status.LibStauts == 0) putimage(295 + WButton * 3, Height - HButton, Pic->button[8]);
	else if (status.LibStauts == 1) putimage(295 + WButton * 3, Height - HButton, Pic->button[9]);
	if (status.ButtonExit == false) putimage(1160, 675, Pic->button[10]);

	setlinestyle(PS_SOLID | PS_ENDCAP_FLAT, 3);
	setlinecolor(RGB(128, 64, 0));
	line(Pic->Miner.x + 30, Pic->Miner.y + 50, Pic->Miner.x + 30, Pic->Catcher.y - 2);
	putimage(Pic->Miner.x, Pic->Miner.y, Pic->Miner.routePng);
	putimage(Pic->Catcher.x, Pic->Catcher.y, Pic->Catcher.routePng);

	setfillcolor(LIGHTGRAY);
	solidrectangle(Width - 1, 1, Width - 60, Height - 1);
	setfillcolor(YELLOW);
	solidrectangle(Width - 55, 5, Width - 5, Height - 5);

	setfillcolor(LIGHTGRAY);
	solidrectangle(1, 1, 60, Height - 1);
	setfillcolor(BLACK);
	solidrectangle(5, 5, 55, Height - 5);

	setfillcolor(BLACK);
	solidrectangle(Width - 55, (int)H_Time, Width - 5, 5);//显示左侧时间条
	
	setfillcolor(RGB(0, 185, 230));
	solidrectangle(5, H_Energy, 55, Height - 5);//显示左侧能量条

	Note *p = H_OutPut->next;
	while (p != NULL)
	{
		if (p->x >= 40 && p->x < Width - 175 && p->status == 1) putimage(p->x, p->y, p->charaDesc->path);
		if (p->status == 2) putimage(p->x, p->y, p->charaDesc->path);
		p = p->next;
	}

	setlinestyle(PS_SOLID | PS_ENDCAP_FLAT, 4);
	setlinecolor(LIGHTGRAY);
	rectangle(135 + 2 * WButton, Height - 60, 285 + 2 * WButton, Height - 1);
	settextcolor(RGB(238,199,16));
	settextstyle(70,30,_T("Candara"));
	setbkmode(TRANSPARENT);
	char put_score[7];
	sprintf_s(put_score, "%d", *Score);
	outtextxy(135 + 2 * WButton, Height - 70, put_score);
	
	EndBatchDraw();
}
void DelNoteList(Note* head)
{
	Note p = *head;
	if (head == NULL) return;
	while (p.next != NULL)
	{
		Note* q = p.next;
		p.next = p.next->next;
		free(q);
	}
	free(head);
}

void Record(char *ID, int *Score, int chapter, char* Path)
{
	FILE *fp;
	char RecordPath[MAX_PATH], add[255];
	strcpy_s(RecordPath, Path);
	sprintf_s(add, "Assets/record/%d.txt", chapter);
	PathAppend(RecordPath, add);
	fopen_s(&fp, RecordPath, "a");
	char score[10];
	strcat_s(ID, 12, "\t");
	sprintf_s(score, "%d\n", *Score);
	fseek(fp, 0, SEEK_END);
	fputs(ID, fp);
	fputs(score, fp);
	fclose(fp);
}

void Turn(int chapter, Note* head, Note* H_outPut, char *ID, int *Score, char* Path, PicSource* Pic)
{
	int flag = 2;
	int flag_exit = 2;
	if (chapter < 5)
	{
		while (1)
		{
			BeginBatchDraw();
			putimage(453, 60, Pic->GeneralSets[1]);
			putimage(360, 410, Pic->GeneralSets[2]);
			putimage(885, 396, Pic->GeneralSets[3]);
			EndBatchDraw();
			flag -= ButtonOperation(885, 396, 975, 486);
			flag_exit -= ButtonOperation(1160, 675, 1205, Height - 1);
			if (flag == 0)
			{
				mciSendString("close mymusic", NULL, 0, NULL);
				DelNoteList(H_outPut);
				DelNoteList(head);
				break;
			}
			if (flag_exit == 0) {
				mciSendString("close mymusic", NULL, 0, NULL);
				exit(0);
			}
			Sleep(12);
		}
	}
	else GameWin(ID, Score, Path, Pic);
}



void  Music(char *Path, int chapter)
{
	char MusicPath[_MAX_PATH], add[255];
	strcpy_s(MusicPath, Path);
	sprintf_s(add, "Assets/music/%d.mp3", chapter);
	PathAppend(MusicPath, add);

	char Object[_MAX_PATH + 20];
	_stprintf_s(Object, _T("open \"%s\" alias mymusic"), MusicPath);
	mciSendString("close mymusic", NULL, 0, NULL);
	mciSendString(Object, NULL, 0, NULL);
		
	// 播放音乐
	mciSendString(_T("play mymusic repeat"), NULL, 0, NULL);
}

int GameOver(PicSource* Pic)
{
	int flag1 = 2, flag2 = 2;
	MOUSEMSG msg;
	Pic->GAMEOVER = new IMAGE();
	getimage(Pic->GAMEOVER, 0, 0, Width, Height);
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, Pic->GAMEOVER);
		putimage(420, 220, Pic->GeneralSets[5]);
		putimage(320, 350, Pic->panel[1]);
		putimage(760, 350, Pic->panel[2]);
		while (MouseHit()) {
			msg = GetMouseMsg();
			flag1 -= ButtonOperation(&msg, 320, 350, 570, 450);
			flag2 -= ButtonOperation(&msg, 760, 350, 1010, 450);
		}
		if (flag1 == 0) exit(0);
		if (flag2 == 0) return 1;
		EndBatchDraw();
		Sleep(12);
	}
}

void GameWin(char *ID, int *Score, char* Path, PicSource* Pic)
{
	while (1)
	{
		BeginBatchDraw();
		putimage(0, 0, Pic->panel[4]);
		putimage(500, 240, Pic->GeneralSets[4]);
		putimage(600, 450, Pic->GeneralSets[6]);
		if (ButtonOperation(600, 450, 700, 550) == 2) {
			Record(ID, Score, 5, Path);
			mciSendString("close mymusic", NULL, 0, NULL);
			exit(0);
		}
		EndBatchDraw();
		Sleep(12);
	}
}