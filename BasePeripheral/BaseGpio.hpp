#ifndef GPIO_HPP_
#define GPIO_HPP_

#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Gpio {
		// Перечисление режимов работы GPIO
		enum class Mode : uint32_t {
			Input,             // Режим входа
			Output,            // Режим выхода
			AlternateFunction, // Режим альтернативной функции
			Analog             // Аналоговый режим
		};

		// Перечисление настроек подтягивающих резисторов
		enum class Pull : uint32_t {
			NoPull,   // Без подтягивания
			PullUp,   // Подтягивание к питанию
			PullDown  // Подтягивание к земле
		};

		// Перечисление типов выходного сигнала
		enum class OutputType : uint32_t {
			PushPull,   // Тип "толкать-тянуть"
			OpenDrain   // Открытый сток
		};

		// Перечисление скоростей выходного сигнала
		enum class OutputSpeed : uint32_t {
			Low,        // Низкая скорость
			Medium,     // Средняя скорость
			High,       // Высокая скорость
			VeryHigh    // Очень высокая скорость
		};

		// Перечисление возможных ошибок в настройках GPIO
		enum class Error : error_t {
			PeripheralDisabled, // Переферия GPIO отключена
			PinNumberError		// Некорректный номер пина
		};

		// Структура для хранения настроек GPIO
		struct Settings
		{
		private:
			Mode _mode;				// Режим порта
			Pull _pull;             // Настройка подтягивающего резистора
			OutputType _outputType; // Тип выходного сигнала
			OutputSpeed _outputSpeed; // Скорость выходного сигнала

		public:
			// Геттеры для получения настроек
			Mode getMode() const { return _mode; }
			Pull getPull() const { return _pull; }
			OutputType getOutputType() const { return _outputType; }
			OutputSpeed getOutputSpeed() const { return _outputSpeed; }

			// Конструктор с параметрами по умолчанию
			Settings(
				Mode mode = Mode::Output, // значение по умолчанию для режима
				Pull pull = Pull::NoPull, // значение по умолчанию для подтягивающего резистора
				OutputType outputType = OutputType::PushPull, // значение по умолчанию для типа выходного сигнала
				OutputSpeed outputSpeed = OutputSpeed::Low // значение по умолчанию для скорости выходного сигнала
			) : _mode(mode), _pull(pull), _outputType(outputType), _outputSpeed(outputSpeed) {}

			//Функции билдера
			Settings& setMode(Mode mode) { _mode = mode; return *this; }
			Settings& setPull(Pull pull) { _pull = pull; return *this; }
			Settings& setOutputType(OutputType outputType) { _outputType = outputType; return *this; }
			Settings& setOutputSpeed(OutputSpeed outputSpeed) { _outputSpeed = outputSpeed; return *this; }

			// Операторы сравнения
			bool operator==(const Settings& other) const {
				return _mode == other._mode && _pull == other._pull && _outputType == other._outputType && _outputSpeed == other._outputSpeed;
			}

			bool operator!=(const Settings& other) const {
				return !(*this == other);
			}
		};

		typedef uint32_t pin_number_t;  // Тип данных для номера пина
		typedef std::function<void(pin_number_t)> ExternalInterruptCallback_t;  // Тип функции обратного вызова для внешних прерываний

		template <uint32_t IOCount> // Максимальный номер пина
		class BaseGpio : public ControllerPeripheral {
		public:
			static constexpr pin_number_t PinMaxNumber = IOCount - 1;

			virtual ~BaseGpio() = default;

			// Метод инициализации контроллера DMA
			virtual void init() override {
				onEnableClock(); // Включение тактирования контроллера DMA
			}

			// Метод деинициализации контроллера DMA
			virtual void deInit() override {
				onDisableClock(); // Отключение тактирования контроллера DMA
			}

			// Инициализирует пин с указанными настройками
			bool initPin(pin_number_t pin, const Settings& settings = Settings()) {
				if(!isEnabled()){
					onError(Error::PeripheralDisabled); // Обработка ошибки: контроллер DMA отключен
					return false;
				}
				if (pin > PinMaxNumber) {
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
					return false;
				}

				return onSetSettings(pin, settings);
			}

			// Обновляет настройки указанного пина
			virtual bool updateSettings(pin_number_t pin, const Settings& settings = Settings()) {
				if (!isEnabled()) {
					onError(Error::PeripheralDisabled); // Обработка ошибки: контроллер DMA отключен
					return false;
				}
				if (pin > PinMaxNumber) {
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
					return false;
				}

				return onUpdateSettings(pin, settings);
			};

			// Возвращает текущие настройки указанного пина
			virtual Settings getSettings(pin_number_t pin) const {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина

				return onGetSettings(pin);
			};

			// Устанавливает логическую единицу на указанном пине
			void setPin(pin_number_t pin) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				onSetPin(pin);
			}

			// Устанавливает логический ноль на указанном пине
			void resetPin(pin_number_t pin) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				onResetPin(pin);
			}

			// Переключает состояние указанного пина на противоположное
			void togglePin(pin_number_t pin) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				if(onGetPinOutput(pin))
					onResetPin(pin);
				else
					onSetPin(pin);
			}

			// Читает текущее входное состояние указанного пина
			virtual bool readPinInput(pin_number_t pin) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				return onGetPinInput(pin);
			}

			// Читает текущее входное состояние указанного пина
			virtual bool readPinOutput(pin_number_t pin) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				return onGetPinOutput(pin);
			}

			// Устанавливает тип подтяжки для пина
			virtual void setPull(pin_number_t pin, Pull pull) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				onSetPull(pin, pull);
			}

			// Устанавливает режим работы пина
			virtual void setMode(pin_number_t pin, Mode mode) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				onSetMode(pin, mode);
			}

			// Устанавливает тип выходного сигнала пина
			virtual void setOutputType(pin_number_t pin, OutputType outputType) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				onSetOutputType(pin, outputType);
			}

			// Устанавливает выходную скорость работы пина
			virtual void setOutputSpeed(pin_number_t pin, OutputSpeed speed) {
				if (pin > PinMaxNumber)
					onError(Error::PinNumberError); // Обработка ошибки: некорректный номер пина
				onSetOutputSpeed(pin, speed);
			}

			// Устанавливает внешний обработчик прерываний
			void setInterruptCallback(ExternalInterruptCallback_t callback) {
				interruptCallback = std::move(callback);
			}

			// Удаляет внешний обработчик прерываний
			void clearInterruptCallback() {
				interruptCallback = nullptr;
			}

		protected:
			// Возвращает битовую маску для указанного пина
			static constexpr uint32_t getPinMask(pin_number_t pin) {
				return 1 << pin;
			}

			//Виртуальные функции применения настроек
			// 
			//Виртуальный метод установки настроек пина GPIO (должен быть реализован в наследнике)
			virtual bool onSetSettings(pin_number_t, const Settings&) = 0;

			//Виртуальный метод получения настроек пина GPIO (должен быть реализован в наследнике)
			virtual bool onGetSettings(pin_number_t) = 0;

			//Виртуальный метод обновления настроек пина GPIO (должен быть реализован в наследнике)
			virtual bool onUpdateSettings(pin_number_t, const Settings&) = 0;

			//Виртуальный метод установки логической единицы на пине GPIO (должен быть реализован в наследнике)
			virtual void onSetPin(pin_number_t, bool) = 0;

			//Виртуальный метод установки логического нуля на пине GPIO (должен быть реализован в наследнике)
			virtual void onResetPin(pin_number_t, bool) = 0;

			//Виртуальный метод установки подтяжки пина GPIO (должен быть реализован в наследнике)
			virtual void onSetPull(pin_number_t, Pull) = 0;

			//Виртуальный метод установки режима работы пина GPIO (должен быть реализован в наследнике)
			virtual void onSetMode(pin_number_t, Mode) = 0;

			//Виртуальный метод установки типа выходно сигнала пина GPIO (должен быть реализован в наследнике)
			virtual void onSetOutputType(pin_number_t, OutputType) = 0;

			//Виртуальный метод установки скорости выходно сигнала пина GPIO (должен быть реализован в наследнике)
			virtual void onSetOutputSpeed(pin_number_t, OutputSpeed) = 0;

			//Функции получения установленных настроек
			//
			// Виртуальный метод получения состояние выхода пина (должен быть реализован в наследнике)
			virtual bool onGetPinOutput(pin_number_t) = 0;

			// Виртуальный метод получения состояние входа пина (должен быть реализован в наследнике)
			virtual bool onGetPinInput(pin_number_t) = 0;

			// Виртуальный метод получения режима работы пина (должен быть реализован в наследнике)
			virtual Mode getMode(pin_number_t) const = 0;

			// Виртуальный метод получения подтяжки пина (должен быть реализован в наследнике)
			virtual Pull getPull(pin_number_t) const = 0;

			// Виртуальный метод получения типа выходно сигнала пина (должен быть реализован в наследнике)
			virtual OutputType getOutputType(pin_number_t) const = 0;

			// Виртуальный метод получения скорости выходно сигнала пина (должен быть реализован в наследнике)
			virtual OutputSpeed getOutputSpeed(pin_number_t) const = 0;

			// Виртуальный метод валидации порта GPIO (должен быть реализован в наследнике)
			virtual bool validatePortAddr(uint32_t portAddr) const = 0;

			//Внешний обработчик прерываний (может быть реализован как в наследнике, так и из объекта)
			ExternalInterruptCallback_t interruptCallback;
		};
	}
}

#endif