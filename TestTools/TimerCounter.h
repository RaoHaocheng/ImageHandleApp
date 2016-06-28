#ifndef TIMERCOUNTER_H
#define TIMERCOUNTER_H

// 这个是测试类
namespace TestTools
{
	class TimerCounter
	{
	public:
		TimerCounter();
		~TimerCounter();

	public:
		void StartTimer();             // 开始计时装置
		void StopTimer();              // 提示计时
		void PauseTimer();             // 暂停计时
		void ReStartTimer();           // 重新开始计时
		void CleanTimer();             // 清除定时器

	private:

	};
}
#endif // !DEFINE_H
