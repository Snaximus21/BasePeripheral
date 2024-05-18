#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Dma {

		typedef uint32_t address_t;
		typedef uint32_t channel_number_t;

		// Перечисление для направления передачи данных
		enum class Direction {
			PeriphToMemory, // Передача данных от периферии к памяти
			MemoryToPeriph, // Передача данных от памяти к периферии
			MemoryToMemory  // Передача данных от памяти к памяти
		};

		// Перечисление для режима работы DMA
		enum class Mode {
			Normal,   // Обычный режим передачи данных
			Circular  // Циклический режим передачи данных
		};

		// Перечисление для режима инкрементации адресов
		enum class IncrementMode {
			NoIncrement, // Без инкрементации
			Increment    // С инкрементацией
		};

		// Перечисление для выравнивания данных
		enum class DataAlign {
			Byte,      // Выравнивание по байту
			HalfWord,  // Выравнивание по полуслову (2 байта)
			Word       // Выравнивание по слову (4 байта)
		};

		// Перечисление для приоритета DMA
		enum class Priority {
			Low,      // Низкий приоритет
			Medium,   // Средний приоритет
			High,     // Высокий приоритет
			VeryHigh  // Очень высокий приоритет
		};

		enum class Error : error_t {
			ChannelDisabled,	 //Переферия DMA отключена
			ChannelNumberError,  //Некорректный номер канала
		};

		typedef struct MemorySettings {
			address_t _memAddr;         // Адрес памяти
			DataAlign _dataAlign;       // Выравнивание данных
			IncrementMode _incMode;     // Режим инкрементации адресов

			// Геттеры для получения настроек
			address_t getAddr() const { return _memAddr; }
			DataAlign getDataAlign() const { return _dataAlign; }
			IncrementMode getPriority() const { return _incMode; }

			// Конструктор с параметрами по умолчанию
			MemorySettings(
				address_t memAddr = 0,
				DataAlign dataAlign = DataAlign::Byte, // значение по умолчанию для режима
				IncrementMode incMode = IncrementMode::NoIncrement
			) : _memAddr(memAddr), _dataAlign(dataAlign), _incMode(incMode) {}

			// Операторы сравнения
			bool operator==(const MemorySettings& other) const {
				return _memAddr == other._memAddr && _dataAlign == other._dataAlign && _incMode == other._incMode;
			}

			bool operator!=(const MemorySettings& other) const {
				return !(*this == other);
			}
		};

		// Структура для хранения настроек DMA
		struct Settings
		{
		private:
			Direction _direction;                  // Направление передачи данных
			Mode _mode;                            // Режим работы DMA
			Priority _priority;                    // Приоритет DMA
			MemorySettings _periphOrMemToMemSrc;   // Настройки источника данных (периферия или память)
			MemorySettings _memoryOrMemToMemDst;   // Настройки назначения данных (память)

		public:
			// Геттеры для получения настроек
			Direction getDirection() const { return _direction; }
			Mode getMode() const { return _mode; }
			Priority getPriority() const { return _priority; }
			MemorySettings getPeriphOrMemToMemSrc() const { return _periphOrMemToMemSrc; }
			MemorySettings getMemoryOrMemToMemDst() const { return _memoryOrMemToMemDst; }

			// Конструктор с параметрами по умолчанию
			Settings(
				Direction direction = Direction::PeriphToMemory,
				Mode mode = Mode::Normal,
				Priority priority = Priority::Low,
				MemorySettings periphOrMemToMemSrc = MemorySettings(),
				MemorySettings memoryOrMemToMemDst = MemorySettings()
			) : _direction(direction), _mode(mode), _priority(priority), _periphOrMemToMemSrc(periphOrMemToMemSrc), _memoryOrMemToMemDst(memoryOrMemToMemDst) {}

			//Функции билдера
			Settings& setDirection(Direction direction) { _direction = direction; return *this; }
			Settings& setMode(Mode mode) { _mode = mode; return *this; }
			Settings& setPriority(Priority priority) { _priority = priority; return *this; }
			Settings& setPeriphOrMemToMemSrc(MemorySettings memorySettings) { _periphOrMemToMemSrc = memorySettings; return *this; }
			Settings& setMemoryOrMemToMemDst(MemorySettings memorySettings) { _memoryOrMemToMemDst = memorySettings; return *this; }

			// Операторы сравнения
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

		template <uint32_t ChannelsCount> // Максимальный номер пина
		class BaseDma : public ControllerPeripheral {
			static constexpr channel_number_t ChannelMaxNumber = ChannelsCount - 1;

			virtual ~BaseDma() = default;

			virtual bool isEnabled() const override {
				return false;
			}

			virtual void init() override {
				enableClock();
			}

			//Проверка валидности входных данных и настройка канала
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

			//Проверка валидности входных данных и установка направления передачи
			void setDirection(channel_number_t channel, Direction direction) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);
				
				onSetDirection(channel, direction);
			}

			//Проверка валидности входных данных и установка режима передачи
			void setMode(channel_number_t channel, Mode mode) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);
				
				onSetMode(channel, mode);
			}

			//Проверка валидности входных данных и установка приоритета работы канала
			void setPriority(channel_number_t channel, Priority priority) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);
	
				onSetPriority(channel, priority);
			}

			//Проверка валидности входных данных и установка настроек памяти
			void setMemorySettings(channel_number_t channel, const MemorySettings& src, const MemorySettings& dst) {
				if (channel > ChannelMaxNumber)
					onError(Error::ChannelNumberError);

				onSetMemorySettings(channel, src, dst);
			}

		protected:

			//Настройка канала
			virtual bool onSetSettings(channel_number_t channel, const Settings& settings) = 0;

			//Установка направления передачи
			virtual void onSetDirection(channel_number_t channel, Direction direction) = 0;

			//Установка режима передачи
			virtual void onSetMode(channel_number_t channel, Mode mode) = 0;

			//Установка приоритета работы канала
			virtual void onSetPriority(channel_number_t channel, Priority priority) = 0;

			//Установка настроек памяти
			virtual void onSetMemorySettings(channel_number_t channel, const MemorySettings& src, const MemorySettings& dst) = 0;
		};
	}
}