#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Dma {

		typedef uint32_t address_t;

		enum class Direction {
			PeriphToMemory,
			MemoryToPeriph,
			MemoryToMemory
		};

		enum class Mode {
			Normal,
			Circular
		};

		enum class IncrementMode {
			NoIncrement,
			Increment
		};

		enum class DataAlign {
			Byte,
			HalfWord,
			Word
		};

		enum class Priority {
			Low,
			Medium,
			High,
			VeryHigh
		};

		typedef struct MemorySettings {
			address_t _memAddr;
			DataAlign _dataAlign;
			IncrementMode _incMode;

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

		// ��������� ��� �������� �������� GPIO
		struct Settings
		{
		private:
			Direction _direction;
			Mode _mode;
			Priority _priority;
			MemorySettings _periphOrMemToMemSrc;
			MemorySettings _memoryOrMemToMemDst;

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
				Mode mode = Mode::Normal, // �������� �� ��������� ��� ������
				Priority priority = Priority::Low,
				MemorySettings periphOrMemToMemSrc = MemorySettings(),
				MemorySettings memoryOrMemToMemDst = MemorySettings()
			) : _direction(direction), _mode(mode), _priority(priority), _periphOrMemToMemSrc(periphOrMemToMemSrc), _memoryOrMemToMemDst(memoryOrMemToMemDst) {}


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

		class BaseDma : public ControllerPeripheral {



		};
	}
}