#ifndef USB_AUDIO_CDC_HPP
#define USB_AUDIO_CDC_HPP

#include <stddef.h>
#include <stdint.h>

#include "stm32h743xx.h"

namespace fx4cr {
namespace usb {

struct UsbDeviceId {
	uint16_t vendorId;
	uint16_t productId;
	uint16_t deviceBcd;
	const char *manufacturer;
	const char *product;
	const char *serial;
};

struct AudioLine48k24Callbacks {
	void (*outputSamples)(const int32_t *samples, uint16_t frames, void *context);
	uint16_t (*inputSamples)(int32_t *samples, uint16_t maxFrames, void *context);
	void *context;
};

struct CdcLineCoding {
	uint32_t baudRate;
	uint8_t stopBits;
	uint8_t parity;
	uint8_t dataBits;
};

class UsbCdcAcm {
public:
	enum { RxBufferSize = 512, TxBufferSize = 512 };

	void reset();
	uint16_t available() const;
	uint16_t read(uint8_t *data, uint16_t length);
	uint16_t write(const uint8_t *data, uint16_t length);
	const CdcLineCoding &lineCoding() const;
	uint16_t controlLineState() const;

private:
	friend class UsbCompositeAudioCdc;

	uint16_t pushRx(const uint8_t *data, uint16_t length);
	uint16_t popTx(uint8_t *data, uint16_t length);
	uint16_t txPending() const;
	void setLineCoding(const uint8_t *data);
	void getLineCoding(uint8_t *data) const;

	uint8_t rxBuffer[RxBufferSize];
	uint8_t txBuffer[TxBufferSize];
	uint16_t rxHead;
	uint16_t rxTail;
	uint16_t txHead;
	uint16_t txTail;
	uint16_t ctrlState;
	CdcLineCoding coding;
};

class UsbAudioLine48k24 {
public:
	enum { SampleRate = 48000, BitsPerSample = 24, Channels = 2, FramesPerMs = 48 };
	enum { BytesPerFrame = Channels * 3, PacketBytes = FramesPerMs * BytesPerFrame };

	void reset();
	void setCallbacks(const AudioLine48k24Callbacks *callbacks);
	void setOutputEnabled(bool enabled);
	void setInputEnabled(bool enabled);
	bool outputEnabled() const;
	bool inputEnabled() const;

private:
	friend class UsbCompositeAudioCdc;

	void receivedOutputPacket(const uint8_t *data, uint16_t length);
	uint16_t buildInputPacket(uint8_t *data, uint16_t maxLength);

	AudioLine48k24Callbacks callbacks;
	bool outEnabled;
	bool inEnabled;
};

class UsbCompositeAudioCdc {
public:
	enum {
		Ep0Size = 64,
		CdcInterruptPacketSize = 8,
		CdcDataPacketSize = 64,
		AudioPacketSize = UsbAudioLine48k24::PacketBytes
	};

	void init(const UsbDeviceId *deviceId = 0, const AudioLine48k24Callbacks *audioCallbacks = 0);
	void irq();
	void poll();
	bool configured() const;

	UsbCdcAcm &cdc();
	UsbAudioLine48k24 &audio();

private:
	struct SetupPacket {
		uint8_t bmRequestType;
		uint8_t bRequest;
		uint16_t wValue;
		uint16_t wIndex;
		uint16_t wLength;
	};

	enum ControlOutRequest {
		ControlOutNone,
		ControlOutCdcLineCoding
	};

	void resetState();
	void initPinsAndClock();
	void initCore();
	void openControlEndpoint();
	void openClassEndpoints();
	void flushRxFifo();
	void flushTxFifo(uint8_t fifo);
	void prepareOut(uint8_t ep, uint16_t length);
	void prepareEp0Setup();
	void startIn(uint8_t ep, const uint8_t *data, uint16_t length);
	void writeFifo(uint8_t ep, const uint8_t *data, uint16_t length);
	void readFifo(uint8_t *data, uint16_t length);
	void handleRxFifo();
	void handleInInterrupts();
	void handleOutInterrupts();
	void handleSetup(const SetupPacket &setup);
	void handleStandardRequest(const SetupPacket &setup);
	void handleClassRequest(const SetupPacket &setup);
	void handleEp0OutData(const uint8_t *data, uint16_t length);
	void sendControlData(const uint8_t *data, uint16_t length);
	void sendControlStatus();
	void stallControl();
	void finishEp0In();
	void setAddress(uint8_t address);
	void setConfiguration(uint8_t value);
	void serviceCdcIn();
	void serviceAudioIn();
	const uint8_t *descriptor(uint8_t type, uint8_t index, uint16_t *length);
	const uint8_t *stringDescriptor(uint8_t index, uint16_t *length);
	void buildDeviceDescriptor();

	UsbCdcAcm cdcClass;
	UsbAudioLine48k24 audioClass;
	UsbDeviceId id;
	uint8_t deviceDescriptor[18];
	uint8_t stringBuffer[64];
	uint8_t ep0Buffer[Ep0Size];
	uint8_t cdcTxPacket[CdcDataPacketSize];
	uint8_t audioPacket[AudioPacketSize];
	const uint8_t *ep0TxData;
	uint16_t ep0TxRemaining;
	ControlOutRequest controlOut;
	uint8_t pendingAddress;
	uint8_t hasPendingAddress;
	uint8_t configuration;
	uint8_t epInBusy[4];
};

UsbCompositeAudioCdc &device();

} // namespace usb
} // namespace fx4cr

#endif
