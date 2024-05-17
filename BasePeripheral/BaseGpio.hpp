#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Gpio {
		// ����������� ������� ������ ����� GPIO
		enum class Mode : uint32_t {
			Input,             // ����� �����
			Output,            // ����� ������
			AlternateFunction, // ����� �������������� �������
			Analog             // ���������� �����
		};

		// ����������� �������� ������������� ����������
		enum class Pull : uint32_t {
			NoPull,   // ��� ������������
			PullUp,   // ������������ � �������
			PullDown  // ������������ � �����
		};

		// ����������� ����� ��������� �������
		enum class OutputType : uint32_t {
			PushPull,   // ��� "�������-������"
			OpenDrain   // �������� ����
		};

		// ����������� ��������� ��������� �������
		enum class OutputSpeed : uint32_t {
			Low,        // ������ ��������
			Medium,     // ������� ��������
			High,       // ������� ��������
			VeryHigh    // ����� ������� ��������
		};

		// ����������� ��������� ������ � ���������� GPIO
		enum class Error {
			None,
			InitError,  // ������ �������������
		};

		// ��������� ��� �������� �������� GPIO
		struct Settings
		{
		private:
			Mode _mode;				// ����� �����
			Pull _pull;             // ��������� �������������� ���������
			OutputType _outputType; // ��� ��������� �������
			OutputSpeed _outputSpeed; // �������� ��������� �������

		public:
			// ������� ��� ��������� ��������
			Mode getMode() const { return _mode; }
			Pull getPull() const { return _pull; }
			OutputType getOutputType() const { return _outputType; }
			OutputSpeed getOutputSpeed() const { return _outputSpeed; }

			// ����������� � ����������� �� ���������
			Settings(
				Mode mode = Mode::Output, // �������� �� ��������� ��� ������
				Pull pull = Pull::NoPull, // �������� �� ��������� ��� �������������� ���������
				OutputType outputType = OutputType::PushPull, // �������� �� ��������� ��� ���� ��������� �������
				OutputSpeed outputSpeed = OutputSpeed::Low // �������� �� ��������� ��� �������� ��������� �������
			) : _mode(mode), _pull(pull), _outputType(outputType), _outputSpeed(outputSpeed) {}


			// ��������� ���������
			bool operator==(const Settings& other) const {
				return _mode == other._mode && _pull == other._pull && _outputType == other._outputType && _outputSpeed == other._outputSpeed;
			}

			bool operator!=(const Settings& other) const {
				return !(*this == other);
			}
		};

		typedef uint32_t pin_number_t;  // ��� ������ ��� ������ ����
		typedef std::function<void(pin_number_t pin)> ExternalInterruptCallback_t;  // ��� ������� ��������� ������ ��� ������� ����������

		template <uint32_t IOCount> // ������������ ����� ����
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

			// �������������� ��� � ���������� �����������
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

			// ������������� ���������� ������� �� ��������� ����
			void setPin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				setPinOutputState(pin, true);
			}

			// ������������� ���������� ���� �� ��������� ����
			void resetPin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				setPinOutputState(pin, false);
			}

			// ����������� ��������� ���������� ���� �� ���������������
			void togglePin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				bool pinState = getPinOutputState(pin);
				setPinOutputState(pin, !pinState);
			}

			// ������ ������� ������� ��������� ���������� ����
			virtual bool readPin(pin_number_t pin) {
				RG_ASSERT_MSG(pin <= PinMaxNumber, "Incorrect pin number.");
				return getPinInputState(pin);
			}

			// ��������� ��������� ���������� ����, � ������ ���� ��������� ����������� - ���������� ������
			virtual Error updateSettings(pin_number_t, const Settings&) { return Error(); };
			// ���������� ������� ��������� ���������� ����
			virtual Settings getSettings(pin_number_t) const { return Settings(); };

		protected:

			// ���������� ������� ����� ��� ���������� ����
			static constexpr uint32_t getPinMask(pin_number_t pin) {
				return 1 << pin;
			}

			// ������������� ��������� ������ ����
			virtual void setPinOutputState(pin_number_t, bool) = 0;
			// ���������� ��������� ������ ����
			virtual bool getPinOutputState(pin_number_t) = 0;

			// ���������� ��������� ����� ����
			virtual bool getPinInputState(pin_number_t) = 0;

			// ������������� ����� ������ ����
			virtual void setMode(pin_number_t, Mode) = 0;
			// ���������� ����� ������ ����
			virtual Mode getMode(pin_number_t) const {
				return Mode();
			};

			// ������������� ��� �������� ��� ����
			virtual void setPull(pin_number_t, Pull) = 0;
			// ���������� ��� �������� ��� ����
			virtual Pull getPull(pin_number_t) const {
				return Pull();
			};

			// ������������� ��� ��������� ������� ����
			virtual void setOutputType(pin_number_t, OutputType) = 0;
			// ���������� ��� ��������� ������� ����
			virtual OutputType getOutputType(pin_number_t) const {
				return OutputType();
			};

			// ������������� �������� �������� ������ ����
			virtual void setOutputSpeed(pin_number_t, OutputSpeed) = 0;
			// ���������� �������� �������� ������ ����
			virtual OutputSpeed getOutputSpeed(pin_number_t) const {
				return OutputSpeed();
			};

			ExternalInterruptCallback_t interruptCallback;

			BaseGpio() {}
		};
	}
}