#ifndef CONTROLLER_PERIPHERAL_HPP
#define CONTROLLER_PERIPHERAL_HPP

namespace BasePeripheral {
	typedef uint32_t error_t;

	class ControllerPeripheral {
	protected:
		// ����������� ����� ��� ��������� ������������
		virtual void enableClock() = 0;

		// ����������� ����� ��� ���������� ������������
		virtual void disableClock() = 0;

		//������� �� ������
		virtual void onError(error_t) = 0;

	public:
		// ����������� ���������� ��� ����������� ����������� �������� ����������� ��������
		virtual ~ControllerPeripheral() {}

		// ����������� ����� ��� ������������� ����������
		virtual void init() = 0;

		// ���������� ��������� ��������� ����������
		virtual bool isEnabled() const = 0;
	};
}

#endif // !CONTROLLER_PERIPHERAL_HPP
