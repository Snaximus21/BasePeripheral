#ifndef DMA_HPP_
#define DMA_HPP_

#include <iostream>
#include <functional>
#include "ControllerPeripheral.hpp"
#include "SharedMacro.hpp"

namespace BasePeripheral {
	namespace Dma {
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

		// Перечисление для ошибок DMA
		enum class Error : error_t {
			PeripheralDisabled, // Переферия DMA отключена
			ChannelNumberError  // Некорректный номер канала
		};

		typedef uint32_t address_t;

		// Структура для хранения настроек памяти
		struct MemorySettings {
			address_t _memAddr;         // Адрес памяти
			DataAlign _dataAlign;       // Выравнивание данных
			IncrementMode _incMode;     // Режим инкрементации адресов

			// Геттеры для получения настроек
			address_t getAddr() const { return _memAddr; }
			DataAlign getDataAlign() const { return _dataAlign; }
			IncrementMode getIncMode() const { return _incMode; }

			// Конструктор с параметрами по умолчанию
			MemorySettings(
				address_t memAddr = 0,
				DataAlign dataAlign = DataAlign::Byte, // Значение по умолчанию для режима
				IncrementMode incMode = IncrementMode::NoIncrement
			) : _memAddr(memAddr), _dataAlign(dataAlign), _incMode(incMode) {}

			// Функции билдера для настройки параметров
			MemorySettings& setAddr(address_t memAddr) { _memAddr = memAddr; return *this; }
			MemorySettings& setDataAlign(DataAlign dataAlign) { _dataAlign = dataAlign; return *this; }
			MemorySettings& setIncMode(IncrementMode incMode) { _incMode = incMode; return *this; }

			// Операторы сравнения
			bool operator==(const MemorySettings& other) const {
				return _memAddr == other._memAddr && _dataAlign == other._dataAlign && _incMode == other._incMode;
			}

			bool operator!=(const MemorySettings& other) const {
				return !(*this == other);
			}
		};

		// Структура для хранения общих настроек DMA
		struct Settings {
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

			// Функции билдера для настройки параметров
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

		typedef uint32_t channel_number_t;

		//Максимальное количество каналов в блоке DMA
		template <uint32_t ChannelsCount> 
		class BaseDma : public ControllerPeripheral {
		public:
			static constexpr channel_number_t ChannelMaxNumber = ChannelsCount - 1;

			virtual ~BaseDma() = default;

			// Метод инициализации контроллера DMA
			virtual void init() override {
				onEnableClock(); // Включение тактирования контроллера DMA
			}

			// Метод деинициализации контроллера DMA
			virtual void deInit() override {
				onDisableClock(); // Отключение тактирования контроллера DMA
			}

			// Метод для инициализации канала с проверкой валидности входных данных
			bool initChannel(channel_number_t channel, const Settings& settings = Settings()) {
				if (!isEnabled()) {
					onError(Error::PeripheralDisabled); // Обработка ошибки: контроллер DMA отключен
					return false;
				}
				if (channel > ChannelMaxNumber) {
					onError(Error::ChannelNumberError); // Обработка ошибки: некорректный номер канала
					return false;
				}
				return onSetSettings(channel, settings); // Применение настроек к каналу
			}

			// Метод для установки направления передачи данных с проверкой валидности входных данных
			void setDirection(channel_number_t channel, Direction direction) {
				if (channel > ChannelMaxNumber) {
					onError(Error::ChannelNumberError); // Обработка ошибки: некорректный номер канала
					return;
				}
				onSetDirection(channel, direction); // Установка направления передачи данных
			}

			// Метод для установки режима передачи данных с проверкой валидности входных данных
			void setMode(channel_number_t channel, Mode mode) {
				if (channel > ChannelMaxNumber) {
					onError(Error::ChannelNumberError); // Обработка ошибки: некорректный номер канала
					return;
				}
				onSetMode(channel, mode); // Установка режима передачи данных
			}

			// Метод для установки приоритета работы канала с проверкой валидности входных данных
			void setPriority(channel_number_t channel, Priority priority) {
				if (channel > ChannelMaxNumber) {
					onError(Error::ChannelNumberError); // Обработка ошибки: некорректный номер канала
					return;
				}
				onSetPriority(channel, priority); // Установка приоритета работы канала
			}

			// Метод для установки настроек памяти с проверкой валидности входных данных
			void setMemorySettings(channel_number_t channel, const MemorySettings& src, const MemorySettings& dst) {
				if (channel > ChannelMaxNumber) {
					onError(Error::ChannelNumberError); // Обработка ошибки: некорректный номер канала
					return;
				}
				onSetMemorySettings(channel, src, dst); // Установка настроек памяти
			}

		protected:

			//Виртуальный метод установки настроек канала DMA (должен быть реализован в наследнике)
			virtual bool onSetSettings(channel_number_t, const Settings&) = 0;

			//Виртуальный метод установки направления передачи канала DMA (должен быть реализован в наследнике)
			virtual void onSetDirection(channel_number_t, Direction) = 0;

			//Виртуальный метод установки режима передачи канала DMA (должен быть реализован в наследнике)
			virtual void onSetMode(channel_number_t, Mode) = 0;

			//Виртуальный метод установки приоритета передачи канала DMA (должен быть реализован в наследнике)
			virtual void onSetPriority(channel_number_t, Priority) = 0;

			//Виртуальный метод установки настроек памяти канала DMA (должен быть реализован в наследнике)
			virtual void onSetMemorySettings(channel_number_t, const MemorySettings&, const MemorySettings&) = 0;

			//Виртуальный метод включения передачи данных канала DMA (должен быть реализован в наследнике)
			virtual void onEnableChannel(channel_number_t) = 0;

			//Виртуальный метод выключения передачи данных канала DMA (должен быть реализован в наследнике)
			virtual void onDisableChannel(channel_number_t) = 0;
		};
	}
}

#endif