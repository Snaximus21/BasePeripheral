#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Gpio {
		// Определение режимов работы порта GPIO
		enum class Mode : uint32_t {
			Input,             // Режим входа
			Output,            // Режим выхода
			AlternateFunction, // Режим альтернативной функции
			Analog             // Аналоговый режим
		};

		// Определение настроек подтягивающих резисторов
		enum class Pull : uint32_t {
			NoPull,   // Без подтягивания
			PullUp,   // Подтягивание к питанию
			PullDown  // Подтягивание к земле
		};

		// Определение типов выходного сигнала
		enum class OutputType : uint32_t {
			PushPull,   // Тип "толкать-тянуть"
			OpenDrain   // Открытый сток
		};

		// Определение скоростей выходного сигнала
		enum class OutputSpeed : uint32_t {
			Low,        // Низкая скорость
			Medium,     // Средняя скорость
			High,       // Высокая скорость
			VeryHigh    // Очень высокая скорость
		};

		// Определение возможных ошибок в настройках GPIO
		enum class Error {
			None,
			InitError,  // Ошибка инициализации
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


			// Операторы сравнения
			bool operator==(const Settings& other) const {
				return _mode == other._mode && _pull == other._pull && _outputType == other._outputType && _outputSpeed == other._outputSpeed;
			}

			bool operator!=(const Settings& other) const {
				return !(*this == other);
			}
		};

		typedef uint32_t pin_number_t;  // Тип данных для номера пина
		typedef std::function<void(pin_number_t pin)> ExternalInterruptCallback_t;  // Тип функции обратного вызова для внешних прерываний

		template <uint32_t IOCount> // Максимальный номер пина
		class BaseGpio : public ControllerPeripheral {
		public:
			static constexpr pin_number_t PinMaxNumber = IOCount;

			virtual ~BaseGpio() {}

			virtual bool isEnabled() const override {
				return false;
			}

			virtual void init() override {
				enableClock();
			}

			// Инициализирует пин с указанными настройками
			Error initPin(pin_number_t pin, const Settings& settings = Settings()) {
				RG_ASSERT_MSG(isEnabled(), "GPIO interface currenty disabled.");
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");

				auto mode = settings.getMode();
				if (mode == Mode::Output || mode == Mode::AlternateFunction) {
					setOutputSpeed(pin, settings.getOutputSpeed());
					setOutputType(pin, settings.getOutputType());
				}

				setPull(pin, settings.getPull());

				if (mode == Mode::AlternateFunction) {
					//SetAlternateFunctions
				}

				setMode(pin, settings.getMode());
				return Error::None;
			}

			// Устанавливает логическую единицу на указанном пине
			void setPin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				setPinOutputState(pin, true);
			}

			// Устанавливает логический ноль на указанном пине
			void resetPin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				setPinOutputState(pin, false);
			}

			// Переключает состояние указанного пина на противоположное
			void togglePin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				bool pinState = getPinOutputState(pin);
				setPinOutputState(pin, !pinState);
			}

			// Читает текущее входное состояние указанного пина
			virtual bool readPin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				return getPinInputState(pin);
			}

			// Обновляет настройки указанного пина, в случае если настройки некорректны - возвращает ошибку
			virtual Error updateSettings(pin_number_t, const Settings&) { return Error(); };
			// Возвращает текущие настройки указанного пина
			virtual Settings getSettings(pin_number_t) const { return Settings(); };

		protected:

			// Возвращает битовую маску для указанного пина
			static constexpr uint32_t getPinMask(pin_number_t pin) {
				return 1 << pin;
			}

			// Устанавливает состояние выхода пина
			virtual void setPinOutputState(pin_number_t, bool) = 0;
			// Возвращает состояние выхода пина
			virtual bool getPinOutputState(pin_number_t) = 0;

			// Возвращает состояние входа пина
			virtual bool getPinInputState(pin_number_t) = 0;

			// Устанавливает режим работы пина
			virtual void setMode(pin_number_t, Mode) = 0;
			// Вовзращает режим работы пина
			virtual Mode getMode(pin_number_t) const {
				return Mode();
			};

			// Устанавливает тип подтяжки для пина
			virtual void setPull(pin_number_t, Pull) = 0;
			// Возвращает тип подтяжки для пина
			virtual Pull getPull(pin_number_t) const {
				return Pull();
			};

			// Устанавливает тип выходного сигнала пина
			virtual void setOutputType(pin_number_t, OutputType) = 0;
			// Возвращает тип выходного сигнала пина
			virtual OutputType getOutputType(pin_number_t) const {
				return OutputType();
			};

			// Устанавливает выходную скорость работы пина
			virtual void setOutputSpeed(pin_number_t, OutputSpeed) = 0;
			// Вовзращает выходную скорость работы пина
			virtual OutputSpeed getOutputSpeed(pin_number_t) const {
				return OutputSpeed();
			};

			ExternalInterruptCallback_t interruptCallback;

			BaseGpio() {}
		};
	}
}