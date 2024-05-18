#ifndef CONTROLLER_PERIPHERAL_HPP
#define CONTROLLER_PERIPHERAL_HPP

namespace BasePeripheral {
	typedef uint32_t error_t;

	class ControllerPeripheral {
	protected:
		// Абстрактный метод для включения тактирования
		virtual void enableClock() = 0;

		// Абстрактный метод для выключения тактирования
		virtual void disableClock() = 0;

		//Коллбек об ошибке
		virtual void onError(error_t) = 0;

	public:
		// Виртуальный деструктор для обеспечения корректного удаления производных объектов
		virtual ~ControllerPeripheral() {}

		// Абстрактный метод для инициализации интерфейса
		virtual void init() = 0;

		// Возвращает состояние включения интерфейса
		virtual bool isEnabled() const = 0;
	};
}

#endif // !CONTROLLER_PERIPHERAL_HPP
