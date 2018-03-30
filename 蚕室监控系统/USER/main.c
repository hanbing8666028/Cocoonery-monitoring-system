

#include "main.h"


typedef enum sensor_num
{
    Multi_function_sensor   = 1,
    Wind_sensor             = 2,
    Noise_sensor            = 3
}enum_sensor_num_t;

u8 baojing_down_flag = 0;
u8 warn_happen_flag  = 0;
u8 beep_count = 0;

u8 flash_data_save[100];

u16 wendu_alarm_up = 30;
u16 wendu_alarm_down = 10;


u16 shidu_alarm_up = 90;
u16 shidu_alarm_down = 10;

u16 fengsu_alarm_up = 30;
u16 fengsu_alarm_down = 0;

u16 guangzhao_alarm_up  = 50;
u16 guangzhao_alarm_down = 20;

u16 co2_alarm_up = 50;
u16 co2_alarm_down = 0;

u16 zaoshen_alarm_up = 90;
u16 zaoshen_alarm_down = 0;


u16 wendu_vaule  = 0;
u16 shidu_vaule  = 0;
u16 fengsu_vaule  = 0;
u16 guangzhao_vaule  = 0;
u16 co2_vaule  = 0;
u16 zaoshen_vaule  = 0;


u16 co2_vaule_bak;
u16 wendu_vaule_bak;
u16 shidu_vaule_bak;
u16 guangzhao_vaule_bak;
u16 fengsu_vaule_bak;
u16 zaoshen_vaule_bak;


u8 ask_data_count = 0;


typedef enum warn_table_t
{
    CO2_SHANG   = 0,
    CO2_XIA,
    WENDU_SHANG,
    WENDU_XIA,
    SHIDU_SHANG,
    SHIDU_XIA,
    GUANGZHAO_SHANG,
    GUANGZHAO_XIA,
    FENGSU_SHANG,
    FENGSU_XIA,
    ZAOSHEN_SHANG,
    ZAOSHEN_XIA
}enum_warn_table_t;

u16 warn_table[12] = {1000,0,30,0,90,0,100,0,50,0,100,0};
u8  warn_num = 0;
u8  warn_old_num = 0;


void lcd_show_init(void);
void write_save_data(void);

////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
    if(len==0)return;
    LCD_Fill(x0,y0,x0+len-1,y0 + 1,color);
}

void gui_draw_sline(u16 x0,u16 y0,u16 len,u16 color)
{
    if(len==0)return;
    LCD_Fill(x0,y0,x0 + 1,y0+len-1,color);
}


//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{
    u32 i;
    u32 imax = ((u32)r*707)/1000+1;
    u32 sqmax = (u32)r*(u32)r+(u32)r/2;
    u32 x=r;
    gui_draw_hline(x0-r,y0,2*r,color);
    for (i=1;i<=imax;i++)
    {
        if ((i*i+x*x)>sqmax)// draw lines from outside
        {
            if (x>imax)
            {
                gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
                gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
            }
            x--;
        }
        // draw lines from inside (center)
        gui_draw_hline(x0-x,y0+i,2*x,color);
        gui_draw_hline(x0-x,y0-i,2*x,color);
    }
}
//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{

    u16 x,y;

    warn_num =WENDU_SHANG;
    LCD_Clear(WHITE);   //清屏

    /* 报警设定 */
    x = 150;
    y = 10;
    POINT_COLOR=BROWN;//设置字体色
    LCD_ShowHZ(x,y,hanzi_baojing1,288);
    LCD_ShowHZ(x + 50,y,hanzi_baojing2,288);
    LCD_ShowHZ(x + 100,y,hanzi_sheding1,288);
    LCD_ShowHZ(x + 150,y,hanzi_sheding2,288);
    /* 显示横线 */
    gui_draw_hline(0,60,480,MAGENTA);

    /* 退出设定 */
    x = 150;
    y = 750;
    LCD_Fill(150,y,350,y + 50,BLACK);
    POINT_COLOR=BROWN;//设置字体色
    LCD_ShowHZ(x,y,hanzi_tui,288);
    LCD_ShowHZ(x + 50,y,hanzi_chu,288);
    LCD_ShowHZ(x + 100,y,hanzi_sheding1,288);
    LCD_ShowHZ(x + 150,y,hanzi_sheding2,288);


    /* 项目 */
    x = 0;
    y = 60;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_xiang,288);
    LCD_ShowHZ(x + 50,y,hanzi_mu,288);

    /* 显示横线 */
    gui_draw_hline(0,111,480,MAGENTA);
    gui_draw_hline(0,113,480,MAGENTA);

    /* 显示竖线 */
    gui_draw_sline(100,60,372,MAGENTA);
    /* 显示竖线 */
    gui_draw_sline(295,60,372,MAGENTA);


    /* 上限 */
    x = 100;
    y = 60;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_shang,288);
    LCD_ShowHZ(x + 50,y,hanzi_xian,288);

    /* 下限 */
    x = 300;
    y = 60;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_xia,288);
    LCD_ShowHZ(x + 50,y,hanzi_xian,288);

    /* 温度 */
    x = 0;
    y = WENDUSHANG_BUTTON_Y_MIN_COORDINATE;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_wen,288);
    LCD_ShowHZ(x + 50,y,hanzi_du,288);
    /* 温度上限 */
    POINT_COLOR=RED;//设置字体色
    x = WENDUSHANG_BUTTON_X_MIN_COORDINATE;
    y = WENDUSHANG_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[WENDU_SHANG]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[WENDU_SHANG]%10],288);
    /* 温度下限 */
    POINT_COLOR=BLACK;//设置字体色
    x = WENDUXIA_BUTTON_X_MIN_COORDINATE;
    y = WENDUXIA_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[WENDU_XIA]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[WENDU_XIA]%10],288);
    /* 显示横线 */
    gui_draw_hline(0,y + 51,480,MAGENTA);


    /* 湿度 */
    x = 0;
    y = SHIDUSHANG_BUTTON_Y_MIN_COORDINATE;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_shi,288);
    LCD_ShowHZ(x + 50,y,hanzi_du,288);
    /* 湿度上限 */
    x = SHIDUSHANG_BUTTON_X_MIN_COORDINATE;
    y = SHIDUSHANG_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[SHIDU_SHANG]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[SHIDU_SHANG]%10],288);
    /* 湿度下限 */
    x = SHIDUXIA_BUTTON_X_MIN_COORDINATE;
    y = SHIDUXIA_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[SHIDU_XIA]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[SHIDU_XIA]%10],288);
    /* 显示横线 */
    gui_draw_hline(0,y + 51,480,MAGENTA);

    /* 光照 */
    x = 0;
    y = GUANGZHAOSHANG_BUTTON_Y_MIN_COORDINATE;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_guang,288);
    LCD_ShowHZ(x + 50,y,hanzi_zhao,288);
    /* 光照上限 */
    x = GUANGZHAOSHANG_BUTTON_X_MIN_COORDINATE;
    y = GUANGZHAOSHANG_BUTTON_Y_MIN_COORDINATE;

    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]/100],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]/10%10],288);
    LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]%10],288);
    /* 光照下限 */
    x = GUANGZHAOXIA_BUTTON_X_MIN_COORDINATE;
    y = GUANGZHAOXIA_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]/100],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]/10%10],288);
    LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]%10],288);
    /* 显示横线 */
    gui_draw_hline(0,y + 51,480,MAGENTA);


    /* 风速 */
    x = 0;
    y = FENGSUSHANG_BUTTON_X_MAX_COORDINATE;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_feng,288);
    LCD_ShowHZ(x + 50,y,hanzi_su,288);
    /* 风速上限 */
    x = FENGSUSHANG_BUTTON_X_MIN_COORDINATE;
    y = FENGSUSHANG_BUTTON_X_MAX_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[FENGSU_SHANG]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[FENGSU_SHANG]%10],288);
    /* 风速下限 */
    x = FENGSUXIA_BUTTON_X_MIN_COORDINATE;
    y = FENGSUXIA_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[FENGSU_XIA]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[FENGSU_XIA]%10],288);
    /* 显示横线 */
    gui_draw_hline(0,y + 51,480,MAGENTA);


    /* 噪声 */
    x = 0;
    y = ZAOSHENSHANG_BUTTON_Y_MIN_COORDINATE;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_zao,288);
    LCD_ShowHZ(x + 50,y,hanzi_shen,288);

    /* 噪声上限 */
    x = ZAOSHENSHANG_BUTTON_X_MIN_COORDINATE;
    y = ZAOSHENSHANG_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[ZAOSHEN_SHANG]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[ZAOSHEN_SHANG]%10],288);
    /* 噪声下限 */
    x = ZAOSHENXIA_BUTTON_X_MIN_COORDINATE;
    y = ZAOSHENXIA_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[ZAOSHEN_XIA]/10%10],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[ZAOSHEN_XIA]%10],288);
    /* 显示横线 */
    gui_draw_hline(0,y + 51,480,MAGENTA);

    /* CO2 */
    x = 0;
    y = CO2SHANG_BUTTON_Y_MIN_COORDINATE;
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_CO,288);
    LCD_ShowHZ(x + 50,y,hanzi_CO2,288);
    /* CO2上限 */
    x = CO2SHANG_BUTTON_X_MIN_COORDINATE;
    y = CO2SHANG_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[CO2_SHANG]/1000],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[CO2_SHANG]/100%10],288);
    LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[CO2_SHANG]/10%10],288);
    LCD_ShowHZ(x + 90,y,hanzi_shuzi[warn_table[CO2_SHANG]%10],288);
    /* CO2下限 */
    x = CO2XIA_BUTTON_X_MIN_COORDINATE;
    y = CO2XIA_BUTTON_Y_MIN_COORDINATE;
    LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[CO2_XIA]/1000],288);
    LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[CO2_XIA]/100%10],288);
    LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[CO2_XIA]/10%10],288);
    LCD_ShowHZ(x + 90,y,hanzi_shuzi[warn_table[CO2_XIA]%10],288);
    /* 显示横线 */
    gui_draw_hline(0,y + 51,480,MAGENTA);

    /* + */
    x = 100;
    y = 500;
    gui_fill_circle(JIAHAO_BUTTON_X_MIN_COORDINATE + 50, JIAHAO_BUTTON_Y_MIN_COORDINATE + 50, 50, RED);
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_jiahao,288);

    /* - */
    x = 350;
    y = 500;
    gui_fill_circle(JIANHAO_BUTTON_X_MIN_COORDINATE + 50 , JIANHAO_BUTTON_Y_MIN_COORDINATE + 50, 50, GRED);
    POINT_COLOR=BLACK;//设置字体色
    LCD_ShowHZ(x,y,hanzi_jianhao,288);




}

//两个数之差的绝对值
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{
    if(x1>x2)return x1-x2;
    else return x2-x1;
}
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
    u16 t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    if(x1<size|| x2<size||y1<size|| y2<size)return;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if(delta_x==0)incx=0;//垂直线
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if(delta_y==0)incy=0;//水平线
    else{incy=-1;delta_y=-delta_y;}
    if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0;t<=distance+1;t++ )//画线输出
    {
        gui_fill_circle(uRow,uCol,size,color);//画点
        xerr+=delta_x ;
        yerr+=delta_y ;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
 //5个触控点的颜色
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};
//电阻触摸屏测试函数
void rtp_test(void)
{
    u8 key;
    u8 i=0;
    while(1)
    {
        key=KEY_Scan(0);
        tp_dev.scan(0);
        if(tp_dev.sta&TP_PRES_DOWN)         //触摸屏被按下
        {
            if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
            {
                if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//清除
                else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);        //画图
            }
        }else delay_ms(10); //没有按键按下的时候
        if(key==KEY0_PRES)  //KEY0按下,则执行校准程序
        {
            LCD_Clear(WHITE);//清屏
            TP_Adjust();    //屏幕校准
            Load_Drow_Dialog();
        }
        i++;
        if(i%20==0)LED0=!LED0;
    }
}
//电容触摸屏测试函数
void ctp_test(void)
{
    u8 t=0;
    u8 i=0;
    u16 lastpos[5][2];      //记录最后一次的数据
    while(1)
    {
        tp_dev.scan(0);
        for(t=0;t<CT_MAX_TOUCH;t++)
        {
            if((tp_dev.sta)&(1<<t))
            {
                if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
                {
                    if(lastpos[t][0]==0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
                    lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
                    lastpos[t][0]=tp_dev.x[t];
                    lastpos[t][1]=tp_dev.y[t];
                    if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
                    {
                        Load_Drow_Dialog();//清除
                    }
                }
            }else lastpos[t][0]=0XFFFF;
        }

        delay_ms(5);i++;
        if(i%20==0)LED0=!LED0;
    }
}

void Title_Show(u16 x, u16 y)
{
    POINT_COLOR=RED;//设置字体为红色
    LCD_ShowHZ(x+0,  y,hanzi_can,  288);
    LCD_ShowHZ(x+50, y,hanzi_shiT,  288);
    LCD_ShowHZ(x+100,y,hanzi_huan, 288);
    LCD_ShowHZ(x+150,y,hanzi_jing, 288);
    LCD_ShowHZ(x+200,y,hanzi_jian, 288);
    LCD_ShowHZ(x+250,y,hanzi_kong, 288);
    LCD_ShowHZ(x+300,y,hanzi_xi,   288);
    LCD_ShowHZ(x+350,y,hanzi_tong, 288);
    /* 显示横线 */
    gui_draw_hline(0,50,480,MAGENTA);

//    gui_draw_hline(0,390,480,MAGENTA);
//    gui_draw_hline(0,391,480,MAGENTA);

    /* 显示单位 */
    /*四川农业机械研究院*/
    LCD_ShowHZ(100 , 750,hanzi_sichuan,144);
    LCD_ShowHZ(150 , 750,hanzi_shengnong,144);
    LCD_ShowHZ(200 , 750,hanzi_yeji,144);
    LCD_ShowHZ(250 , 750,hanzi_xieyan,144);
    LCD_ShowHZ(300 , 750,hanzi_jiushe,144);
    LCD_ShowHZ(350 , 750,hanzi_jiyuan,144);
}

void sensor_status_show(u8 num)
{
    if(num == 0)
    {
            // 传感器失效
        POINT_COLOR=RED;//设置字体色
        LCD_ShowHZ(30 + 100 , 600,hanzi_chuangan,288);
        LCD_ShowHZ(80 + 100  , 600,hanzi_gan,288);
        LCD_ShowHZ(130 + 100 , 600,hanzi_qi,288);
        LCD_ShowHZ(180 + 100 , 600,hanzi_yi,288);
        LCD_ShowHZ(230 + 100 , 600,hanzi_chang,288);
    }
    else
    {
        LCD_Fill(130,650,380,600 + 50,WHITE);
    }

}


void wendu_Show(u16 temp, u16 x, u16 y)
{

    POINT_COLOR=BLUE;//设置字体色
    LCD_ShowHZ(x, y, hanzi_wen, 288);
    LCD_ShowHZ(x + 50, y, hanzi_du,288);
    LCD_ShowHZ(x + 100, y, hanzi_maohao,288);

    /* 报警设置 */
    if((temp >= warn_table[WENDU_SHANG] * 100) || (temp <= warn_table[WENDU_XIA] * 100))
    {
        POINT_COLOR=RED;//设置字体色
        warn_happen_flag = 1;
    }
    else
    {
        POINT_COLOR=BLUE;//设置字体色
    }
    LCD_Fill(x + 150,y,310,y + 50,WHITE);

    LCD_ShowHZ(x + 150,y,hanzi_shuzi[temp/1000],288);
    LCD_ShowHZ(x + 180,y,hanzi_shuzi[temp/100%10],288);
    LCD_ShowHZ(x + 210,y,hanzi_duhaodian,288);
    LCD_ShowHZ(x + 230,y,hanzi_shuzi[temp/10%10],288);
    LCD_ShowHZ(x + 260,y,hanzi_shuzi[temp%10],288);
    LCD_ShowHZ(x + 280,y,hanzi_duhao,288);
    LCD_ShowHZ(x + 300,y,hanzi_duhaoC,288);

}
void shidu_Show(u16 temp, u16 x, u16 y)
{
    POINT_COLOR=BLUE;//设置字体色
    LCD_ShowHZ(x,y,hanzi_shi,288);
    LCD_ShowHZ(x + 50,y,hanzi_du,288);
    LCD_ShowHZ(x + 100,y,hanzi_maohao,288);

    if((temp >= warn_table[SHIDU_SHANG] * 100) || (temp <= warn_table[SHIDU_XIA] * 100))
    {
        POINT_COLOR=RED;//设置字体色
        warn_happen_flag = 1;
    }
    else
    {
        POINT_COLOR=BLUE;//设置字体色
    }
    LCD_Fill(x + 150,y,310,y + 50,WHITE);
    LCD_ShowHZ(x + 150,y,hanzi_shuzi[temp/1000],288);
    LCD_ShowHZ(x + 180,y,hanzi_shuzi[temp/100%10],288);
    LCD_ShowHZ(x + 210,y,hanzi_duhaodian,288);
    LCD_ShowHZ(x + 230,y,hanzi_shuzi[temp/10%10],288);
    LCD_ShowHZ(x + 260,y,hanzi_shuzi[temp%10],288);
    LCD_ShowHZ(x + 290,y,hanzi_baifenhao,288);

}

void fengsu_Show(u16 temp, u16 x, u16 y)
{
    POINT_COLOR=BLUE;//设置字体色
    LCD_ShowHZ(x,y,hanzi_feng,288);
    LCD_ShowHZ(x + 50,y,hanzi_su,288);
    LCD_ShowHZ(x + 100,y,hanzi_maohao,288);

    if((temp >= warn_table[FENGSU_SHANG]) || (temp < warn_table[FENGSU_XIA]))
    {
        POINT_COLOR=RED;//设置字体色
        warn_happen_flag = 1;
    }
    else
    {
        POINT_COLOR=BLUE;//设置字体色
    }

    LCD_Fill(x + 150,y,240,y + 50,WHITE);
    LCD_ShowHZ(x + 150,y,hanzi_shuzi[temp/10],288);
    LCD_ShowHZ(x + 180,y,hanzi_duhaodian,288);
    LCD_ShowHZ(x + 200,y,hanzi_shuzi[temp%10],288);
    LCD_ShowHZ(x + 240,y,hanzi_fengsudanwei1,288);
    LCD_ShowHZ(x + 290,y,hanzi_fengsudanwei2,288);

}

void zaoshen_Show(u8 temp, u16 x, u16 y)
{
    POINT_COLOR=BLUE;//设置字体色
    LCD_ShowHZ(x,y,hanzi_zao,288);
    LCD_ShowHZ(x + 50,y,hanzi_shen,288);
    LCD_ShowHZ(x + 100,y,hanzi_maohao,288);

    if((temp >= warn_table[ZAOSHEN_SHANG]) || (temp <= warn_table[ZAOSHEN_XIA]))
    {
        POINT_COLOR=RED;//设置字体色
        warn_happen_flag = 1;
    }
    else
    {
        POINT_COLOR=BLUE;//设置字体色
    }
    LCD_Fill(x + 150,y,220,y + 50,WHITE);
    LCD_ShowHZ(x + 150,y,hanzi_shuzi[temp/10],288);
    LCD_ShowHZ(x + 180,y,hanzi_shuzi[temp%10],288);
    LCD_ShowHZ(x + 220,y,hanzi_zaoshendanwei1,288);
    LCD_ShowHZ(x + 250,y,hanzi_zaoshendanwei2,288);

}


void guangzhao_Show(u16 temp, u16 x, u16 y)
{
    POINT_COLOR=BLUE;//设置字体色
    LCD_ShowHZ(x,y,hanzi_guang,288);
    LCD_ShowHZ(x + 50,y,hanzi_zhao,288);
    LCD_ShowHZ(x + 100,y,hanzi_maohao,288);
    if((temp >= warn_table[GUANGZHAO_SHANG]) || (temp <= warn_table[GUANGZHAO_XIA]))
    {
        POINT_COLOR=RED;//设置字体色
        warn_happen_flag = 1;
    }
    else
    {
        POINT_COLOR=BLUE;//设置字体色
    }
    if(temp < 100)
    {
        LCD_Fill(x + 150,y,290,y + 50,WHITE);
        LCD_ShowHZ(x + 150,y,hanzi_shuzi[temp/10],288);
        LCD_ShowHZ(x + 180,y,hanzi_shuzi[temp%10],288);
        LCD_ShowHZ(x + 220,y,hanzi_guangzhaodanwei1,288);
        LCD_ShowHZ(x + 250,y,hanzi_guangzhaodanwei2,288);
    }
    else
    {
        LCD_Fill(x + 150,y,270,y + 50,WHITE);
        LCD_ShowHZ(x + 150,y,hanzi_shuzi[temp/100],288);
        LCD_ShowHZ(x + 180,y,hanzi_shuzi[temp/10%10],288);
        LCD_ShowHZ(x + 210,y,hanzi_shuzi[temp%10],288);
        LCD_ShowHZ(x + 240,y,hanzi_guangzhaodanwei1,288);
        LCD_ShowHZ(x + 270,y,hanzi_guangzhaodanwei2,288);
    }


}
void co2_Show(u16 temp, u16 x, u16 y)
{

    POINT_COLOR=BLUE;//设置字体色
    LCD_ShowHZ(x,y,hanzi_er,288);
    LCD_ShowHZ(x + 50,y,hanzi_yang,288);
    LCD_ShowHZ(x + 100,y,hanzi_hua,288);
    LCD_ShowHZ(x + 150,y,hanzi_tan,288);
    LCD_ShowHZ(x + 200,y,hanzi_maohao,288);

    if((temp >= warn_table[CO2_SHANG]) || (temp <= warn_table[CO2_XIA]))
    {
        POINT_COLOR=RED;//设置字体色
        warn_happen_flag = 1;
    }
    else
    {
        POINT_COLOR=BLUE;//设置字体色
    }

    if (temp > 999)
    {
        LCD_Fill(x + 250,y,470,y + 50,WHITE);
        LCD_ShowHZ(x + 250,y,hanzi_shuzi[temp/1000],288);
        LCD_ShowHZ(x + 280,y,hanzi_shuzi[temp/100%10],288);
        LCD_ShowHZ(x + 310,y,hanzi_shuzi[temp/10%10],288);
        LCD_ShowHZ(x + 340,y,hanzi_shuzi[temp%10],288);
        LCD_ShowHZ(x + 370,y,hanzi_p,288);
        LCD_ShowHZ(x + 400,y,hanzi_p,288);
        LCD_ShowHZ(x + 430,y,hanzi_m,288);
    }
    else
    {
        LCD_Fill(x + 250,y,430,y + 50,WHITE);
        LCD_ShowHZ(x + 250,y,hanzi_shuzi[temp/100],288);
        LCD_ShowHZ(x + 280,y,hanzi_shuzi[temp/10%10],288);
        LCD_ShowHZ(x + 310,y,hanzi_shuzi[temp%10],288);
        LCD_ShowHZ(x + 340,y,hanzi_p,288);
        LCD_ShowHZ(x + 370,y,hanzi_p,288);
        LCD_ShowHZ(x + 400,y,hanzi_m,288);
    }


}



void Alarm_setting_show(void)
{
    u16 x,y;
    x = 10;
    y = 680;

    LCD_Fill(10,y,210,y + 50,BLACK);
    POINT_COLOR=BROWN;//设置字体色
    LCD_ShowHZ(x,y,hanzi_baojing1,288);
    LCD_ShowHZ(x + 50,y,hanzi_baojing2,288);
    LCD_ShowHZ(x + 100,y,hanzi_sheding1,288);
    LCD_ShowHZ(x + 150,y,hanzi_sheding2,288);
    POINT_COLOR=BLACK;//设置字体色

}


void Alarm_num_show(void)
{
    u16 x,y,x1,y1;

    switch (warn_old_num)
    {
        case CO2_SHANG:
            POINT_COLOR=BLACK;//设置字体色
            x  = CO2SHANG_BUTTON_X_MIN_COORDINATE;
            y  = CO2SHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = CO2SHANG_BUTTON_X_MAX_COORDINATE;
            y1 = CO2SHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[CO2_SHANG]/1000],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[CO2_SHANG]/100%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[CO2_SHANG]/10%10],288);
            LCD_ShowHZ(x + 90,y,hanzi_shuzi[warn_table[CO2_SHANG]%10],288);
        break;
        case CO2_XIA:
            POINT_COLOR=BLACK;//设置字体色
            x  = CO2XIA_BUTTON_X_MIN_COORDINATE;
            y  = CO2XIA_BUTTON_Y_MIN_COORDINATE;
            x1 = CO2XIA_BUTTON_X_MAX_COORDINATE;
            y1 = CO2XIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[CO2_XIA]/1000],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[CO2_XIA]/100%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[CO2_XIA]/10%10],288);
            LCD_ShowHZ(x + 90,y,hanzi_shuzi[warn_table[CO2_XIA]%10],288);
        break;

        case WENDU_SHANG:
            POINT_COLOR=BLACK;//设置字体色
            x  = WENDUSHANG_BUTTON_X_MIN_COORDINATE;
            y  = WENDUSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = WENDUSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = WENDUSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[WENDU_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[WENDU_SHANG]%10],288);
        break;
        case WENDU_XIA:
            POINT_COLOR=BLACK;//设置字体色
            x  = WENDUXIA_BUTTON_X_MIN_COORDINATE;
            y  = WENDUXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = WENDUXIA_BUTTON_X_MAX_COORDINATE;
            y1 = WENDUXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[WENDU_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[WENDU_XIA]%10],288);
        break;

        case SHIDU_SHANG:
            POINT_COLOR=BLACK;//设置字体色
            x  = SHIDUSHANG_BUTTON_X_MIN_COORDINATE;
            y  = SHIDUSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = SHIDUSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = SHIDUSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[SHIDU_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[SHIDU_SHANG]%10],288);
        break;
        case SHIDU_XIA:
            POINT_COLOR=BLACK;//设置字体色
            x  = SHIDUXIA_BUTTON_X_MIN_COORDINATE;
            y  = SHIDUXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = SHIDUXIA_BUTTON_X_MAX_COORDINATE;
            y1 = SHIDUXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[SHIDU_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[SHIDU_XIA]%10],288);
        break;


        case GUANGZHAO_SHANG:
            POINT_COLOR=BLACK;//设置字体色
            x  = GUANGZHAOSHANG_BUTTON_X_MIN_COORDINATE;
            y  = GUANGZHAOSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = GUANGZHAOSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = GUANGZHAOSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]/100],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]/10%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]%10],288);
        break;
        case GUANGZHAO_XIA:
            POINT_COLOR=BLACK;//设置字体色
            x  = GUANGZHAOXIA_BUTTON_X_MIN_COORDINATE;
            y  = GUANGZHAOXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = GUANGZHAOXIA_BUTTON_X_MAX_COORDINATE;
            y1 = GUANGZHAOXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]/100],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]/10%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]%10],288);
        break;


        case FENGSU_SHANG:
            POINT_COLOR=BLACK;//设置字体色
            x  = FENGSUSHANG_BUTTON_X_MIN_COORDINATE;
            y  = FENGSUSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = FENGSUSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = FENGSUSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[FENGSU_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[FENGSU_SHANG]%10],288);
        break;
        case FENGSU_XIA:
            POINT_COLOR=BLACK;//设置字体色
            x  = FENGSUXIA_BUTTON_X_MIN_COORDINATE;
            y  = FENGSUXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = FENGSUXIA_BUTTON_X_MAX_COORDINATE;
            y1 = FENGSUXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[FENGSU_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[FENGSU_XIA]%10],288);
        break;


        case ZAOSHEN_SHANG:
            POINT_COLOR=BLACK;//设置字体色
            x  = ZAOSHENSHANG_BUTTON_X_MIN_COORDINATE;
            y  = ZAOSHENSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = ZAOSHENSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = ZAOSHENSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[ZAOSHEN_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[ZAOSHEN_SHANG]%10],288);
        break;
        case ZAOSHEN_XIA:
            POINT_COLOR=BLACK;//设置字体色
            x  = ZAOSHENXIA_BUTTON_X_MIN_COORDINATE;
            y  = ZAOSHENXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = ZAOSHENXIA_BUTTON_X_MAX_COORDINATE;
            y1 = ZAOSHENXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[ZAOSHEN_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[ZAOSHEN_XIA]%10],288);
        break;

        default:
        break;
    }

    switch (warn_num)
    {
        case CO2_SHANG:
            POINT_COLOR=RED;//设置字体色
            x  = CO2SHANG_BUTTON_X_MIN_COORDINATE;
            y  = CO2SHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = CO2SHANG_BUTTON_X_MAX_COORDINATE;
            y1 = CO2SHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[CO2_SHANG]/1000],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[CO2_SHANG]/100%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[CO2_SHANG]/10%10],288);
            LCD_ShowHZ(x + 90,y,hanzi_shuzi[warn_table[CO2_SHANG]%10],288);
        break;
        case CO2_XIA:
            POINT_COLOR=RED;//设置字体色
            x  = CO2XIA_BUTTON_X_MIN_COORDINATE;
            y  = CO2XIA_BUTTON_Y_MIN_COORDINATE;
            x1 = CO2XIA_BUTTON_X_MAX_COORDINATE;
            y1 = CO2XIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[CO2_XIA]/1000],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[CO2_XIA]/100%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[CO2_XIA]/10%10],288);
            LCD_ShowHZ(x + 90,y,hanzi_shuzi[warn_table[CO2_XIA]%10],288);
        break;

        case WENDU_SHANG:
            POINT_COLOR=RED;//设置字体色
            x  = WENDUSHANG_BUTTON_X_MIN_COORDINATE;
            y  = WENDUSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = WENDUSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = WENDUSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[WENDU_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[WENDU_SHANG]%10],288);
        break;
        case WENDU_XIA:
            POINT_COLOR=RED;//设置字体色
            x  = WENDUXIA_BUTTON_X_MIN_COORDINATE;
            y  = WENDUXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = WENDUXIA_BUTTON_X_MAX_COORDINATE;
            y1 = WENDUXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[WENDU_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[WENDU_XIA]%10],288);
        break;

        case SHIDU_SHANG:
            POINT_COLOR=RED;//设置字体色
            x  = SHIDUSHANG_BUTTON_X_MIN_COORDINATE;
            y  = SHIDUSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = SHIDUSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = SHIDUSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[SHIDU_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[SHIDU_SHANG]%10],288);
        break;
        case SHIDU_XIA:
            POINT_COLOR=RED;//设置字体色
            x  = SHIDUXIA_BUTTON_X_MIN_COORDINATE;
            y  = SHIDUXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = SHIDUXIA_BUTTON_X_MAX_COORDINATE;
            y1 = SHIDUXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[SHIDU_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[SHIDU_XIA]%10],288);
        break;


        case GUANGZHAO_SHANG:
            POINT_COLOR=RED;//设置字体色
            x  = GUANGZHAOSHANG_BUTTON_X_MIN_COORDINATE;
            y  = GUANGZHAOSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = GUANGZHAOSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = GUANGZHAOSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]/100],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]/10%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[GUANGZHAO_SHANG]%10],288);
        break;
        case GUANGZHAO_XIA:
            POINT_COLOR=RED;//设置字体色
            x  = GUANGZHAOXIA_BUTTON_X_MIN_COORDINATE;
            y  = GUANGZHAOXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = GUANGZHAOXIA_BUTTON_X_MAX_COORDINATE;
            y1 = GUANGZHAOXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]/100],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]/10%10],288);
            LCD_ShowHZ(x + 60,y,hanzi_shuzi[warn_table[GUANGZHAO_XIA]%10],288);
        break;


        case FENGSU_SHANG:
            POINT_COLOR=RED;//设置字体色
            x  = FENGSUSHANG_BUTTON_X_MIN_COORDINATE;
            y  = FENGSUSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = FENGSUSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = FENGSUSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[FENGSU_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[FENGSU_SHANG]%10],288);
        break;
        case FENGSU_XIA:
            POINT_COLOR=RED;//设置字体色
            x  = FENGSUXIA_BUTTON_X_MIN_COORDINATE;
            y  = FENGSUXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = FENGSUXIA_BUTTON_X_MAX_COORDINATE;
            y1 = FENGSUXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[FENGSU_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[FENGSU_XIA]%10],288);
        break;


        case ZAOSHEN_SHANG:
            POINT_COLOR=RED;//设置字体色
            x  = ZAOSHENSHANG_BUTTON_X_MIN_COORDINATE;
            y  = ZAOSHENSHANG_BUTTON_Y_MIN_COORDINATE;
            x1 = ZAOSHENSHANG_BUTTON_X_MAX_COORDINATE;
            y1 = ZAOSHENSHANG_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[ZAOSHEN_SHANG]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[ZAOSHEN_SHANG]%10],288);
        break;
        case ZAOSHEN_XIA:
            POINT_COLOR=RED;//设置字体色
            x  = ZAOSHENXIA_BUTTON_X_MIN_COORDINATE;
            y  = ZAOSHENXIA_BUTTON_Y_MIN_COORDINATE;
            x1 = ZAOSHENXIA_BUTTON_X_MAX_COORDINATE;
            y1 = ZAOSHENXIA_BUTTON_Y_MAX_COORDINATE;
            LCD_Fill(x,y,x1,y1,WHITE);
            LCD_ShowHZ(x,y,hanzi_shuzi[warn_table[ZAOSHEN_XIA]/10%10],288);
            LCD_ShowHZ(x + 30,y,hanzi_shuzi[warn_table[ZAOSHEN_XIA]%10],288);
        break;

        default:
        break;
    }

}

u8 set_button_down_handle(void)
{
    u8 ret = 0;
    u8 t = 0;

    warn_old_num = warn_num;
    //CO2上按钮按下
    if(tp_dev.x[t]>=CO2SHANG_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= CO2SHANG_BUTTON_X_MAX_COORDINATE &&
       tp_dev.y[t]>=CO2SHANG_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= CO2SHANG_BUTTON_Y_MAX_COORDINATE)
    {
       ret = 1;
       warn_num = CO2_SHANG;
       LCD_Fill(CO2SHANG_BUTTON_X_MIN_COORDINATE,CO2SHANG_BUTTON_Y_MIN_COORDINATE,CO2SHANG_BUTTON_X_MAX_COORDINATE,CO2SHANG_BUTTON_Y_MAX_COORDINATE,BLACK);
    }
    //CO2下按钮按下
    else if(tp_dev.x[t]>=CO2XIA_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= CO2XIA_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=CO2XIA_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= CO2XIA_BUTTON_Y_MAX_COORDINATE)
    {
        ret = 1;
        warn_num = CO2_XIA;
        LCD_Fill(CO2XIA_BUTTON_X_MIN_COORDINATE,CO2XIA_BUTTON_Y_MIN_COORDINATE,CO2XIA_BUTTON_X_MAX_COORDINATE,CO2XIA_BUTTON_Y_MAX_COORDINATE,BLACK);
    }

    //温度上按钮按下
    else if(tp_dev.x[t]>=WENDUSHANG_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= WENDUSHANG_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=WENDUSHANG_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= WENDUSHANG_BUTTON_Y_MAX_COORDINATE)
    {
       ret = 1;
       warn_num = WENDU_SHANG;
       LCD_Fill(WENDUSHANG_BUTTON_X_MIN_COORDINATE,WENDUSHANG_BUTTON_Y_MIN_COORDINATE,WENDUSHANG_BUTTON_X_MAX_COORDINATE,WENDUSHANG_BUTTON_Y_MAX_COORDINATE,BLACK);
    }
    //温度下按钮按下
    else if(tp_dev.x[t]>=WENDUXIA_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= WENDUXIA_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=WENDUXIA_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= WENDUXIA_BUTTON_Y_MAX_COORDINATE)
    {
        ret = 1;
        warn_num = WENDU_XIA;
        LCD_Fill(WENDUXIA_BUTTON_X_MIN_COORDINATE,WENDUXIA_BUTTON_Y_MIN_COORDINATE,WENDUXIA_BUTTON_X_MAX_COORDINATE,WENDUXIA_BUTTON_Y_MAX_COORDINATE,BLACK);
    }

    //湿度上按钮按下
    else if(tp_dev.x[t]>=SHIDUSHANG_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= SHIDUSHANG_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=SHIDUSHANG_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= SHIDUSHANG_BUTTON_Y_MAX_COORDINATE)
    {
       ret = 1;
       warn_num = SHIDU_SHANG;
       LCD_Fill(SHIDUSHANG_BUTTON_X_MIN_COORDINATE,SHIDUSHANG_BUTTON_Y_MIN_COORDINATE,SHIDUSHANG_BUTTON_X_MAX_COORDINATE,SHIDUSHANG_BUTTON_Y_MAX_COORDINATE,BLACK);
    }
    //湿度下按钮按下
    else if(tp_dev.x[t]>=SHIDUXIA_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= SHIDUXIA_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=SHIDUXIA_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= SHIDUXIA_BUTTON_Y_MAX_COORDINATE)
    {
        ret = 1;
        warn_num = SHIDU_XIA;
        LCD_Fill(SHIDUXIA_BUTTON_X_MIN_COORDINATE,SHIDUXIA_BUTTON_Y_MIN_COORDINATE,SHIDUXIA_BUTTON_X_MAX_COORDINATE,SHIDUXIA_BUTTON_Y_MAX_COORDINATE,BLACK);
    }

    //光照上按钮按下
    else if(tp_dev.x[t]>=GUANGZHAOSHANG_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= GUANGZHAOSHANG_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=GUANGZHAOSHANG_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= GUANGZHAOSHANG_BUTTON_Y_MAX_COORDINATE)
    {
       ret = 1;
       warn_num = GUANGZHAO_SHANG;
       LCD_Fill(GUANGZHAOSHANG_BUTTON_X_MIN_COORDINATE,GUANGZHAOSHANG_BUTTON_Y_MIN_COORDINATE,GUANGZHAOSHANG_BUTTON_X_MAX_COORDINATE,GUANGZHAOSHANG_BUTTON_Y_MAX_COORDINATE,BLACK);
    }
    //光照下按钮按下
    else if(tp_dev.x[t]>=GUANGZHAOXIA_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= GUANGZHAOXIA_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=GUANGZHAOXIA_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= GUANGZHAOXIA_BUTTON_Y_MAX_COORDINATE)
    {
        ret = 1;
        warn_num = GUANGZHAO_XIA;
        LCD_Fill(GUANGZHAOXIA_BUTTON_X_MIN_COORDINATE,GUANGZHAOXIA_BUTTON_Y_MIN_COORDINATE,GUANGZHAOXIA_BUTTON_X_MAX_COORDINATE,GUANGZHAOXIA_BUTTON_Y_MAX_COORDINATE,BLACK);
    }

    //风速上按钮按下
    else if(tp_dev.x[t]>=FENGSUSHANG_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= FENGSUSHANG_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=FENGSUSHANG_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= FENGSUSHANG_BUTTON_Y_MAX_COORDINATE)
    {
       ret = 1;
       warn_num = FENGSU_SHANG;
       LCD_Fill(FENGSUSHANG_BUTTON_X_MIN_COORDINATE,FENGSUSHANG_BUTTON_Y_MIN_COORDINATE,FENGSUSHANG_BUTTON_X_MAX_COORDINATE,FENGSUSHANG_BUTTON_Y_MAX_COORDINATE,BLACK);
    }
    //风速下按钮按下
    else if(tp_dev.x[t]>=FENGSUXIA_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= FENGSUXIA_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=FENGSUXIA_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= FENGSUXIA_BUTTON_Y_MAX_COORDINATE)
    {
        ret = 1;
        warn_num = FENGSU_XIA;
        LCD_Fill(FENGSUXIA_BUTTON_X_MIN_COORDINATE,FENGSUXIA_BUTTON_Y_MIN_COORDINATE,FENGSUXIA_BUTTON_X_MAX_COORDINATE,FENGSUXIA_BUTTON_Y_MAX_COORDINATE,BLACK);
    }

    //噪声上按钮按下
    else if(tp_dev.x[t]>=ZAOSHENSHANG_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= ZAOSHENSHANG_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=ZAOSHENSHANG_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= ZAOSHENSHANG_BUTTON_Y_MAX_COORDINATE)
    {
       ret = 1;
       warn_num = ZAOSHEN_SHANG;
       LCD_Fill(ZAOSHENSHANG_BUTTON_X_MIN_COORDINATE,ZAOSHENSHANG_BUTTON_Y_MIN_COORDINATE,ZAOSHENSHANG_BUTTON_X_MAX_COORDINATE,ZAOSHENSHANG_BUTTON_Y_MAX_COORDINATE,BLACK);
    }
    //噪声下按钮按下
    else if(tp_dev.x[t]>=ZAOSHENXIA_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= ZAOSHENXIA_BUTTON_X_MAX_COORDINATE &&
            tp_dev.y[t]>=ZAOSHENXIA_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= ZAOSHENXIA_BUTTON_Y_MAX_COORDINATE)
    {
        ret = 1;
        warn_num = ZAOSHEN_XIA;
        LCD_Fill(ZAOSHENXIA_BUTTON_X_MIN_COORDINATE,ZAOSHENXIA_BUTTON_Y_MIN_COORDINATE,ZAOSHENXIA_BUTTON_X_MAX_COORDINATE,ZAOSHENXIA_BUTTON_Y_MAX_COORDINATE,BLACK);
    }

    return ret;
}

u8 Alarm_setting(void)
{
    u8 t=0;
    u8 ret = 0;

    tp_dev.scan(0);
    //for(t=0;t<CT_MAX_TOUCH;t++)
    {
        if((tp_dev.sta)&(1<<t))
        {
            /* 按下选择 */
            if(tp_dev.x[t]<lcddev.width && tp_dev.y[t]<lcddev.height && tp_dev.sta == 0xC1)
            {
                //报警设定按钮
                if(tp_dev.x[t] >= SET_BUTTON_X_MIN_COORDINATE && tp_dev.x[t] <= SET_BUTTON_X_MAX_COORDINATE &&
                   tp_dev.y[t] >= SET_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t] <= SET_BUTTON_Y_MAX_COORDINATE)
                {
                   baojing_down_flag = 1;
                   Load_Drow_Dialog();
                   ret = 1;
                }

                /* 当前乃设定界面 */
                if (baojing_down_flag == 1)
                {
                    //退出按钮按下
                    if(tp_dev.x[t]>=OUT_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= OUT_BUTTON_X_MAX_COORDINATE &&
                       tp_dev.y[t]>=OUT_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= OUT_BUTTON_Y_MAX_COORDINATE)
                    {
                       baojing_down_flag = 0;
                       lcd_show_init();
                       ret = 1;
                    }
                    else
                    {
                        ret = set_button_down_handle();
                        if(ret == 1)
                        {
                            write_save_data();
                        }
                    }
                }

            }
            /* 抬起选择 */
            if(tp_dev.x[t]<lcddev.width && tp_dev.y[t]<lcddev.height && tp_dev.sta == 0x41)
            {
                    //加号按钮按下
                    if(tp_dev.x[t]>=JIAHAO_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= JIAHAO_BUTTON_X_MAX_COORDINATE &&
                       tp_dev.y[t]>=JIAHAO_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= JIAHAO_BUTTON_Y_MAX_COORDINATE)
                    {
                        warn_table[warn_num]++;
                        ret = 1;
                    }

                    //减号按钮按下
                    else if(tp_dev.x[t]>=JIANHAO_BUTTON_X_MIN_COORDINATE && tp_dev.x[t]<= JIANHAO_BUTTON_X_MAX_COORDINATE &&
                       tp_dev.y[t]>=JIANHAO_BUTTON_Y_MIN_COORDINATE && tp_dev.y[t]<= JIANHAO_BUTTON_Y_MAX_COORDINATE)
                    {
                        warn_table[warn_num]--;
                        ret = 1;
                    }
            }

        }
    }
    delay_ms(10);

    return ret;
}

void sensor_all_send(u8 sensor_num)
{
    u8 rs485buf[8];
    /* 多功能传感器 */
    if(sensor_num == 1)
    {
        rs485buf[0] = 0x01;
        rs485buf[1] = 0x03;
        rs485buf[2] = 0x00;
        rs485buf[3] = 0x00;
        rs485buf[4] = 0x00;
        rs485buf[5] = 0x04;
        rs485buf[6] = 0x44;
        rs485buf[7] = 0x09;
    }
    /* 风速传感器 */
    else if (sensor_num == 2)
    {
        rs485buf[0] = 0x02;
        rs485buf[1] = 0x03;
        rs485buf[2] = 0x00;
        rs485buf[3] = 0x00;
        rs485buf[4] = 0x00;
        rs485buf[5] = 0x01;
        rs485buf[6] = 0x84;
        rs485buf[7] = 0x39;
    }


    RS485_Send_Data(rs485buf,8);
}


void Multi_function_sensor_recive(void)
{
    u8 i   = 0;
    co2_vaule_bak       = co2_vaule;
    wendu_vaule_bak     = wendu_vaule;
    shidu_vaule_bak     = shidu_vaule;
    guangzhao_vaule_bak = guangzhao_vaule;
    zaoshen_vaule_bak   = zaoshen_vaule;

    co2_vaule       = 256 * RS485_RX_BUF[3] + RS485_RX_BUF[4];
    wendu_vaule     = (RS485_RX_BUF[5] << 8) | RS485_RX_BUF[6];
    shidu_vaule     = (RS485_RX_BUF[7] << 8) | RS485_RX_BUF[8];
    guangzhao_vaule = (RS485_RX_BUF[9] << 8) | RS485_RX_BUF[10];
    for(i = 0; i < 13; i++)
    {
        RS485_RX_BUF[i] = 0;
    }
    RS485_RX_CNT = 0;
    if (co2_vaule_bak != co2_vaule)
    {
        co2_Show(co2_vaule, 0, 370);
    }
    if (wendu_vaule_bak != wendu_vaule)
    {
        wendu_Show(wendu_vaule, 0 , 70);
    }
    if(shidu_vaule_bak != shidu_vaule)
    {
        shidu_Show(shidu_vaule, 0, 130);
    }
    if(guangzhao_vaule_bak != guangzhao_vaule)
    {
        guangzhao_Show(guangzhao_vaule, 0, 310);
    }

    if(zaoshen_vaule_bak != zaoshen_vaule)
    {
        zaoshen_Show(zaoshen_vaule, 0 ,250);
    }
}

void Wind_sensor_recive(void)
{
    u8 i   = 0;


    fengsu_vaule_bak    = fengsu_vaule;

    fengsu_vaule        = (RS485_RX_BUF[3] << 8) | RS485_RX_BUF[4];
    for(i = 0; i < 8; i++)
    {
        RS485_RX_BUF[i] = 0;
    }
    if(fengsu_vaule_bak != fengsu_vaule)
    {
        fengsu_Show(fengsu_vaule, 0, 190);
    }
}

void sensor_data(void)
{
    /* 接收到多功能传感器数据 */
    if((RS485_RX_CNT == 13) && (RS485_RX_BUF[0] == 0x01))
    {
        ask_data_count = 0;
        Multi_function_sensor_recive();
        sensor_status_show(1);
    }
    /* 接收到风速传感器数据 */
    if((RS485_RX_CNT == 7) && (RS485_RX_BUF[0] == 0x02))
    {
        ask_data_count = 0;
        Wind_sensor_recive();
        sensor_status_show(1);
    }

}


void sensor_send(void)
{
    static u8 sensor_count = 0;


    sensor_count++;
    if(sensor_count % 3 == 0)
    {
        sensor_all_send(Multi_function_sensor);  // 请求多功能传感器数据
    }
    else if (sensor_count % 3 == 1)
    {
        sensor_all_send(Wind_sensor);
    }
    else if (sensor_count % 3 == 2)
    {

    }
    else
    {
        ;
    }

    if(ask_data_count >= 5)
    {
        ask_data_count = 5;
        // 传感器失效
        sensor_status_show(0);
    }
    else
    {
        ask_data_count++;
    }
}


void system_init(void)
{
    delay_init();            //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    uart_init(115200);      //串口初始化为115200

    LED_Init();              //LED端口初始化
    LCD_Init();
    KEY_Init();
    tp_dev.init();
    RS485_Init(36,9600);     //初始化RS485
    TIM3_Int_Init(6999,7199);
    BEEP_Init();
    W25QXX_Init();

}

void lcd_show_init(void)
{
    LCD_Clear(WHITE);   //清屏
    /* 显示项目标题 */
    Title_Show(40,0);
    /* 显示数据 */

    wendu_Show(wendu_vaule, 0, 70);
    shidu_Show(shidu_vaule, 0, 130);
    fengsu_Show(fengsu_vaule, 0 ,190);
    zaoshen_Show(zaoshen_vaule, 0 ,250);
    guangzhao_Show(guangzhao_vaule, 0, 310);
    co2_Show(co2_vaule, 0, 370);

    /* 警告文字显示 */
    Alarm_setting_show();

}
void write_save_data(void)
{
    flash_data_save[0]  = warn_table[CO2_SHANG] >> 8;
    flash_data_save[1]  = warn_table[CO2_SHANG] & 0xFF;
    flash_data_save[2]  = warn_table[CO2_XIA] >> 8;
    flash_data_save[3]  = warn_table[CO2_XIA] & 0xFF;

    flash_data_save[4]  = warn_table[WENDU_SHANG];
    flash_data_save[5]  = warn_table[WENDU_XIA];

    flash_data_save[6]  = warn_table[SHIDU_SHANG];
    flash_data_save[7]  = warn_table[SHIDU_XIA];

    flash_data_save[8]  = warn_table[GUANGZHAO_SHANG] >> 8;
    flash_data_save[9]  = warn_table[GUANGZHAO_SHANG] & 0xFF;
    flash_data_save[10]  = warn_table[GUANGZHAO_XIA] >> 8;
    flash_data_save[11]  = warn_table[GUANGZHAO_XIA] & 0xFF;

    flash_data_save[12]  = warn_table[FENGSU_SHANG];
    flash_data_save[13]  = warn_table[FENGSU_XIA];

    flash_data_save[14] = warn_table[ZAOSHEN_SHANG];
    flash_data_save[15] = warn_table[ZAOSHEN_XIA];

    W25QXX_Write(flash_data_save, 0x00, 16);
}

void read_save_data(void)
{
    u16 save_data;

    W25QXX_Read(flash_data_save, 0x00, 16);
    save_data = (flash_data_save[0] << 8 | flash_data_save[1]);
    //二氧化碳上限设定
    if ((save_data > 0) && (save_data < 3000))
    {
        warn_table[CO2_SHANG] = save_data;
    }
    //二氧化碳下限设定
    save_data = (flash_data_save[2] << 8 | flash_data_save[3]);
    if ((save_data > 0) && (save_data < 3000))
    {
        warn_table[CO2_XIA] = save_data;
    }
    //温度上限设定
    save_data = (flash_data_save[4]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[WENDU_SHANG] = save_data;
    }
    //温度下限设定
    save_data = (flash_data_save[5]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[WENDU_XIA] = save_data;
    }

    //湿度上限设定
    save_data = (flash_data_save[6]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[SHIDU_SHANG] = save_data;
    }
    //湿度下限设定
    save_data = (flash_data_save[7]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[SHIDU_XIA] = save_data;
    }

    //光照上限设定
    save_data = (flash_data_save[8] << 8 | flash_data_save[9]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[GUANGZHAO_SHANG] = save_data;
    }
    //光照下限设定
    save_data = (flash_data_save[10] << 8 | flash_data_save[11]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[GUANGZHAO_XIA] = save_data;
    }

    //风速上限设定
    save_data = (flash_data_save[12]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[FENGSU_SHANG] = save_data;
    }
    //风速下限设定
    save_data = (flash_data_save[13]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[FENGSU_XIA] = save_data;
    }

    //噪声上限设定
    save_data = (flash_data_save[14]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[ZAOSHEN_SHANG] = save_data;
    }
    //噪声下限设定
    save_data = (flash_data_save[15]);
    if ((save_data > 0) && (save_data < 100))
    {
        warn_table[ZAOSHEN_XIA] = save_data;
    }
}

int main(void)
{
    u8 ret = 0;
    //static u8 rs485buf[20];

    system_init();

    read_save_data();

    lcd_show_init();
    while(1)
    {

        /* 触摸屏监控 */
        ret = Alarm_setting();
        if (ret == 1)
        {
            ret = 0;
            if ((baojing_down_flag == 1) && (warn_num != warn_old_num))
            {
                /* 报警设定值显示 */
                Alarm_num_show();
            }

        }

        if(baojing_down_flag == 0)
        {
            /* 传感器采集时间 */
            if(time_485flag == 1)
            {
                sensor_send();
                time_485flag = 0;
            }
            sensor_data();
        }
        if (warn_happen_flag == 1)
        {
            warn_happen_flag = 0;
            //beep_count = 6;
        }
        delay_ms(10);
    }

}



