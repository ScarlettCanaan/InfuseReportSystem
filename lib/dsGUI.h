/*******************************************
*
*  File: guiUser.h
*  Purpose：gui api include file
*
********************************************/


#ifndef _GUI_USER_H
#define _GUI_USER_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

typedef unsigned long WM_COLOR;

// 在当前页面上增加一个图形
#define IMAGE_ATTRIBUTE_HASTRANS  0x00000001   // 透明处理

typedef struct
{
    long xSize;  // 图片长的相素点数量
    long ySize;  // 图片宽的相素点数量   
    unsigned long attribute;  //bit0: 是否透明处理
    WM_COLOR filterColor;          //透明色
    // 图片的内容数据，存放方式为逐行从上到下,每行从左到右。
    // 每个相素点占用3个字节共24BIT，分别为R(8) G(8) B(8)。
    const unsigned char * data;
}WM_IMAGE;

typedef struct
{
    int fontId;       // 显示字符串时用的字体
    WM_COLOR color;   // 显示字符串时用的颜色
    const char * str;  // 字符串内容
}WM_STRING;

// ============= 系统管理接口 ==========================

// 对GUI进行初始化
// xSize、ySize：GUI画面的尺寸（单位为相素）
// memoryAddr、memorySize：GUI内部使用的动态内存。
//        具体大小与画面尺寸、页面及控件的多少相关。开始可以设的大一些，如xSize*ySize*4字节。
//        在调试时用WM_MemUsedMax函数监控实际使用量，调整到一个合理的大小，节省宝贵的内存空间。
void WM_Init(int xSize, int ySize,unsigned long  memoryAddr,unsigned long  memorySize);

// 获取GUI使用动态内存情况 
//usedSize    当前使用大小 
//peakSize    使用峰值
void WM_MemUsedGet(unsigned long * usedSize, unsigned long * peakSize);

// 获取当前GUI的版本信息
const char *WM_GetVerString(void);

void WM_Print(char * string);

// // 设定显存。显存中存放的是需要立刻显示到画面上的数据。
// // 这个函数由GUI调用，需要用户编程实现，调用驱动函数将画面数据显示到显示屏上。
// #if GUI_SHOW_TYPE == GUI_TYPE_RGB888
// // 显示RGB888模式的一个画面
// //    每个点对应一个长字（LONG），实际只有前24位有效  R(8) G(8) B(8)
// //    整个画面数据量固定为X*Y个长字  
// void WM_DisplayMemorySet(unsigned long* ptr);
// #elif GUI_SHOW_TYPE == GUI_TYPE_RGB565
// // 显示RGB565模式的一个画面
// //    每个点对应一个短字（LONG），16位  R(5) G(6) B(5)
// //    整个画面数据量固定为X*Y个短字  
// void WM_DisplayMemorySet(unsigned short* ptr);
// #endif

void WM_Paint(unsigned char * pageBaseAddr, int x, int y, int xSize, int ySize);

// GUI缺省加载了英文12号和16号字体，其它字体需要在GUI时动态加载

// 加载字库，字库文件由鼎尚信息提供，目前提供英文12号、16号、24号、32号、64号，中文12号、16号、24号、32号、64号
// fontSize：字体大小，如16号字体，则为16。
// fontData：字库文件位置
// 返回：成功返回字库ID，用户根据字库ID调用字库。失败返回-1。
int WM_FontLoad(unsigned short fontSize,const unsigned char * fontData);

// GUI缺省加载的英文12号和16号字体ID，用户可根据ID直接使用相应字库
#define GUI_FONT_ENG_16    0x000010   // 缺省加载的16号英文字符

// 定位设备（触摸屏、鼠标等）输入接口，需每100ms调用一次
// x,y：当前定位设备位置
// state：当前定位设备状态（1：按下  0：弹起）
void WM_HidState(int x,int y,int state);

// ============= 页面管理接口 ==========================
// 页面是GUI的基本管理单元，所有的控件必须承载在页面上。
// 页面从下到上最多可以有8层。
//     可简单新增页面，则新页面覆盖老页面，页面多一层，待新页面删除后会自动恢复老页面。
//     也可先删除老页面，再创建新页面，则新页面替换老页面，页面层数不变。

// 控件事件类型
#define WM_MSG_TYPE_CLICK            0   // 按下事件
#define WM_MSG_TYPE_RELEASE          1   // 释放事件
#define WM_MSG_TYPE_SELECT           2   // 选择事件    
#define WM_MSG_TYPE_VALUE            3   // 数值改变事件
#define WM_MSG_TYPE_FOCUS            4   // 获得焦点事件
#define WM_MSG_TYPE_INIT             5   // 控件初始化事件


// 错误类型
#define WM_ERROR_TYPE_MEM        -1     // 内存申请失败  GUI分配内存不够      
#define WM_ERROR_TYPE_PAGE       -2     // 创建页面数目超过限制
#define WM_ERROR_TYPE_CTL        -3     // 创建控件数目超过限制
#define WM_ERROR_TYPE_SIZE       -4     // 创建控件位置大小不合法
#define WM_ERROR_TYPE_PARA       -5     // 获得/设置 变量参数错误
#define WM_ERROR_TYPE_NO_PAGE    -6     // 没有创建过页面


// 控件触摸事件的回调处理函数
typedef void (* WM_PAGE_CTL_CB)(int ctlHandle, int msgType);

// 创建一个新页，没有图形填0，没有回调填0
//返回 0 成功  非零 错误码
int WM_PageNew(WM_COLOR bkColor, WM_IMAGE * bkImage, WM_PAGE_CTL_CB pageCb);

// 页面空白区域触摸事件的回调处理函数
// 返回：当前点击在界面空白区域的坐标参数
typedef void (* WM_PAGE_SPACE_CB)(int msgType, int x, int y);

// 向当前页面添加空白区域的处理回调函数
int WM_SpaceCbAdd(WM_PAGE_SPACE_CB spaceCb);

// 激活页面。页面激活后页面内容才会在屏上显示。
void WM_PageActive(void);

// 删除页面。当前页面删除后，下层页面会自动显示。
void WM_PageDel(void);

// 删除控件。
int WM_CtlDelete(int handle);

// ============= 静态控件 ==========================
// 静态控件没有控件ID，不采集用户输入，只对页面进行静态修饰。

// 在当前页面上打印字符串
void WM_StringDisp(int x, int y, WM_STRING * string);

//在当前页面增加一个图像
void WM_ImageAdd(int x, int y, WM_IMAGE * image, unsigned long attribute);

// 在当前页面上增加一个包罗框 
void WM_GroupAdd(int x, int y, int xSize, int ySize, WM_STRING * string);

// 在当前页面上使用指定颜色清除区域
void WM_ClearByColor(int x, int y, int xSize, int ySize, WM_COLOR color);

// ============= 动态控件 ==========================
// 动态控件有控件ID，主要用来采集用户输入。

// ~~~~~~~~~~~~~ 按钮控件 ~~~~~~~~~~~~~~~~~~~~~~~~

// 在当前页面上增加一个按钮	  创建成功返回按键句柄
// 返回控件ID  
int WM_ButtonAdd(int x , int y, int xSize, int ySize);

// 设置按钮背景色
void WM_ButtonSetBkColor(int handle, WM_COLOR bkColor);

// 设置按钮字符
void WM_ButtonSetText(int handle, WM_STRING * string);

// 设置按钮背景图
void WM_ButtonSetImage(int handle, WM_IMAGE * image);

//设置边框 on：1 : 开  0: 关
void WM_ButtonSetBorder(int handle, int on);

//设置透明 on：1 : 开  0: 关
void WM_ButtonSetTransparent(int handle, int on);

// 按钮响应事件类型
//  WM_MSG_TYPE_CLICK / WM_MSG_TYPE_RELEASE

// ~~~~~~~~~~~~~ 复选框控件 ~~~~~~~~~~~~~~~~~~~~~~~~

// 在当前页面上增加一个复选框  defaultState 1：缺省状态为选中  0：缺省状态为未选中
int WM_CheckBoxAdd(int x, int y, int xSize, int ySize, WM_STRING * string, int defaultState);

// 设置复选框的状态  1：选择   0：未选择
void WM_CheckBoxSet(int handle, int state);

// 获取复选框的状态
//   返回 1：选择   0：未选择
int WM_CheckBoxGet(int handle);

// 复选框响应事件类型
// WM_MSG_TYPE_RELEASE

// ~~~~~~~~~~~~~ 列表框控件 ~~~~~~~~~~~~~~~~~~~~~~~~

// 在当前页面上增加一个列表框
int WM_ListBoxAdd(int x, int y, int xSize, int ySize, WM_COLOR bkColor, WM_COLOR textColor, int fontId);

// 在列表框中增加一个表项
void WM_ListBoxItemAdd(int handle, const char * str);

// 在列表框中删除一个表项
void WM_ListBoxItemDel(int handle, int itemIndex);

// 设置列表框中的选中项
void WM_ListBoxSet(int handle, int itemIndex);

// 得到列表框中的选中项(没有选中，返回-1)，*pStr返回选中项的字符串
int WM_ListBoxGet(int handle);

// 列表框响应事件类型
// WM_MSG_TYPE_SELECTED_CHANGED

// ~~~~~~~~~~~~~ 单选框控件 ~~~~~~~~~~~~~~~~~~~~~~~~

// 在当前页面上增加一个单选框
int WM_RadioAdd(int x, int y, int xSize, int ySize, WM_COLOR textColor, int fontId);

// 在单选框中增加一个表项
void WM_RadioItemAdd(int handle, const unsigned char * str);

// 设置单选框中的选中项
void WM_RadioSet(int handle, int itemIndex);

// 得到单选框中的选中项(没有选中，返回-1)
int WM_RadioGet(int handle);

// 单选框响应事件类型
// WM_MSG_TYPE_SELECTED_CHANGED

// ~~~~~~~~~~~~~ 滑动条控件 ~~~~~~~~~~~~~~~~~~~~~~~~
#define SLIDER_MODE_SLIDER   0    //滑动条
#define SLIDER_MODE_LOCK     1    //滑动锁
// 在当前页面上增加一个滑动条，回调返回滑动时的当前刻度
int WM_SliderAdd(int x, int y, int xSize, int ySize, int min, int max);

void WM_SliderSetMode(int handle, int mode);   //默认是滑动条

// 在滑动条设置当前值
void WM_SliderSet(int handle, int value);

// 在滑动条查询当前值
int WM_SliderGet(int handle);

//设置滑动条的默认背景,如果滑动条左右不一样需要设置滑动条左边的图片，宽度为1，以及圆角的宽度
void WM_sliderSetBkImage(int handle, WM_IMAGE * bkImage, WM_IMAGE * leftImage, unsigned char fillet);

//设置滑动条对应的图片
void WM_sliderSetBarImage(int handle, WM_IMAGE * barImage);

// 滑动条响应事件类型
// WM_MSG_TYPE_VALUE_CHANGED

// ~~~~~~~~~~~~~ 进度条控件 ~~~~~~~~~~~~~~~~~~~~~~~~

// 在当前页面上增加一个进度条
int WM_ProgBarAdd(int x, int y, int xSize, int ySize, WM_COLOR leftColor, WM_COLOR rightColor);

// 设置进度条
int WM_ProgBarSet(int handle, int value);

// 进度条无需响应外部输入事件

// ~~~~~~~~~~~~~ 编辑框控件 ~~~~~~~~~~~~~~~~~~~~~~~~

//  在当前页面上增加一个编辑框
//  参数 attribute：BIT 0 ：靠右对齐    1 ：靠左对齐
int WM_EditAdd(int x, int y, int xSize, int ySize, WM_COLOR bkColor, WM_COLOR textColor, int fontId,unsigned char attribute);

// 将字符串设置到编辑框中
void WM_EditSet(int handle, const char * str);

// 从编辑框查询得到当前输入值
const char * WM_EditGet(int handle);

void WM_EditSetBkColor(int handle, WM_COLOR bkColor);

// 编辑框响应事件类型
// WM_MSG_TYPE_FOCUS

// ~~~~~~~~~~~~~ 文本框控件 ~~~~~~~~~~~~~~~~~~~~~~~~

//  在当前页面上增加一个文本框
//  参数 attribute：BIT 0-1 ：0:靠右对齐 1 ：靠左对齐   2: 居中对齐 , bit 2: 1:透明 0:不透明
int WM_LabelAdd(int x, int y, int xSize, int ySize, WM_COLOR bkColor, WM_COLOR textColor, int fontId ,unsigned char attribute);

// 将字符串设置到文本框中
void WM_LabelSet(int handle, const char * str);

void WM_LabelSetBkColor(int handle, WM_COLOR bkColor);

void WM_LabelSetFrontColor(int handle, WM_COLOR frontColor);

// 文本框无需响应外部输入事件

// ~~~~~~~~~~~~~ 下拉框控件 ~~~~~~~~~~~~~~~~~~~~~~~~

// 在当前页面上增加一个下拉框
int WM_DropDownAdd(int x, int y, int xSize, int ySize, WM_COLOR bkColor, WM_COLOR textColor, int fontId);

// 在下拉框中增加一个表项
void WM_DropDownItemAdd(int handle, const char * str);

// 在下拉框中删除一个表项
void WM_DropDownItemDel(int handle, int itemIndex);

// 设置下拉框中的选中项
void WM_DropDownSet(int handle, int itemIndex);

// 得到下拉框中的选中项(没有选中，返回-1)，*pStr返回选中项的字符串
int WM_DropDownGet(int handle);

// 下拉框响应事件类型
// WM_MSG_TYPE_SELECTED_CHANGED


// 额外新增控件

// ~~~~~~~~~~~~~~~ 表格控件 ~~~~~~~~~~~~~~~~~~~~~~~
// 创建表格控件
//int WM_TableAdd(int x , int y, int xSize, int ySize);


// 在表格控件中添加一列 (表头)
//void WM_TableColumnAdd(int handle, int columnIndex, int columnWidth, WM_STRING *str);

// 在表格控件中添加一行
//void WM_TableRowAdd(int handle, int rowIndex, int rowHigh);

// 设置表格控件的字符串内容
//void WM_TableItemSet(int handle, int rowIndex, int columnIndex, WM_STRING * str);

//  ~~~~~~~~~~~~~~~~ 消息框 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// 消息框创建返回控件句柄  消息框用户点击返回 WM_MSG_TYPE_SELECT消息
//captionStr  ：消息框文本
//button0Str  : 第一个按钮的文本
//button1Str  : 第二个按钮的文本
//button2Str  : 第三个按钮的文本                 
// 返回值：消息框的控件句柄
int WM_MsgBoxAdd(int x, int y, int xSize, int ySize, WM_STRING * captionStr, WM_STRING * button0Str, WM_STRING * button1Str, WM_STRING * button2Str);

// 获取消息框被按下的键序号
// 返回值：  0：button0
//           1: button1
//           2: button2
int WM_MsgBoxGet(int handle);

#define WM_COLOR_BLACK        0x00000000  // 黑色
#define WM_COLOR_WHITE        0x00FFFFFF  // 白色
#define WM_COLOR_GRAY         0x007F7F7F  // 灰色（50%灰度）
#define WM_COLOR_DARK_GRAY    0x003F3F3F  // 深灰色（75%灰度）
#define WM_COLOR_LIGHT_GRAY   0x00C3C3C3  // 浅灰色（25%灰度）
#define WM_COLOR_RED          0x00FF0000  // 红色
#define WM_COLOR_DARK_RED     0x007F0000  // 深红色（酒红色）
#define WM_COLOR_LIGHT_RED    0x00FF7F7F  // 浅红色（粉红色）
#define WM_COLOR_GREEN        0x0000FF00  // 绿色
#define WM_COLOR_DARK_GREEN   0x00007F00  // 深绿色
#define WM_COLOR_LIGHT_GREEN  0x007FFF7F  // 浅绿色
#define WM_COLOR_BLUE         0x000000FF  // 蓝色
#define WM_COLOR_DARK_BLUE    0x0000007F  // 深蓝色（海蓝色）
#define WM_COLOR_LIGHT        0x007F7FFF  // 浅蓝色（天蓝色）
#define WM_COLOR_YELLOW       0x00FFFF00  // 黄色
#define WM_COLOR_DART_YELLOW  0x007F7F00  // 深黄色（橄榄色）
#define WM_COLOR_LIGHT_YELLOW 0x00FFFF7F  // 浅黄色
#define WM_COLOR_PURPLE       0x00FF00FF  // 紫色
#define WM_COLOR_DART_PURPLE  0x007F007F  // 深紫色
#define WM_COLOR_LIGHT_PURPLE 0x00FF7FFF  // 浅紫色
#define WM_COLOR_CYAN         0x0000FFFF  // 青色
#define WM_COLOR_DARK_CYAN    0x00007F7F  // 深青色
#define WM_COLOR_LIGHT_CYAN   0x007FFFFF  // 浅青色（天青色）
#define WM_COLOR_ORANGE       0x00FF7F00  // 橙色
#define WM_COLOR_GOLD         0x00FFD700  // 金色

// 功能：创建键盘
// 参数：kbType  键盘类型 1：数字键盘   2：数字/英文
//			 fontId  字体大小格式	
//       str     初始内容
//       handle  触发键盘的控件句柄（编辑框）
//int WM_KeyBoardAdd(unsigned char kbType, int fontId, char * str);

// 获取当前键盘上面编辑的数值
//char * WM_KeyBoardGetVaule(int handle);

//绘图函数

//设置画笔
void WM_PenSet(WM_COLOR color, int size);

//画点
void WM_PainPoint(int x, int y);

//画线
void WM_DrawLine(int x0, int y0, int x1, int y1);

//填色
void WM_FillColor(int x, int y, WM_COLOR color);

//画圆
void WM_DrawCircle(int x0, int y0, int r);

//重绘
void WM_Redraw(void);



#if defined(__cplusplus)
}
#endif 


#endif
