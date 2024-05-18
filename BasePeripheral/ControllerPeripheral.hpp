#ifndef CONTROLLER_PERIPHERAL_HPP
#define CONTROLLER_PERIPHERAL_HPP

namespace BasePeripheral {
	typedef uint32_t error_t;

	class ControllerPeripheral {
	protected:
		// Виртуальный метод включения тактирования блока периферии (должен быть реализован в конечном наследнике)
		virtual void onEnableClock() = 0;

		// Виртуальный метод выключения тактирования блока периферии (должен быть реализован в конечном наследнике)
		virtual void onDisableClock() = 0;

		// Виртуальный метод установки ошибки блока периферии (должен быть реализован в конечном наследнике)
		virtual void onError(error_t) = 0;

	public:
		// Виртуальный деструктор
		virtual ~ControllerPeripheral() {}

		// Виртуальный метод инициализации блока периферии (должен быть реализован в наследнике)
		virtual void init() = 0;

		// Виртуальный метод деинициализации блока периферии (должен быть реализован в наследнике)
		virtual void deInit() = 0;

		// Виртуальный метод получения состояния включения периферии (должен быть реализован в конечном наследнике)
		virtual bool isEnabled() const = 0;
	};
}

#endif // !CONTROLLER_PERIPHERAL_HPP
