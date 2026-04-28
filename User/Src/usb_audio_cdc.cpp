#include "usb_audio_cdc.hpp"

namespace fx4cr {
namespace usb {

namespace {

enum {
	USB_REQ_GET_STATUS = 0,
	USB_REQ_CLEAR_FEATURE = 1,
	USB_REQ_SET_ADDRESS = 5,
	USB_REQ_GET_DESCRIPTOR = 6,
	USB_REQ_GET_CONFIGURATION = 8,
	USB_REQ_SET_CONFIGURATION = 9,
	USB_REQ_GET_INTERFACE = 10,
	USB_REQ_SET_INTERFACE = 11,

	USB_DESC_DEVICE = 1,
	USB_DESC_CONFIGURATION = 2,
	USB_DESC_STRING = 3,
	USB_DESC_INTERFACE = 4,
	USB_DESC_ENDPOINT = 5,
	USB_DESC_DEVICE_QUALIFIER = 6,
	USB_DESC_IAD = 11,
	USB_DESC_CS_INTERFACE = 0x24,
	USB_DESC_CS_ENDPOINT = 0x25,

	CDC_REQ_SET_LINE_CODING = 0x20,
	CDC_REQ_GET_LINE_CODING = 0x21,
	CDC_REQ_SET_CONTROL_LINE_STATE = 0x22,

	REQ_TYPE_MASK = 0x60,
	REQ_TYPE_STANDARD = 0x00,
	REQ_TYPE_CLASS = 0x20,
	REQ_RECIPIENT_MASK = 0x1f,
	REQ_DIR_IN = 0x80,

	EP_TYPE_ISOCHRONOUS = 1,
	EP_TYPE_BULK = 2,
	EP_TYPE_INTERRUPT = 3,

	EP_CDC_NOTIFY = 1,
	EP_CDC_OUT = 2,
	EP_CDC_IN = 2,
	EP_AUDIO_OUT = 3,
	EP_AUDIO_IN = 3,

	IF_AUDIO_CONTROL = 0,
	IF_AUDIO_STREAM_OUT = 1,
	IF_AUDIO_STREAM_IN = 2,
	IF_CDC_CONTROL = 3,
	IF_CDC_DATA = 4,

	GRXSTS_PKT_OUT = 2,
	GRXSTS_PKT_OUT_DONE = 3,
	GRXSTS_PKT_SETUP_DONE = 4,
	GRXSTS_PKT_SETUP = 6
};

enum {
	kEp0Size = UsbCompositeAudioCdc::Ep0Size,
	kCdcInterruptPacketSize = UsbCompositeAudioCdc::CdcInterruptPacketSize,
	kCdcDataPacketSize = UsbCompositeAudioCdc::CdcDataPacketSize,
	kAudioPacketSize = UsbCompositeAudioCdc::AudioPacketSize
};

static UsbCompositeAudioCdc gUsb;

static USB_OTG_DeviceTypeDef *dev()
{
	return reinterpret_cast<USB_OTG_DeviceTypeDef *>(USB2_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE);
}

static USB_OTG_INEndpointTypeDef *inEp(uint8_t ep)
{
	return reinterpret_cast<USB_OTG_INEndpointTypeDef *>(USB2_OTG_FS_PERIPH_BASE + USB_OTG_IN_ENDPOINT_BASE + ep * USB_OTG_EP_REG_SIZE);
}

static USB_OTG_OUTEndpointTypeDef *outEp(uint8_t ep)
{
	return reinterpret_cast<USB_OTG_OUTEndpointTypeDef *>(USB2_OTG_FS_PERIPH_BASE + USB_OTG_OUT_ENDPOINT_BASE + ep * USB_OTG_EP_REG_SIZE);
}

static volatile uint32_t *fifo(uint8_t ep)
{
	return reinterpret_cast<volatile uint32_t *>(USB2_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + ep * USB_OTG_FIFO_SIZE);
}

static volatile uint32_t *pcgcctl()
{
	return reinterpret_cast<volatile uint32_t *>(USB2_OTG_FS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE);
}

static uint16_t min16(uint16_t a, uint16_t b)
{
	return a < b ? a : b;
}

static uint16_t ringUsed(uint16_t head, uint16_t tail, uint16_t size)
{
	return head >= tail ? head - tail : size - tail + head;
}

static uint16_t ringFree(uint16_t head, uint16_t tail, uint16_t size)
{
	return static_cast<uint16_t>(size - 1U - ringUsed(head, tail, size));
}

static void put16(uint8_t *dst, uint16_t value)
{
	dst[0] = static_cast<uint8_t>(value);
	dst[1] = static_cast<uint8_t>(value >> 8);
}

static void put24(uint8_t *dst, uint32_t value)
{
	dst[0] = static_cast<uint8_t>(value);
	dst[1] = static_cast<uint8_t>(value >> 8);
	dst[2] = static_cast<uint8_t>(value >> 16);
}

static void put32(uint8_t *dst, uint32_t value)
{
	dst[0] = static_cast<uint8_t>(value);
	dst[1] = static_cast<uint8_t>(value >> 8);
	dst[2] = static_cast<uint8_t>(value >> 16);
	dst[3] = static_cast<uint8_t>(value >> 24);
}

static uint16_t get16(const uint8_t *src)
{
	return static_cast<uint16_t>(src[0] | (src[1] << 8));
}

static int32_t get24s(const uint8_t *src)
{
	uint32_t value = static_cast<uint32_t>(src[0]) |
				(static_cast<uint32_t>(src[1]) << 8) |
				(static_cast<uint32_t>(src[2]) << 16);
	if ((value & 0x00800000U) != 0U) {
		value |= 0xff000000U;
	}
	return static_cast<int32_t>(value);
}

static uint8_t configurationDescriptor[] = {
	9, USB_DESC_CONFIGURATION, 0, 0, 5, 1, 0, 0x80, 250,

	8, USB_DESC_IAD, IF_AUDIO_CONTROL, 3, 0x01, 0x01, 0x20, 0,
	9, USB_DESC_INTERFACE, IF_AUDIO_CONTROL, 0, 0, 0x01, 0x01, 0x20, 0,
	10, USB_DESC_CS_INTERFACE, 0x01, 0x00, 0x01, 52, 0, 2, IF_AUDIO_STREAM_OUT, IF_AUDIO_STREAM_IN,
	12, USB_DESC_CS_INTERFACE, 0x02, 1, 0x01, 0x01, 0, 2, 0x03, 0, 0, 0,
	9, USB_DESC_CS_INTERFACE, 0x03, 2, 0x03, 0x06, 0, 1, 0,
	12, USB_DESC_CS_INTERFACE, 0x02, 3, 0x03, 0x06, 0, 2, 0x03, 0, 0, 0,
	9, USB_DESC_CS_INTERFACE, 0x03, 4, 0x01, 0x01, 0, 3, 0,

	9, USB_DESC_INTERFACE, IF_AUDIO_STREAM_OUT, 0, 0, 0x01, 0x02, 0x20, 0,
	9, USB_DESC_INTERFACE, IF_AUDIO_STREAM_OUT, 1, 1, 0x01, 0x02, 0x20, 0,
	7, USB_DESC_CS_INTERFACE, 0x01, 1, 1, 1, 0,
	11, USB_DESC_CS_INTERFACE, 0x02, 1, 2, 3, 24, 1, 48, 0xbb, 0,
	9, USB_DESC_ENDPOINT, static_cast<uint8_t>(EP_AUDIO_OUT), 0x0d, 0, 0, 1, 0, 0,
	7, USB_DESC_CS_ENDPOINT, 0x01, 0, 0, 0, 0,

	9, USB_DESC_INTERFACE, IF_AUDIO_STREAM_IN, 0, 0, 0x01, 0x02, 0x20, 0,
	9, USB_DESC_INTERFACE, IF_AUDIO_STREAM_IN, 1, 1, 0x01, 0x02, 0x20, 0,
	7, USB_DESC_CS_INTERFACE, 0x01, 2, 1, 1, 0,
	11, USB_DESC_CS_INTERFACE, 0x02, 1, 2, 3, 24, 1, 48, 0xbb, 0,
	9, USB_DESC_ENDPOINT, static_cast<uint8_t>(0x80 | EP_AUDIO_IN), 0x0d, 0, 0, 1, 0, 0,
	7, USB_DESC_CS_ENDPOINT, 0x01, 0, 0, 0, 0,

	8, USB_DESC_IAD, IF_CDC_CONTROL, 2, 0x02, 0x02, 0x01, 0,
	9, USB_DESC_INTERFACE, IF_CDC_CONTROL, 0, 1, 0x02, 0x02, 0x01, 0,
	5, USB_DESC_CS_INTERFACE, 0x00, 0x10, 0x01,
	5, USB_DESC_CS_INTERFACE, 0x01, 0, IF_CDC_DATA,
	4, USB_DESC_CS_INTERFACE, 0x02, 0x02,
	5, USB_DESC_CS_INTERFACE, 0x06, IF_CDC_CONTROL, IF_CDC_DATA,
	7, USB_DESC_ENDPOINT, static_cast<uint8_t>(0x80 | EP_CDC_NOTIFY), 0x03, kCdcInterruptPacketSize, 0, 16,
	9, USB_DESC_INTERFACE, IF_CDC_DATA, 0, 2, 0x0a, 0, 0, 0,
	7, USB_DESC_ENDPOINT, static_cast<uint8_t>(EP_CDC_OUT), 0x02, kCdcDataPacketSize, 0, 0,
	7, USB_DESC_ENDPOINT, static_cast<uint8_t>(0x80 | EP_CDC_IN), 0x02, kCdcDataPacketSize, 0, 0
};

static const uint8_t deviceQualifier[] = {
	10, USB_DESC_DEVICE_QUALIFIER, 0x00, 0x02, 0xef, 0x02, 0x01, kEp0Size, 1, 0
};

static const char *defaultManufacturer = "F5BUD";
static const char *defaultProduct = "FX-4CR USB Audio CDC";
static const char *defaultSerial = "FX4CR";

static void patchConfigDescriptor()
{
	put16(&configurationDescriptor[2], static_cast<uint16_t>(sizeof(configurationDescriptor)));
	for (unsigned i = 0; i + 6 < sizeof(configurationDescriptor); ++i) {
		if (configurationDescriptor[i] == 9 &&
			configurationDescriptor[i + 1] == USB_DESC_ENDPOINT &&
			(configurationDescriptor[i + 2] == EP_AUDIO_OUT ||
			 configurationDescriptor[i + 2] == static_cast<uint8_t>(0x80 | EP_AUDIO_IN))) {
			put16(&configurationDescriptor[i + 4], kAudioPacketSize);
		}
	}
}

} // namespace

void UsbCdcAcm::reset()
{
	rxHead = 0;
	rxTail = 0;
	txHead = 0;
	txTail = 0;
	ctrlState = 0;
	coding.baudRate = 115200;
	coding.stopBits = 0;
	coding.parity = 0;
	coding.dataBits = 8;
}

uint16_t UsbCdcAcm::available() const
{
	return ringUsed(rxHead, rxTail, RxBufferSize);
}

uint16_t UsbCdcAcm::read(uint8_t *data, uint16_t length)
{
	uint16_t count = 0;
	while (count < length && rxTail != rxHead) {
		data[count++] = rxBuffer[rxTail];
		rxTail = static_cast<uint16_t>((rxTail + 1U) % RxBufferSize);
	}
	return count;
}

uint16_t UsbCdcAcm::write(const uint8_t *data, uint16_t length)
{
	uint16_t count = 0;
	while (count < length && ringFree(txHead, txTail, TxBufferSize) != 0U) {
		txBuffer[txHead] = data[count++];
		txHead = static_cast<uint16_t>((txHead + 1U) % TxBufferSize);
	}
	return count;
}

const CdcLineCoding &UsbCdcAcm::lineCoding() const
{
	return coding;
}

uint16_t UsbCdcAcm::controlLineState() const
{
	return ctrlState;
}

uint16_t UsbCdcAcm::pushRx(const uint8_t *data, uint16_t length)
{
	uint16_t count = 0;
	while (count < length && ringFree(rxHead, rxTail, RxBufferSize) != 0U) {
		rxBuffer[rxHead] = data[count++];
		rxHead = static_cast<uint16_t>((rxHead + 1U) % RxBufferSize);
	}
	return count;
}

uint16_t UsbCdcAcm::popTx(uint8_t *data, uint16_t length)
{
	uint16_t count = 0;
	while (count < length && txTail != txHead) {
		data[count++] = txBuffer[txTail];
		txTail = static_cast<uint16_t>((txTail + 1U) % TxBufferSize);
	}
	return count;
}

uint16_t UsbCdcAcm::txPending() const
{
	return ringUsed(txHead, txTail, TxBufferSize);
}

void UsbCdcAcm::setLineCoding(const uint8_t *data)
{
	coding.baudRate = static_cast<uint32_t>(data[0]) |
		(static_cast<uint32_t>(data[1]) << 8) |
		(static_cast<uint32_t>(data[2]) << 16) |
		(static_cast<uint32_t>(data[3]) << 24);
	coding.stopBits = data[4];
	coding.parity = data[5];
	coding.dataBits = data[6];
}

void UsbCdcAcm::getLineCoding(uint8_t *data) const
{
	put32(data, coding.baudRate);
	data[4] = coding.stopBits;
	data[5] = coding.parity;
	data[6] = coding.dataBits;
}

void UsbAudioLine48k24::reset()
{
	callbacks.outputSamples = 0;
	callbacks.inputSamples = 0;
	callbacks.context = 0;
	outEnabled = false;
	inEnabled = false;
}

void UsbAudioLine48k24::setCallbacks(const AudioLine48k24Callbacks *newCallbacks)
{
	if (newCallbacks != 0) {
		callbacks = *newCallbacks;
	} else {
		callbacks.outputSamples = 0;
		callbacks.inputSamples = 0;
		callbacks.context = 0;
	}
}

void UsbAudioLine48k24::setOutputEnabled(bool enabled)
{
	outEnabled = enabled;
}

void UsbAudioLine48k24::setInputEnabled(bool enabled)
{
	inEnabled = enabled;
}

bool UsbAudioLine48k24::outputEnabled() const
{
	return outEnabled;
}

bool UsbAudioLine48k24::inputEnabled() const
{
	return inEnabled;
}

void UsbAudioLine48k24::receivedOutputPacket(const uint8_t *data, uint16_t length)
{
	if (!outEnabled || callbacks.outputSamples == 0) {
		return;
	}
	int32_t samples[FramesPerMs * Channels];
	uint16_t frames = static_cast<uint16_t>(length / BytesPerFrame);
	if (frames > FramesPerMs) {
		frames = FramesPerMs;
	}
	for (uint16_t frame = 0; frame < frames; ++frame) {
		for (uint16_t channel = 0; channel < Channels; ++channel) {
			samples[frame * Channels + channel] = get24s(&data[frame * BytesPerFrame + channel * 3]);
		}
	}
	callbacks.outputSamples(samples, frames, callbacks.context);
}

uint16_t UsbAudioLine48k24::buildInputPacket(uint8_t *data, uint16_t maxLength)
{
	if (!inEnabled || callbacks.inputSamples == 0 || maxLength < PacketBytes) {
		return 0;
	}
	int32_t samples[FramesPerMs * Channels];
	uint16_t frames = callbacks.inputSamples(samples, FramesPerMs, callbacks.context);
	if (frames > FramesPerMs) {
		frames = FramesPerMs;
	}
	for (uint16_t frame = 0; frame < frames; ++frame) {
		for (uint16_t channel = 0; channel < Channels; ++channel) {
			put24(&data[frame * BytesPerFrame + channel * 3],
			      static_cast<uint32_t>(samples[frame * Channels + channel]));
		}
	}
	return static_cast<uint16_t>(frames * BytesPerFrame);
}

void UsbCompositeAudioCdc::init(const UsbDeviceId *deviceId, const AudioLine48k24Callbacks *audioCallbacks)
{
	id.vendorId = deviceId != 0 ? deviceId->vendorId : 0x0483;
	id.productId = deviceId != 0 ? deviceId->productId : 0x5740;
	id.deviceBcd = deviceId != 0 ? deviceId->deviceBcd : 0x0200;
	id.manufacturer = deviceId != 0 && deviceId->manufacturer != 0 ? deviceId->manufacturer : defaultManufacturer;
	id.product = deviceId != 0 && deviceId->product != 0 ? deviceId->product : defaultProduct;
	id.serial = deviceId != 0 && deviceId->serial != 0 ? deviceId->serial : defaultSerial;
	buildDeviceDescriptor();
	patchConfigDescriptor();
	cdcClass.reset();
	audioClass.reset();
	audioClass.setCallbacks(audioCallbacks);
	resetState();
	initPinsAndClock();
	initCore();
	openControlEndpoint();
	prepareEp0Setup();
	NVIC_EnableIRQ(OTG_FS_IRQn);
	dev()->DCTL &= ~USB_OTG_DCTL_SDIS;
}

void UsbCompositeAudioCdc::irq()
{
	uint32_t pending = USB2_OTG_FS->GINTSTS & USB2_OTG_FS->GINTMSK;
	if ((pending & USB_OTG_GINTSTS_USBRST) != 0U) {
		USB2_OTG_FS->GINTSTS = USB_OTG_GINTSTS_USBRST;
		resetState();
		flushRxFifo();
		flushTxFifo(0x10);
		dev()->DAINTMSK = 0x00010001U;
		USB2_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;
		openControlEndpoint();
		prepareEp0Setup();
	}
	if ((pending & USB_OTG_GINTSTS_ENUMDNE) != 0U) {
		USB2_OTG_FS->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
		USB2_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_TRDT_0 | USB_OTG_GUSBCFG_TRDT_1 | USB_OTG_GUSBCFG_TRDT_3;
	}
	if ((pending & USB_OTG_GINTSTS_RXFLVL) != 0U) {
		handleRxFifo();
	}
	if ((pending & USB_OTG_GINTSTS_IEPINT) != 0U) {
		handleInInterrupts();
	}
	if ((pending & USB_OTG_GINTSTS_OEPINT) != 0U) {
		handleOutInterrupts();
	}
	if ((pending & USB_OTG_GINTSTS_SOF) != 0U) {
		USB2_OTG_FS->GINTSTS = USB_OTG_GINTSTS_SOF;
		serviceAudioIn();
	}
}

void UsbCompositeAudioCdc::poll()
{
	serviceCdcIn();
}

bool UsbCompositeAudioCdc::configured() const
{
	return configuration != 0U;
}

UsbCdcAcm &UsbCompositeAudioCdc::cdc()
{
	return cdcClass;
}

UsbAudioLine48k24 &UsbCompositeAudioCdc::audio()
{
	return audioClass;
}

void UsbCompositeAudioCdc::resetState()
{
	configuration = 0;
	pendingAddress = 0;
	hasPendingAddress = 0;
	ep0TxData = 0;
	ep0TxRemaining = 0;
	controlOut = ControlOutNone;
	for (uint8_t i = 0; i < 4; ++i) {
		epInBusy[i] = 0;
	}
	audioClass.setOutputEnabled(false);
	audioClass.setInputEnabled(false);
}

void UsbCompositeAudioCdc::initPinsAndClock()
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
	RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGFSEN;
	RCC->D2CCIP2R = (RCC->D2CCIP2R & ~RCC_D2CCIP2R_USBSEL) | RCC_D2CCIP2R_USBSEL_0;
	PWR->CR3 |= PWR_CR3_USB33DEN | PWR_CR3_USBREGEN;
	SYSCFG->PMCR |= SYSCFG_PMCR_BOOSTEN;

	GPIOA->MODER = (GPIOA->MODER & ~((3UL << (11U * 2U)) | (3UL << (12U * 2U)))) |
		(2UL << (11U * 2U)) | (2UL << (12U * 2U));
	GPIOA->OTYPER &= ~((1UL << 11U) | (1UL << 12U));
	GPIOA->OSPEEDR |= (3UL << (11U * 2U)) | (3UL << (12U * 2U));
	GPIOA->PUPDR &= ~((3UL << (11U * 2U)) | (3UL << (12U * 2U)));
	GPIOA->AFR[1] = (GPIOA->AFR[1] & ~((15UL << ((11U - 8U) * 4U)) | (15UL << ((12U - 8U) * 4U)))) |
		(10UL << ((11U - 8U) * 4U)) | (10UL << ((12U - 8U) * 4U));
}

void UsbCompositeAudioCdc::initCore()
{
	RCC->AHB1RSTR |= RCC_AHB1RSTR_USB2OTGFSRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_USB2OTGFSRST;
	USB2_OTG_FS->GAHBCFG = 0;
	USB2_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL | USB_OTG_GUSBCFG_FDMOD;
	USB2_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN | USB_OTG_GOTGCTL_BVALOVAL;
	for (volatile uint32_t delay = 0; delay < 200000U; ++delay) {
	}
	USB2_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
	while ((USB2_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_CSRST) != 0U) {
	}
	while ((USB2_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0U) {
	}
	*pcgcctl() = 0;
	USB2_OTG_FS->GCCFG = (USB2_OTG_FS->GCCFG | USB_OTG_GCCFG_PWRDWN) & ~USB_OTG_GCCFG_VBDEN;
	USB2_OTG_FS->GINTMSK = USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM |
		USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT |
		USB_OTG_GINTMSK_SOFM;
	USB2_OTG_FS->GINTSTS = 0xffffffffU;
	dev()->DCFG = USB_OTG_DCFG_DSPD_0 | USB_OTG_DCFG_DSPD_1;
	dev()->DCTL |= USB_OTG_DCTL_SDIS;
	USB2_OTG_FS->GRXFSIZ = 256;
	USB2_OTG_FS->DIEPTXF0_HNPTXFSIZ = (64U << 16) | 256U;
	USB2_OTG_FS->DIEPTXF[0] = (32U << 16) | 320U;
	USB2_OTG_FS->DIEPTXF[1] = (128U << 16) | 352U;
	USB2_OTG_FS->DIEPTXF[2] = (128U << 16) | 480U;
	dev()->DIEPMSK = USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_TXFURM;
	dev()->DOEPMSK = USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_STUPM;
	dev()->DAINTMSK = 0x00010001U;
	USB2_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

void UsbCompositeAudioCdc::openControlEndpoint()
{
	inEp(0)->DIEPCTL = (inEp(0)->DIEPCTL & ~USB_OTG_DIEPCTL_MPSIZ) | kEp0Size | USB_OTG_DIEPCTL_USBAEP;
	outEp(0)->DOEPCTL = (outEp(0)->DOEPCTL & ~USB_OTG_DOEPCTL_MPSIZ) | USB_OTG_DOEPCTL_USBAEP;
}

void UsbCompositeAudioCdc::openClassEndpoints()
{
	inEp(EP_CDC_NOTIFY)->DIEPCTL = kCdcInterruptPacketSize | USB_OTG_DIEPCTL_USBAEP |
		(EP_TYPE_INTERRUPT << USB_OTG_DIEPCTL_EPTYP_Pos) |
		(EP_CDC_NOTIFY << USB_OTG_DIEPCTL_TXFNUM_Pos);
	outEp(EP_CDC_OUT)->DOEPCTL = kCdcDataPacketSize | USB_OTG_DOEPCTL_USBAEP |
		(EP_TYPE_BULK << USB_OTG_DOEPCTL_EPTYP_Pos);
	inEp(EP_CDC_IN)->DIEPCTL = kCdcDataPacketSize | USB_OTG_DIEPCTL_USBAEP |
		(EP_TYPE_BULK << USB_OTG_DIEPCTL_EPTYP_Pos) |
		(EP_CDC_IN << USB_OTG_DIEPCTL_TXFNUM_Pos);
	outEp(EP_AUDIO_OUT)->DOEPCTL = kAudioPacketSize | USB_OTG_DOEPCTL_USBAEP |
		(EP_TYPE_ISOCHRONOUS << USB_OTG_DOEPCTL_EPTYP_Pos);
	inEp(EP_AUDIO_IN)->DIEPCTL = kAudioPacketSize | USB_OTG_DIEPCTL_USBAEP |
		(EP_TYPE_ISOCHRONOUS << USB_OTG_DIEPCTL_EPTYP_Pos) |
		(EP_AUDIO_IN << USB_OTG_DIEPCTL_TXFNUM_Pos);
	dev()->DAINTMSK = (1U << 0) | (1U << EP_CDC_NOTIFY) | (1U << EP_CDC_IN) | (1U << EP_AUDIO_IN) |
		(1U << 16) | (1U << (16 + EP_CDC_OUT)) | (1U << (16 + EP_AUDIO_OUT));
	prepareOut(EP_CDC_OUT, kCdcDataPacketSize);
	prepareOut(EP_AUDIO_OUT, kAudioPacketSize);
}

void UsbCompositeAudioCdc::flushRxFifo()
{
	USB2_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
	while ((USB2_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) != 0U) {
	}
}

void UsbCompositeAudioCdc::flushTxFifo(uint8_t fifoNumber)
{
	USB2_OTG_FS->GRSTCTL = (USB2_OTG_FS->GRSTCTL & ~USB_OTG_GRSTCTL_TXFNUM) |
		(static_cast<uint32_t>(fifoNumber) << USB_OTG_GRSTCTL_TXFNUM_Pos) |
		USB_OTG_GRSTCTL_TXFFLSH;
	while ((USB2_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) != 0U) {
	}
}

void UsbCompositeAudioCdc::prepareOut(uint8_t ep, uint16_t length)
{
	uint32_t packets = (length + kEp0Size - 1U) / kEp0Size;
	if (ep == EP_CDC_OUT) {
		packets = 1;
	} else if (ep == EP_AUDIO_OUT) {
		packets = 1;
	}
	outEp(ep)->DOEPTSIZ = length | (packets << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
	outEp(ep)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
}

void UsbCompositeAudioCdc::prepareEp0Setup()
{
	outEp(0)->DOEPTSIZ = (3U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) |
		(1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | (8U * 3U);
	outEp(0)->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
}

void UsbCompositeAudioCdc::startIn(uint8_t ep, const uint8_t *data, uint16_t length)
{
	if (epInBusy[ep] != 0U) {
		return;
	}
	uint32_t packetSize = ep == 0 ? kEp0Size : (ep == EP_CDC_NOTIFY ? kCdcInterruptPacketSize :
		(ep == EP_CDC_IN ? kCdcDataPacketSize : kAudioPacketSize));
	uint32_t packets = length == 0 ? 1U : (length + packetSize - 1U) / packetSize;
	inEp(ep)->DIEPTSIZ = length | (packets << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
	inEp(ep)->DIEPCTL |= USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA;
	if (length != 0U && data != 0) {
		writeFifo(ep, data, length);
	}
	epInBusy[ep] = 1;
}

void UsbCompositeAudioCdc::writeFifo(uint8_t ep, const uint8_t *data, uint16_t length)
{
	volatile uint32_t *txFifo = fifo(ep);
	uint16_t offset = 0;
	while (offset < length) {
		uint32_t word = 0;
		for (uint8_t i = 0; i < 4 && offset < length; ++i) {
			word |= static_cast<uint32_t>(data[offset++]) << (8U * i);
		}
		*txFifo = word;
	}
}

void UsbCompositeAudioCdc::readFifo(uint8_t *data, uint16_t length)
{
	volatile uint32_t *rxFifo = fifo(0);
	uint16_t offset = 0;
	while (offset < length) {
		uint32_t word = *rxFifo;
		for (uint8_t i = 0; i < 4 && offset < length; ++i) {
			data[offset++] = static_cast<uint8_t>(word >> (8U * i));
		}
	}
}

void UsbCompositeAudioCdc::handleRxFifo()
{
	uint32_t mask = USB2_OTG_FS->GINTMSK;
	USB2_OTG_FS->GINTMSK = mask & ~USB_OTG_GINTMSK_RXFLVLM;
	while ((USB2_OTG_FS->GINTSTS & USB_OTG_GINTSTS_RXFLVL) != 0U) {
		uint32_t status = USB2_OTG_FS->GRXSTSP;
		uint8_t ep = static_cast<uint8_t>((status & USB_OTG_GRXSTSP_EPNUM) >> USB_OTG_GRXSTSP_EPNUM_Pos);
		uint16_t count = static_cast<uint16_t>((status & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos);
		uint8_t packetStatus = static_cast<uint8_t>((status & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos);
		if (packetStatus == GRXSTS_PKT_SETUP && ep == 0 && count == 8) {
			readFifo(ep0Buffer, 8);
			SetupPacket setup;
			setup.bmRequestType = ep0Buffer[0];
			setup.bRequest = ep0Buffer[1];
			setup.wValue = get16(&ep0Buffer[2]);
			setup.wIndex = get16(&ep0Buffer[4]);
			setup.wLength = get16(&ep0Buffer[6]);
			handleSetup(setup);
		} else if (packetStatus == GRXSTS_PKT_OUT && count != 0U) {
			uint8_t *rxData = ep == EP_AUDIO_OUT ? audioPacket : ep0Buffer;
			uint16_t rxCapacity = ep == EP_AUDIO_OUT ? static_cast<uint16_t>(sizeof(audioPacket)) :
				static_cast<uint16_t>(sizeof(ep0Buffer));
			uint16_t readLength = min16(count, rxCapacity);
			readFifo(rxData, readLength);
			if (count > readLength) {
				uint8_t drop[4];
				for (uint16_t remaining = static_cast<uint16_t>(count - readLength); remaining != 0U;) {
					uint16_t chunk = min16(remaining, static_cast<uint16_t>(sizeof(drop)));
					readFifo(drop, chunk);
					remaining = static_cast<uint16_t>(remaining - chunk);
				}
			}
			if (ep == 0) {
				handleEp0OutData(rxData, readLength);
			} else if (ep == EP_CDC_OUT) {
				cdcClass.pushRx(rxData, readLength);
				prepareOut(EP_CDC_OUT, kCdcDataPacketSize);
			} else if (ep == EP_AUDIO_OUT) {
				audioClass.receivedOutputPacket(rxData, readLength);
				prepareOut(EP_AUDIO_OUT, kAudioPacketSize);
			}
		} else if (packetStatus == GRXSTS_PKT_OUT_DONE) {
		} else if (packetStatus == GRXSTS_PKT_SETUP_DONE) {
			prepareEp0Setup();
		}
	}
	USB2_OTG_FS->GINTMSK = mask;
}

void UsbCompositeAudioCdc::handleInInterrupts()
{
	uint32_t pending = dev()->DAINT & dev()->DAINTMSK & USB_OTG_DAINT_IEPINT;
	for (uint8_t ep = 0; ep < 4; ++ep) {
		if ((pending & (1U << ep)) == 0U) {
			continue;
		}
		uint32_t epPending = inEp(ep)->DIEPINT & dev()->DIEPMSK;
		if ((epPending & USB_OTG_DIEPINT_XFRC) != 0U) {
			inEp(ep)->DIEPINT = USB_OTG_DIEPINT_XFRC;
			epInBusy[ep] = 0;
			if (ep == 0) {
				finishEp0In();
			} else if (ep == EP_CDC_IN) {
				serviceCdcIn();
			}
		}
		if ((epPending & USB_OTG_DIEPINT_TXFE) != 0U) {
			inEp(ep)->DIEPINT = USB_OTG_DIEPINT_TXFE;
		}
	}
}

void UsbCompositeAudioCdc::handleOutInterrupts()
{
	uint32_t pending = (dev()->DAINT & dev()->DAINTMSK & USB_OTG_DAINT_OEPINT) >> USB_OTG_DAINT_OEPINT_Pos;
	for (uint8_t ep = 0; ep < 4; ++ep) {
		if ((pending & (1U << ep)) == 0U) {
			continue;
		}
		uint32_t epPending = outEp(ep)->DOEPINT & dev()->DOEPMSK;
		outEp(ep)->DOEPINT = epPending;
	}
}

void UsbCompositeAudioCdc::handleSetup(const SetupPacket &setup)
{
	ep0TxData = 0;
	ep0TxRemaining = 0;
	controlOut = ControlOutNone;
	if ((setup.bmRequestType & REQ_TYPE_MASK) == REQ_TYPE_STANDARD) {
		handleStandardRequest(setup);
	} else if ((setup.bmRequestType & REQ_TYPE_MASK) == REQ_TYPE_CLASS) {
		handleClassRequest(setup);
	} else {
		stallControl();
	}
}

void UsbCompositeAudioCdc::handleStandardRequest(const SetupPacket &setup)
{
	if (setup.bRequest == USB_REQ_GET_DESCRIPTOR) {
		uint16_t length = 0;
		const uint8_t *data = descriptor(static_cast<uint8_t>(setup.wValue >> 8),
			static_cast<uint8_t>(setup.wValue), &length);
		if (data != 0) {
			if (length > setup.wLength) {
				length = setup.wLength;
			}
			sendControlData(data, length);
			return;
		}
	} else if (setup.bRequest == USB_REQ_SET_ADDRESS) {
		setAddress(static_cast<uint8_t>(setup.wValue & 0x7fU));
		sendControlStatus();
		return;
	} else if (setup.bRequest == USB_REQ_SET_CONFIGURATION) {
		setConfiguration(static_cast<uint8_t>(setup.wValue));
		sendControlStatus();
		return;
	} else if (setup.bRequest == USB_REQ_GET_CONFIGURATION) {
		ep0Buffer[0] = configuration;
		sendControlData(ep0Buffer, 1);
		return;
	} else if (setup.bRequest == USB_REQ_GET_STATUS) {
		ep0Buffer[0] = 0;
		ep0Buffer[1] = 0;
		sendControlData(ep0Buffer, 2);
		return;
	} else if (setup.bRequest == USB_REQ_GET_INTERFACE) {
		ep0Buffer[0] = (setup.wIndex == IF_AUDIO_STREAM_OUT && audioClass.outputEnabled()) ||
			(setup.wIndex == IF_AUDIO_STREAM_IN && audioClass.inputEnabled()) ? 1 : 0;
		sendControlData(ep0Buffer, 1);
		return;
	} else if (setup.bRequest == USB_REQ_SET_INTERFACE) {
		if (setup.wIndex == IF_AUDIO_STREAM_OUT) {
			audioClass.setOutputEnabled(setup.wValue != 0U);
			if (audioClass.outputEnabled()) {
				prepareOut(EP_AUDIO_OUT, kAudioPacketSize);
			}
			sendControlStatus();
			return;
		}
		if (setup.wIndex == IF_AUDIO_STREAM_IN) {
			audioClass.setInputEnabled(setup.wValue != 0U);
			sendControlStatus();
			return;
		}
		sendControlStatus();
		return;
	} else if (setup.bRequest == USB_REQ_CLEAR_FEATURE) {
		sendControlStatus();
		return;
	}
	if ((setup.bmRequestType & REQ_RECIPIENT_MASK) == 0U) {
		sendControlStatus();
		return;
	}
	stallControl();
}

void UsbCompositeAudioCdc::handleClassRequest(const SetupPacket &setup)
{
	if ((setup.wIndex & 0xffU) != IF_CDC_CONTROL) {
		stallControl();
		return;
	}
	if (setup.bRequest == CDC_REQ_GET_LINE_CODING && (setup.bmRequestType & REQ_DIR_IN) != 0U) {
		cdcClass.getLineCoding(ep0Buffer);
		sendControlData(ep0Buffer, min16(7, setup.wLength));
		return;
	}
	if (setup.bRequest == CDC_REQ_SET_LINE_CODING && (setup.bmRequestType & REQ_DIR_IN) == 0U && setup.wLength == 7) {
		controlOut = ControlOutCdcLineCoding;
		prepareOut(0, 7);
		return;
	}
	if (setup.bRequest == CDC_REQ_SET_CONTROL_LINE_STATE) {
		cdcClass.ctrlState = setup.wValue;
		sendControlStatus();
		return;
	}
	stallControl();
}

void UsbCompositeAudioCdc::handleEp0OutData(const uint8_t *data, uint16_t length)
{
	if (controlOut == ControlOutCdcLineCoding && length >= 7U) {
		cdcClass.setLineCoding(data);
		controlOut = ControlOutNone;
		sendControlStatus();
	} else {
		prepareEp0Setup();
	}
}

void UsbCompositeAudioCdc::sendControlData(const uint8_t *data, uint16_t length)
{
	ep0TxData = data;
	ep0TxRemaining = length;
	uint16_t chunk = min16(ep0TxRemaining, kEp0Size);
	startIn(0, ep0TxData, chunk);
	ep0TxData += chunk;
	ep0TxRemaining = static_cast<uint16_t>(ep0TxRemaining - chunk);
}

void UsbCompositeAudioCdc::sendControlStatus()
{
	startIn(0, 0, 0);
}

void UsbCompositeAudioCdc::stallControl()
{
	inEp(0)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
	outEp(0)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
	prepareEp0Setup();
}

void UsbCompositeAudioCdc::finishEp0In()
{
	if (ep0TxRemaining != 0U) {
		uint16_t chunk = min16(ep0TxRemaining, kEp0Size);
		startIn(0, ep0TxData, chunk);
		ep0TxData += chunk;
		ep0TxRemaining = static_cast<uint16_t>(ep0TxRemaining - chunk);
		return;
	}
	if (hasPendingAddress != 0U) {
		dev()->DCFG = (dev()->DCFG & ~USB_OTG_DCFG_DAD) |
			(static_cast<uint32_t>(pendingAddress) << USB_OTG_DCFG_DAD_Pos);
		hasPendingAddress = 0;
	}
	prepareOut(0, kEp0Size);
}

void UsbCompositeAudioCdc::setAddress(uint8_t address)
{
	pendingAddress = address;
	hasPendingAddress = 1;
}

void UsbCompositeAudioCdc::setConfiguration(uint8_t value)
{
	configuration = value;
	if (configuration != 0U) {
		openClassEndpoints();
	} else {
		audioClass.setOutputEnabled(false);
		audioClass.setInputEnabled(false);
	}
}

void UsbCompositeAudioCdc::serviceCdcIn()
{
	if (configuration == 0U || epInBusy[EP_CDC_IN] != 0U || cdcClass.txPending() == 0U) {
		return;
	}
	uint16_t length = cdcClass.popTx(cdcTxPacket, kCdcDataPacketSize);
	startIn(EP_CDC_IN, cdcTxPacket, length);
}

void UsbCompositeAudioCdc::serviceAudioIn()
{
	if (configuration == 0U || epInBusy[EP_AUDIO_IN] != 0U || !audioClass.inputEnabled()) {
		return;
	}
	uint16_t length = audioClass.buildInputPacket(audioPacket, kAudioPacketSize);
	if (length != 0U) {
		startIn(EP_AUDIO_IN, audioPacket, length);
	}
}

const uint8_t *UsbCompositeAudioCdc::descriptor(uint8_t type, uint8_t index, uint16_t *length)
{
	if (type == USB_DESC_DEVICE) {
		*length = static_cast<uint16_t>(sizeof(deviceDescriptor));
		return deviceDescriptor;
	}
	if (type == USB_DESC_CONFIGURATION) {
		*length = static_cast<uint16_t>(sizeof(configurationDescriptor));
		return configurationDescriptor;
	}
	if (type == USB_DESC_STRING) {
		return stringDescriptor(index, length);
	}
	if (type == USB_DESC_DEVICE_QUALIFIER) {
		*length = static_cast<uint16_t>(sizeof(deviceQualifier));
		return deviceQualifier;
	}
	return 0;
}

const uint8_t *UsbCompositeAudioCdc::stringDescriptor(uint8_t index, uint16_t *length)
{
	const char *text = 0;
	if (index == 0) {
		stringBuffer[0] = 4;
		stringBuffer[1] = USB_DESC_STRING;
		stringBuffer[2] = 0x09;
		stringBuffer[3] = 0x04;
		*length = 4;
		return stringBuffer;
	}
	if (index == 1) {
		text = id.manufacturer;
	} else if (index == 2) {
		text = id.product;
	} else if (index == 3) {
		text = id.serial;
	} else {
		return 0;
	}
	uint8_t count = 0;
	while (text[count] != '\0' && count < 31U) {
		stringBuffer[2U + count * 2U] = static_cast<uint8_t>(text[count]);
		stringBuffer[3U + count * 2U] = 0;
		++count;
	}
	stringBuffer[0] = static_cast<uint8_t>(2U + count * 2U);
	stringBuffer[1] = USB_DESC_STRING;
	*length = stringBuffer[0];
	return stringBuffer;
}

void UsbCompositeAudioCdc::buildDeviceDescriptor()
{
	deviceDescriptor[0] = 18;
	deviceDescriptor[1] = USB_DESC_DEVICE;
	put16(&deviceDescriptor[2], 0x0200);
	deviceDescriptor[4] = 0xef;
	deviceDescriptor[5] = 0x02;
	deviceDescriptor[6] = 0x01;
	deviceDescriptor[7] = kEp0Size;
	put16(&deviceDescriptor[8], id.vendorId);
	put16(&deviceDescriptor[10], id.productId);
	put16(&deviceDescriptor[12], id.deviceBcd);
	deviceDescriptor[14] = 1;
	deviceDescriptor[15] = 2;
	deviceDescriptor[16] = 3;
	deviceDescriptor[17] = 1;
}

UsbCompositeAudioCdc &device()
{
	return gUsb;
}

} // namespace usb
} // namespace fx4cr

extern "C" void OTG_FS_IRQHandler(void)
{
	fx4cr::usb::device().irq();
}
