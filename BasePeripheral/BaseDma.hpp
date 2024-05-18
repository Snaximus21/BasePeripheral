#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Dma {

		typedef uint32_t address_t;
		typedef uint32_t channel_number_t;

		// ������������ ��� ����������� �������� ������
		enum class Direction {
			PeriphToMemory, // �������� ������ �� ��������� � ������
			MemoryToPeriph, // �������� ������ �� ������ � ���������
			MemoryToMemory  // �������� ������ �� ������ � ������
		};

		// ������������ ��� ������ ������ DMA
		enum class Mode {
			Normal,   // ������� ����� �������� ������
			Circular  // ����������� ����� �������� ������
		};

		// ������������ ��� ������ ������������� �������
		enum class IncrementMode {
			NoIncrement, // ��� �������������
			Increment    // � ��������������
		};

		// ������������ ��� ������������ ������
		enum class DataAlign {
			Byte,      // ������������ �� �����
			HalfWord,  // ������������ �� ��������� (2 �����)
			Word       // ������������ �� ����� (4 �����)
		};

		// ������������ ��� ���������� DMA
		enum class Priority {
			Low,      // ������ ���������
			Medium,   // ������� ���������
			High,     // ������� ���������
			VeryHigh  // ����� ������� ���������
		};

		enum class Error : error_t {
			ChannelDisabled,	 //��������� DMA ���������
			ChannelNumberError,  //������������ ����� ������
		};

		typedef struct MemorySettings {
			address_t _memAddr;         // ����� ������
			DataAlign _dataAlign;       // ������������ ������
			IncrementMode _incMode;     // ����� ������������� �������

			// ������� ��� ��������� ��������
			address_t getAddr() const { return _memAddr; }
			DataAlign getDataAlign() const { return _dataAlign; }
			IncrementMode getPriority() const { return _incMode; }

			// ����������� � ����������� �� ���������
			MemorySettings(
				address_t memAddr = 0,
				DataAlign dataAlign = DataAlign::Byte, // �������� �� ��������� ��� ������
				IncrementMode incMode = IncrementMode::NoIncrement
			) : _memAddr(memAddr), _dataAlign(dataAlign), _incMode(incMode) {}

			// ��������� ���������
			bool operator==(const MemorySettings& other) const {
				return _memAddr == other._memAddr && _dataAlign == other._dataAlign && _incMode == other._incMode;
			}

			bool operator!=(const MemorySettings& other) const {
				return !(*this == other);
			}
		};

		// ��������� ��� �������� �������� DMA
		struct Settings
		{
		private:
			Direction _direction;                  // ����������� �������� ������
			Mode _mode;                            // ����� ������ DMA
			Priority _priority;                    // ��������� DMA
			MemorySettings _periphOrMemToMemSrc;   // ��������� ��������� ������ (��������� ��� ������)
			MemorySettings _memoryOrMemToMemDst;   // ��������� ���������� ������ (������)

		public:
			// ������� ��� ��������� ��������
			Direction getDirection() const { return _direction; }
			Mode getMode() const { return _mode; }
			Priority getPriority() const { return _priority; }
			MemorySettings getPeriphOrMemToMemSrc() const { return _periphOrMemToMemSrc; }
			MemorySettings getMemoryOrMemToMemDst() const { return _memoryOrMemToMemDst; }

			// ����������� � ����������� �� ���������
			Settings(
				Direction direction = Direction::PeriphToMemory,
				Mode mode = Mode::Normal,
				Priority priority = Priority::Low,
				MemorySettings periphOrMemToMemSrc = MemorySettings(),
				MemorySettings memoryOrMemToMemDst = MemorySettings()
			) : _direction(direction), _mode(mode), _priority(priority), _periphOrMemToMemSrc(periphOrMemToMemSrc), _memoryOrMemToMemDst(memoryOrMemToMemDst) {}

			//������� �������
			Settings& setDirection(Direction direction) { _direction = direction; return *this; }
			Settings& setMode(Mode mode) { _mode = mode; return *this; }
			Settings& setPriority(Priority priority) { _priority = priority; return *this; }
			Settings& setPeriphOrMemToMemSrc(MemorySettings memorySettings) { _periphOrMemToMemSrc = memorySettings; return *this; }
			Settings& setMemoryOrMemToMemDst(MemorySettings memorySettings) { _memoryOrMemToMemDst = memorySettings; return *this; }

			// ��������� ���������
			bool operator==(const Settings& other) const {
				return _direction == other._direction && 
					_mode == other._mode && 
					_priority == other._priority && 
					_periphOrMemToMemSrc == other._periphOrMemToMemSrc && 
					_memoryOrMemToMemDst == other._memoryOrMemToMemDst;
			}

			bool operator!=(const Settings& other) const {
				return !(*this == other);
			}
		};

		template <uint32_t ChannelsCount> // ������������ ����� ����
		class BaseDma : public ControllerPeripheral {
			static constexpr channel_number_t ChannelMaxNumber = ChannelsCount - 1;

			virtual ~BaseDma() = default;

			virtual bool isEnabled() const override {
				return false;
			}

			virtual void init() override {
				enableClock();
			}

			//�������� ���������� ������� ������ � ��������� ������
			bool initChannel(channel_number_t channel, const Settings& settings = Settings()) {
				if (!isEnabled) {
					onError(Error::ChannelDisabled);
					return false;
				}
				if (channel > ChannelMaxNumber) {
					onError(Error::ChannelNumberError);
					return false;
				}
				return applySettings(channel, settings);
			}

			//�������� ���������� ������� ������ � ��������� ����������� ��������
			void setDirection(channel_number_t channel, Direction direction) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);
				
				onSetDirection(channel, direction);
			}

			//�������� ���������� ������� ������ � ��������� ������ ��������
			void setMode(channel_number_t channel, Mode mode) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);
				
				onSetMode(channel, mode);
			}

			//�������� ���������� ������� ������ � ��������� ���������� ������ ������
			void setPriority(channel_number_t channel, Priority priority) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);
	
				onSetPriority(channel, priority);
			}

			//�������� ���������� ������� ������ � ��������� �������� ������
			void setMemorySettings(channel_number_t channel, const MemorySettings& src, const MemorySettings& dst) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);

				onSetMemorySettings(channel, src, dst);
			}

		protected:

			//��������� ������
			virtual bool onSetSettings(channel_number_t channel, const Settings& settings) = 0;

			//��������� ����������� ��������
			virtual void onSetDirection(channel_number_t channel, Direction direction) = 0;

			//��������� ������ ��������
			virtual void onSetMode(channel_number_t channel, Mode mode) = 0;

			//��������� ���������� ������ ������
			virtual void onSetPriority(channel_number_t channel, Priority priority) = 0;

			//��������� �������� ������
			virtual void onSetMemorySettings(channel_number_t channel, const MemorySettings& src, const MemorySettings& dst) = 0;
		};
	}
}