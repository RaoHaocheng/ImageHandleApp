#ifndef TIMERCOUNTER_H
#define TIMERCOUNTER_H

// ����ǲ�����
namespace TestTools
{
	class TimerCounter
	{
	public:
		TimerCounter();
		~TimerCounter();

	public:
		void StartTimer();             // ��ʼ��ʱװ��
		void StopTimer();              // ��ʾ��ʱ
		void PauseTimer();             // ��ͣ��ʱ
		void ReStartTimer();           // ���¿�ʼ��ʱ
		void CleanTimer();             // �����ʱ��

	private:

	};
}
#endif // !DEFINE_H
