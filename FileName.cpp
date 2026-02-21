/*                 虽然是跟着b站大佬的思路写的，源代码不算百分之百原创，但好歹是我一个字母一个字母亲自敲出来的，也学到了很多东西和编程的思维
*                  有相当一部分功能的构思，代码的实现是我自己原创的
*/

#include<stdio.h>//标准输入输出头文件
#include<vector>//向量容器头文件
#include<easyx.h>//easyx图形库头文件
#include<graphics.h>//图形库头文件
#include<Windows.h>//Windows API头文件
#include<mmsystem.h>//多媒体API头文件
#include"tools.h"//自定义工具头文件
#include <conio.h>//控制台输入输出头文件
#include<time.h>//时间函数头文件
#pragma comment(lib,"winmm.lib")//链接多媒体库
#pragma comment(lib, "msimg32.lib")//链接msimg32库以支持透明图片绘制
#define WIN_WIDTH 1440//宏不能加;
#define WIN_HEIGHT 810
#define OBSTACLE_COUNT 10
#define WIN_SCORE 100
using namespace std;
//在C++中，IMAGE类是一个用于图像处理的类
IMAGE imgBgs[3];//存三个背景图
//人物素材：千早爱音，丰川箱子，三角初音
IMAGE anon;
IMAGE box;
IMAGE imganon[12];//三角初音的跑步图片帧素材
IMAGE imganonDown;//三角初音下蹲图片帧素材
int bgx[3];//存三个背景图的x坐标z
int bgspeed[3] = { 1,2,16 };//背景图滚动速度
int anonX;//千早爱音的X坐标
int anonY;//千早爱音的Y坐标
int anonIndex;//千早爱音图片帧序号
bool anonjump;//true表示正在跳跃
bool anonDown;//true表示正在下蹲
int Height;//最大跳跃高度
int v_jump;//跳跃速度，即跳跃状态的每一次渲染的偏移量
int updating;//表示是否立即刷新画面
int heroBlood;//角色血量
int score;
//障碍物类型
typedef enum {//枚举用，函数用，for用；
	kobe,
	tomorin,
	pao,
	HOOK1,
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT,
}obstacle_type;
//障碍物素材
vector<vector<IMAGE>>obstacleImgs;//存放障碍物的图片的容器
//用结构体打包障碍物信息
typedef struct obstacle {
	//obstacle_type type;//障碍物的类型
	int type;//障碍物类型
	int x, y;//障碍物的坐标
	int imgIndex;//当前显示的障碍物的图片序号
	int speed;//障碍物速度
	int power;//杀伤力
	bool exist;//是否存在 
	bool hited;//表示已经发生碰撞
	bool passed;//表示是否被通过
}obstacle_t;
//障碍物池
obstacle_t obstacles[OBSTACLE_COUNT];
int lastObsIndex;
int aha_num;//计数器，用于碰到阿哈键位转换
IMAGE imgSZ[10];//分数图片
IMAGE imgStartBtn;//开始游戏按钮
IMAGE imgOverBtn;//战败CG，再来一次
IMAGE imgWinBtn;//胜利结算，再来一次

void init();//初始化界面,并加载背景图
void update();//渲染背景
void move();//背景的移动,千早爱音的跳跃，障碍物的移动
void keyboard();//键盘输入检测
void jump();//实现跳跃
void updateObstacle();//更新障碍物状态
void createObstacle();//生成障碍物
void updateHero();//更新爱音的状态
void down();//实现下蹲
void checkHit();//碰撞检测
void updateHP();//更新角色血量
void checkOver();//判断游戏结束
void checkScore();//检查分数
void updateScore();//更新分数
void checkWin();//判断胜利结算
void startupMenu();//开始界面
void WINMenu();//胜利结算界面
void OverMenu();//战败CG界面
void keyboard_aha();//和keyboard的键位是反的，这很欢愉！
void check_aha();//检测阿哈数
void resetGame();//重置游戏
//下面这两个函数是gemini辅助我完成的，功能是加载开屏动画，技术含量太高，所以借助了AI的力量
void drawImageWithAlpha(IMAGE* img, int x, int y, int alpha);//实现带透明度的图片绘制
void playGenshinStyleOpening();//实现原神风格的开屏动画
int main()
{
	init();	//初始化界面,并加载背景图

	playGenshinStyleOpening();//对原神的拙劣模仿罢了
	//Sleep(1100);
	loadimage(0, "resource\\begin.jpg");//加载开始菜单界面
	//加载背景音乐
	//使用 mciSendString 的 repeat 参数时，音频结束到重新开始之间确实会有约 100ms - 500ms 的物理停顿。
	// 这是因为 Windows 需要重新读取文件、填充缓冲区并初始化解码器。
	//PlaySound 是 Windows 提供的另一个 API，它在处理 WAV 格式的循环时几乎是瞬间的，没有 MCI 那种明显的重新加载过程。
	PlaySound(TEXT("resource\\为什么要演奏春日影.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	startupMenu();

	int timer = 0;
	while (1)
	{
		//keyboard();
		check_aha();
		timer = timer + getDelay();//在计时达到30ms时刷新渲染，避免无时间间隔渲染造成画面闪烁
		if (timer > 30)
		{
			timer = 0;
			updating = true;
		}
		if (updating)//keyboard检查到空格输入，调用jump函数，updating会true，此时马上执行以下代码，实现无延迟响应
		{
			updating = false;
			BeginBatchDraw();//开启批量绘制模式。调用后，后续的绘图操作不会立即显示在屏幕上，而是先存储在缓冲区中。
			update();
			updateHero();
			updateHP();
			updateObstacle();
			updateScore();
			checkWin();
			EndBatchDraw();//执行批量绘制，将缓冲区中所有待绘制的内容一次性输出到屏幕，并清空缓冲区。
			checkOver();
			checkScore();
			move();//背景和爱音的移动
		}
	}

	system("pause");
	return 0;
}
void init()//图片出不来的问题：1，路径错误(resource前面不应该加\\)；2，格式问题，网上下的图格式不一定对,用画板净化
//vs界面中需要把图形库放在x64\debug目录下
{
	//随机数种子初始化
	srand((unsigned int)time(NULL));
	//初始化图形窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	//加载背景图
	char name[64];
	for (int i = 0; i < 3; i++)
	{
		sprintf(name, "resource/bg%03d.png", i + 1);//resource前面不应该加/
		loadimage(&imgBgs[i], name);//加载图片
		bgx[i] = 0;
	}
	//加载按钮
	loadimage(&imgStartBtn, "resource\\开始按钮.jpg");
	loadimage(&imgOverBtn, "resource\\胜利结算按钮.png");
	loadimage(&imgWinBtn, "resource\\胜利结算按钮.png",310,120);
	//加载小爱音
	loadimage(&anon, "resource/爱音钓祥子.png");
	//加载三角初音
	for (int i = 0; i < 12; i++)
	{
		sprintf(name, "resource/%d.png", i + 1);
		loadimage(&imganon[i], name, 250, 200);
	}
	//加载丰川箱子
	loadimage(&box, "resource/丰川箱子.png");
	//初始化爱音的坐标
	anonX = WIN_WIDTH * 0.4 - imganon[0].getwidth();
	anonY = WIN_HEIGHT * 0.95 - imganon[0].getheight();
	anonIndex = 0;
	anonjump = false;
	Height = WIN_HEIGHT * 0.95 - imganon[0].getheight();
	v_jump = 0;
	updating = true;
	//加载牢大
	IMAGE imgKobe;
	loadimage(&imgKobe, "resource\\Kobe.png");
	vector<IMAGE>imgKobeArray;
	imgKobeArray.push_back(imgKobe);
	obstacleImgs.push_back(imgKobeArray);
	//加载偷摸零
	IMAGE imgtomorin;
	vector<IMAGE>imgtomorinArray;
	for (int i = 0; i < 6; i++)
	{

		sprintf(name, "resource\\tomorin%d.png", i + 1);
		loadimage(&imgtomorin, name, 200, 150);
		imgtomorinArray.push_back(imgtomorin);
	}
	obstacleImgs.push_back(imgtomorinArray);
	//加载帝国炮火中枢先兆者
	IMAGE imgpao;
	loadimage(&imgpao, "resource\\帝国炮火中枢先兆者.png");
	vector<IMAGE>imgpaoArray;
	imgpaoArray.push_back(imgpao);
	obstacleImgs.push_back(imgpaoArray);
	//初始化障碍物池 
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		obstacles[i].exist = false;
	}
	//加载下蹲素材
	loadimage(&imganonDown, "resource/dun.png");
	anonDown = false;
	for (int i = 0; i < 4; i++)
	{
		vector<IMAGE> imgHookArray;

		IMAGE imgH;
		sprintf(name, "resource/h%d.png", i + 1);
		loadimage(&imgH, name);

		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);
	}
	//加载开始界面
	//loadimage(0, "resource\\begin.jpg");
	//初始化血量
	heroBlood = 100;
	//加载碰撞音效
	preLoadSound("resource\\hit.mp3");
	preLoadSound("resource\\唐笑.mp3");
	//加载背景音乐
	//使用 mciSendString 的 repeat 参数时，音频结束到重新开始之间确实会有约 100ms - 500ms 的物理停顿。
	// 这是因为 Windows 需要重新读取文件、填充缓冲区并初始化解码器。
	//PlaySound 是 Windows 提供的另一个 API，它在处理 WAV 格式的循环时几乎是瞬间的，没有 MCI 那种明显的重新加载过程。
	//PlaySound(TEXT("resource\\为什么要演奏春日影.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	lastObsIndex = -1;
	score = 0;
	aha_num = 0;
	//加载数字图片
	for (int i = 0; i < 10; i++)
	{
		sprintf(name, "resource\\sz\\%d.png", i);
		loadimage(&imgSZ[i], name);
	}
	//删除  system("pause");，因为这个函数会强行切回到控制台。
}
void update()//渲染游戏背景
{
	putimagePNG2(bgx[0], 0, &imgBgs[0]);
	putimagePNG2(bgx[1], 243, &imgBgs[1]);
	putimagePNG2(bgx[2], 675, &imgBgs[2]);
}
void move()
{//平移图片坐标，实现滚动
	for (int i = 0; i < 3; i++)
	{
		bgx[i] = bgx[i] - bgspeed[i];
		if (bgx[i] < -WIN_WIDTH)
		{
			bgx[i] = 0;//循环平移
			bgx[i] = 0;//循环平移
		}
	}
	//anonIndex = (anonIndex + 1)%12;//实现奔跑效果,这行移到else里面，实现不跳跃的时候才奔跑
	//实现跳跃
	if (anonjump)
	{
		//if (anonY < Height)
		{
			v_jump = v_jump + 1;
		}
		anonY = anonY + v_jump;
		if (anonY > Height)
		{
			anonjump = false;
			v_jump = 0;
		}
	}
	else
	{
		anonIndex = (anonIndex + 1) % 12; //实现奔跑效果
	}
	//创建障碍物
	static int frameCount = 0;//因为是static，所以只会初始化一次
	static int enemyFre = 50;//牢大刷新频率
	frameCount++;
	if (frameCount > enemyFre)//刷了100帧
	{
		frameCount = 0;
		enemyFre = 50 + rand() % 50;//50...100,即50+(0...50)
		createObstacle();
	}
	//更新障碍物坐标
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist)
		{
			obstacles[i].x -= obstacles[i].speed + bgspeed[2];
			if (obstacles[i].x < -obstacleImgs[obstacles[i].type][obstacles[i].imgIndex].getwidth() * 2) {
				obstacles[i].exist = false;
			}
			int len = obstacleImgs[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;
		}
	}

	//玩家和障碍物的碰撞检测
	checkHit();
}

void keyboard()
{
	// 1. 跳跃逻辑
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (!anonjump)
		{
			jump();
		}
	}
	// 2. 下蹲逻辑：长按生效，松开恢复
	// 注意：跳跃过程中通常不允许下蹲，所以加了 !anonjump 判断
	if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) && !anonjump) {
		if (!anonDown) {
			down();
		}
	}
	else {
		if (anonDown) {
			anonDown = false;
			updating = true;
		}
	}
}
void keyboard_aha()
{
	// 1. 跳跃逻辑
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		if (!anonjump)
		{
			jump();
		}
	}
	// 2. 下蹲逻辑：长按生效，松开恢复
	// 注意：跳跃过程中通常不允许下蹲，所以加了 !anonjump 判断
	if (GetAsyncKeyState(VK_SPACE) & 0x8000&&!anonjump) {
		if (!anonDown) {
			down();
		}
	}
	else {
		if (anonDown) {
			anonDown = false;
			updating = true;
		}
	}
}
void jump()
{
	anonjump = true;
	updating = true;//在计时还未达到30ms时也刷新
	v_jump = -24;//设定跳跃初始速度
}
void down()
{
	updating = true;
	anonDown = true;
}
void updateObstacle()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist)
		{
			putimagePNG2(obstacles[i].x, obstacles[i].y, WIN_WIDTH, &obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}
}
void createObstacle()
{
	int i = 0;
	for (i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist == false)
		{
			break;
		}
	}
	if (i >= OBSTACLE_COUNT)
	{
		return;
	}
	obstacles[i].exist = true;
	obstacles[i].hited = false;
	obstacles[i].imgIndex = 0;
	obstacles[i].type = (obstacle_type)(rand() % 4);

	if (lastObsIndex >= 0 && obstacles[lastObsIndex].type >= HOOK1 && obstacles[lastObsIndex].type <= HOOK4 && obstacles[i].type == tomorin && obstacles[lastObsIndex].x > WIN_WIDTH - 500)
	{
		obstacles[i].type = kobe;
	}
	lastObsIndex = i;
	if (obstacles[i].type == HOOK1)
	{
		//obstacles[i].type = (obstacle_type)(int)(obstacles[i].type +rand() % 4);
		obstacles[i].type += rand() % 4;
	}

	obstacles[i].x = WIN_WIDTH;
	obstacles[i].y = WIN_HEIGHT * 0.95 - imganon[0].getheight() - 5;//WIN_HEIGHT*0.95 - obstaclesImgs[obstacles[i].type][0].getheight();
	if (obstacles[i].type == kobe)
	{
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == tomorin)
	{
		obstacles[i].speed = 4;
		obstacles[i].power = 10;
	}
	else if (obstacles[i].type == pao)
	{
		obstacles[i].speed = 20;
		obstacles[i].power = 20;
		obstacles[i].y = 303;
	}
	else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4)
	{
		obstacles[i].speed = 0;

		if (obstacles[i].type == HOOK2) {
			obstacles[i].power = 100;
		}
		else if (obstacles[i].type == HOOK4) {
			obstacles[i].power = 0;
		}
		else {
			obstacles[i].power = 20;
		}
		obstacles[i].y = 0;
	}
	obstacles[i].passed = false;
}
void updateHero()
{
	if (!anonDown)
	{
		putimagePNG2(anonX, anonY, &imganon[anonIndex]);
		putimagePNG2(anonX + 50, anonY, &box);
		putimagePNG2(anonX + 50, anonY - 98, &anon);
	}
	else
	{
		int y = WIN_HEIGHT * 0.95 - imganonDown.getheight();
		putimagePNG2(anonX, y, &imganonDown);
	}

}

void checkHit()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist && !obstacles[i].hited)
		{
			int a1x, a1y, a2x, a2y; // 玩家
			int b1x, b1y, b2x, b2y; // 障碍物
			int off = 30; // 判定缩进

			// 1. 计算玩家碰撞盒 
			if (anonDown) {
				a1x = anonX + off;
				// 下蹲时，头顶坐标 = 地面高度 - 下蹲素材高度
				a1y = (int)(WIN_HEIGHT * 0.95) - imganonDown.getheight() + off;
				a2x = anonX + imganonDown.getwidth() - off;
				a2y = (int)(WIN_HEIGHT * 0.95);
			}
			else {
				a1x = anonX + off + 60;
				a1y = anonY - 103;
				a2x = anonX + imganon[anonIndex].getwidth() - off;
				a2y = anonY + imganon[anonIndex].getheight();
			}

			// 2. 计算障碍物碰撞盒 
			b1x = obstacles[i].x + off;
			b1y = obstacles[i].y + off;
			b2x = obstacles[i].x + obstacleImgs[obstacles[i].type][obstacles[i].imgIndex].getwidth()-off;
			b2y = obstacles[i].y + obstacleImgs[obstacles[i].type][obstacles[i].imgIndex].getheight();

			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y))//else if 的条件包含了第一个 if 的条件。
				//当碰撞发生时，程序进入第一个 if 执行完减血后，会直接跳过所有的 else if。所以逻辑 B 永远不会被执行。
			{
				heroBlood -= obstacles[i].power;
				printf("血量剩余%d\n", heroBlood);
				playSound("resource\\hit.mp3");
				obstacles[i].hited = true;
				if (obstacles[i].type==HOOK4) {
					aha_num = (aha_num + 1) % 2;
					heroBlood =(rand() % 100)+1;
				}
					
			}
	   

			
		}
	}
}
void updateHP()
{
	drawBloodBar(20, 20, 400, 15, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}
void checkOver()
{
	if (heroBlood <= 0) {  
		FlushBatchDraw();
		PlaySound(NULL, NULL, 0);
		OverMenu();
		resetGame();
		mciSendString("stop resource\\爱音唐哭.mp3", 0, 0, 0);
		heroBlood = 100;
		score = 0;
		PlaySound(TEXT("resource\\为什么要演奏春日影.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
}
void checkScore()
{
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		if (obstacles[i].exist &&
			obstacles[i].passed == false &&
			obstacles[i].hited == false &&
			obstacles[i].x + obstacleImgs[obstacles[i].type][0].getwidth() < anonX)//赋值运算符与比较运算符混用：报错，表达式必须是可修改的左值
		{
			score += 1;
			obstacles[i].passed = true;
			playSound("resource\\唐笑.mp3");
			printf("分数：%d", score);
		}
	}
}
void updateScore()
{
	char str[8];
	sprintf(str, "%d", score);
	int x = 20, y = 65;
	for (int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth() + 5;

	}
}
void checkWin()
{
	if (score >= WIN_SCORE)
	{
		FlushBatchDraw();
		mciSendString("play resource\\爱音唐笑.mp3", 0, 0, 0);
		FlushBatchDraw();
		WINMenu();
		resetGame();
		mciSendString("stop resource\\爱音唐笑.mp3", 0, 0, 0);
		heroBlood = 100;
		score = 0;
		PlaySound(TEXT("resource\\为什么要演奏春日影.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	}
}
void startupMenu() 
{
	// 1. 绘制背景
	loadimage(0, "resource\\begin.jpg", WIN_WIDTH, WIN_HEIGHT);

	// 2. 设定按钮位置
	int btnX = (WIN_WIDTH - imgStartBtn.getwidth()) / 2; // 居中显示
	int btnY = 580;
	int btnW = imgStartBtn.getwidth();
	int btnH = imgStartBtn.getheight();

	// 3. 绘制图片按钮
	putimagePNG2(btnX, btnY, &imgStartBtn);

	// 4. 鼠标交互
	ExMessage msg;
	while (true) {
		if (peekmessage(&msg, EM_MOUSE)) {
			// 判定：左键按下 且 坐标在图片范围内
			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x >= btnX && msg.x <= btnX + btnW &&
					msg.y >= btnY && msg.y <= btnY + btnH) {
					break; // 点击成功，退出菜单进入游戏
				}
			}
		}
	}
}
void WINMenu() {
	// 1. 绘制背景
	loadimage(0, "resource\\胜利结算.jpg",WIN_WIDTH, WIN_HEIGHT);
	FlushBatchDraw();
	PlaySound(NULL, NULL, 0);
	mciSendString("play resource\\爱音唐笑.mp3 repeat", 0, 0, 0);

	// 2. 设定按钮位置
	int btnX = 1120;
	int btnY = 426;
	int btnW = imgWinBtn.getwidth();
	int btnH = imgWinBtn.getheight();

	// 3. 绘制图片按钮
	putimagePNG2(btnX, btnY, &imgWinBtn);
	FlushBatchDraw();
	// 4. 鼠标交互
	ExMessage msg;
	while (true) {
		if (peekmessage(&msg, EM_MOUSE)) {
			// 判定：左键按下 且 坐标在图片范围内
			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x >= btnX && msg.x <= btnX + btnW &&
					msg.y >= btnY && msg.y <= btnY + btnH) {
					break; // 点击成功，退出菜单进入游戏
				}
			}
		}
	}
}
void OverMenu() {
	// 1. 绘制背景
	loadimage(0, "resource\\战败CG.jpg", WIN_WIDTH, WIN_HEIGHT);
	FlushBatchDraw();
	mciSendString("play resource\\爱音唐哭.mp3 repeat", 0, 0, 0);

	// 2. 设定按钮位置
	int btnX = 300; // 居中显示
	int btnY = 150;
	int btnW = imgOverBtn.getwidth();
	int btnH = imgOverBtn.getheight();

	// 3. 绘制图片按钮
	putimagePNG2(btnX, btnY, &imgOverBtn);

	// 4. 鼠标交互
	ExMessage msg;
	while (true) {
		if (peekmessage(&msg, EM_MOUSE)) {
			// 判定：左键按下 且 坐标在图片范围内
			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x >= btnX && msg.x <= btnX + btnW &&
					msg.y >= btnY && msg.y <= btnY + btnH) {
					break; // 点击成功，退出菜单进入游戏
				}
			}
		}
	}
}
void check_aha()
{
	if (aha_num == 0) {
		keyboard();
	}
	else {
		keyboard_aha();
	}
}





// 线性内插实现淡入淡出 
void drawImageWithAlpha(IMAGE* img, int x, int y, int alpha) {
	// alpha 范围 0 - 255
	BLENDFUNCTION f = { AC_SRC_OVER, 0, (BYTE)alpha, AC_SRC_ALPHA };
	// 使用 Windows 原生 AlphaBlend 函数实现全局透明度
	HDC dstDC = GetImageHDC(NULL);
	HDC srcDC = GetImageHDC(img);
	AlphaBlend(dstDC, x, y, img->getwidth(), img->getheight(),
		srcDC, 0, 0, img->getwidth(), img->getheight(), f);
}

void playGenshinStyleOpening() {
	IMAGE logos[2];
	loadimage(&logos[0], "resource\\启动1.jpg", WIN_WIDTH, WIN_HEIGHT);
	loadimage(&logos[1], "resource\\启动2.jpg", WIN_WIDTH, WIN_HEIGHT);

	for (int i = 0; i < 2; i++) {
		// 淡入
		for (int a = 0; a <= 255; a += 5) {
			cleardevice();
			drawImageWithAlpha(&logos[i], 0, 0, a);
			FlushBatchDraw();
			Sleep(20);
		}
		Sleep(1000); // 停留

		// 淡出
		for (int a = 255; a >= 0; a -= 5) {
			cleardevice();
			drawImageWithAlpha(&logos[i], 0, 0, a);
			FlushBatchDraw();
			Sleep(20);
		}
	}
}

void resetGame() {
	// 1. 重置角色状态
	anonY = WIN_HEIGHT * 0.95 - imganon[0].getheight();
	anonIndex = 0;
	anonjump = false;
	anonDown = false;
	v_jump = 0;

	// 2. 重置背景位置
	for (int i = 0; i < 3; i++) {
		bgx[i] = 0;
	}

	// 3. 清空所有障碍物 (最重要的一步)
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
		obstacles[i].passed = false;
		obstacles[i].hited = false;
	}

	// 4. 重置数值
	heroBlood = 100;
	score = 0;
	aha_num = 0;
	lastObsIndex = -1;

	// 5. 确保立即刷新一次画面
	updating = true;
}