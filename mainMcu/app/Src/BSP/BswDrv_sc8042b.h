#ifndef __SC8042B_H__
#define __SC8042B_H__

#define AU_POWER_EN()   gpio_bit_set(GPIOC,GPIO_PIN_6)
#define AU_POWER_DIS()  gpio_bit_reset(GPIOC,GPIO_PIN_6)

#define AU_DATA_HIHG()  gpio_bit_set(GPIOB,GPIO_PIN_2)
#define AU_DATA_LOW()   gpio_bit_reset(GPIOB,GPIO_PIN_2)

#define AU_RST_EN()     gpio_bit_set(GPIOB,GPIO_PIN_0)
#define AU_RST_DIS()    gpio_bit_reset(GPIOB,GPIO_PIN_0)

#define READ_AU_BUSY()  gpio_input_bit_get(GPIOB,GPIO_Pin_1)


typedef enum{
	VOIC_NULL1=1,							//空
	VOIC_WELCOME=2,							//欢迎使用
	VOIC_NULL2=3,							//空
	VOIC_SHARE_CHARGE=4,					//共享充电
	VOIC_START_UPGRADE=5,                   //开始升级
	VOIC_READING_CARD=6,               		//正在读卡
	VOIC_DEVICE_EXCEPTION=7,				//设备故障
	VOIC_CARD_BANLANCE=8,					//卡片余额
	VOIC_CARD_RECHARGER=9,					//卡余额不足,请充值
	VOIC_CARD_INVALID=10,					//卡片无效
	VOIC_CARD_CHARGING=11,					//你的爱车正在充电
	VOIC_INPUT_SOCKET_NUM=12,				//请输入插座编号,按确认键
	VOIC_INPUT_CHARGRE_MONEY=13,			//请输入充电金额,按确认键.如需返回上一级,请按返回键
	VOIC_SOCKET_OCCUPID=14,					//插座被占用,请选用其他插座
	VOIC_SOCKET_ERROR=15,					//插座故障,请选用其他插座
	VOIC_SOCKET_NUM_INVALID=16,				//插座编号无效,请重新输入
	VOIC_NIN_YI_XUANZE=17,					//您已选择
	VOIC_HAO_CHAZUO=18,						//号插座
	VOIC_CHARGER_MONEY=19,					//充电金额
	VOIC_YUAN=20,							//元
	VOIC_VERIFIED_PLUG=21,					//请确认充电插座和车端插座均已插好
	VOIC_CARD_BANLANCE_INSUFFICIENT=22,		//余额不足,请重新输入
	VOIC_START_CHARGING=23,					//开始充电
	VOIC_STOP_CHARGER_TIP=24,				//如需结束充电,可拔掉插头
	VOIC_PLUG_IN_PLUG=25,					//请将插头插入
	VOIC_CARD=26,                          	//嘀（刷卡音）
	VOIC_KEY=27,                           	//噔（左右键按键音）
	VOIC_BLUETOOTH_ONLINE=28,				//蓝牙已连接
	VOIC_BLUETOOTH_OFFLINE=29,				//蓝牙已断开
	VOIC_ERROR=30,							//错误
	VOIC_0=31,                             	//0 (数字0)
    VOIC_1=32,                             	//1.
    VOIC_2=33,                             	//2.
    VOIC_3=34,                             	//3.
    VOIC_4=35,                             	//4.
    VOIC_5=36,                             	//5.
    VOIC_6=37,                             	//6.
    VOIC_7=38,                            	//7.
    VOIC_8=39,                             	//8.
    VOIC_9=40,                             	//9
	VOIC_TEST_TIP=41,						//1输桩号,2输识别码,3继电器测试
	VOIC_INPUT_SOCKET_TEST=42,				//输插座号按确定测试
	VOIC_HUNDRED=43,						//百
	VOIC_TEN=44,							//十
	VOIC_YUAN1=45,							//元
	VOIC_POINT=46,							//点
	VOIC_DEVICE_REBOOT=47,					//设备重启
	VOIC_SUCCESS=48,						//成功
	VOIC_A=49,								//a
	VOIC_B=50,								//b
	VOIC_C=51,								//c
	VOIC_D=52,								//d
	VOIC_E=53,								//e
	VOIC_F=54,								//f
	VOIC_TEM=55,							//温度
	VOIC_DEGREE=56,							//度
	VOIC_THOUSAND=57,						//千
	VOIC_START_CHARGER_FAIL=58,				//开启充电失败
	VOIC_POWER_TOO_LARGE=59,                //设备功率过大，无法充电
	VOIC_CARD_RETRY = 60,					//请再次刷卡已开启充电
	VOIC_INPUT_COUNTNUM = 61,				//请输入柜门或者插座编号，并按确认键
	VOIC_COUNT_OPEN = 62,					//柜门已经打开
	VOIC_PUT_BATTERY = 63,					//请放入电池
	VOIC_CHARGED_CLOSEDOOR = 64,			//已经开启充电，关闭柜门
	VOIC_ENSURE_BUTTON_CLOSECHARGER = 65,	//如需结束充电，请安确认键
	VOIC_TOMEOUT_RECARD = 66,				//因为你未按时取出电池，请你再次刷卡以扣除超出费用
	VOIC_GET_BATTERY_CLOSEDOOR = 67,		//请取出电池并关好柜门
	VOIC_NO_CONNECTOR	= 68,				//号插座，充电器未连接，请确认
	VOIC_CARD_REMAIN_TIMERS = 69,			//卡片剩余次数
	VOIC_TIMERS = 70,						//次
	VOICE_NUM,
}SC8042B_VOICE_TYPE;

void BswDrv_Sc8042bSpeech(SC8042B_VOICE_TYPE cnt);
void BswDrv_SC8042B_Init(void);
void PlayWaringVoice(void);


#define Sc8042bSpeech	BswDrv_Sc8042bSpeech

#endif

