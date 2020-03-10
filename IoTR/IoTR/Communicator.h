// Communicator.h

#ifndef _COMMUNICATOR_h
#define _COMMUNICATOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include "StatusCodes.h"

#pragma endregion

#pragma region Definitions

/** @brief Interval at which to check [milliseconds]. */
#define UIPDATE_TIME 5

/** @brief Minimum frame length. */
#define FRAME_MIN_LEN 6

/** @brief Frame data size. */
#define FRAME_MAX_DATA_LEN 26

/** @brief Maximum frame length. */
#define FRAME_MAX_LEN 32

/** @brief Number 5 stand of sum of length of static size fields;
* 1 - Start
* 1 - Request / Response
* 1 - Length
* 1 - OpCode
*/
#define FRAME_REQUEST_STATIC_FIELD_SIZE 4

#define FRAME_REQUEST_STATIC_OFFSET_SIZE 1

/** @brief Number 5 stand of sum of length of static size fields;
* 1 - Start
* 1 - Request / Response
* 1 - Length
* 1 - Opcode
* 1 - Status
*/
#define FRAME_RESPONSE_STATIC_FIELD_SIZE 5

#define FRAME_RESPONSE_STATIC_OFFSET_SIZE 2

/** @brief Length of the CRC. */
#define FRAME_CRC_LEN 2

/** @brief Beginning byte value. */
#define FRAME_BEGIN_VALUE 0xAA

/** @brief Measurements index size. */
#define MEASUREMENT_INDEX_SIZE 2

/** @brief Measurements data size. */
#define MEASUREMENT_DATA_SIZE 8

/** @brief Measurements count. */
#define MEASUREMENTS_COUNT 3

/** @brief Measurements count. */
#define MEASUREMENT_FIELD_SIZE 4

#pragma endregion

#pragma region Enums

/** @brief Frame indexes byte. */
enum FrameIndexes : int
{
	Begin = 0, ///< Beginning byte.
	RequestResponse, ///< Request / Response byte.
	Length, ///< Length of the frame.
	OperationCode,  ///< Operation Code byte.
	StatusCode,  ///< Status Code byte.
};

enum FrameState : uint8
{
	fsBegin = 0, ///< Beginning byte.
	fsRequestResponse, ///< Request / Response byte.
	fsLength, ///< Length of the frame.
	fsOperationCode,  ///< Operation Code byte.
	fsData, //< Data part.
	fsCRC, ///< CRC state
};

/** @brief Request / Response flags. */
enum ReqRes : uint8
{
	Request = 1U, ///< Request flag.
	Response = 2U, ///< Response flag.
};

/** @brief Operation codes byte. */
enum OpCodes : uint8
{
	Ping = 1U, ///< Ping

};

#pragma endregion

#pragma region Unions

/** @brief Unsigned long union for converting bytes to unsigned Long. */
union uLongtUnion {
	uint8 Buffer[sizeof(long unsigned int)]; ///< Byte buffer.
	long unsigned int Value; ///< Interpreted value.
};

/** @brief Integer union for converting bytes to unsigned int. */
union IntUnion {
	uint8 Buffer[sizeof(int)]; ///< Byte buffer.
	int Value; ///< Interpreted value.
};

/** @brief uint16_t union for converting bytes to unsigned int. */
union UInt16Union {
	uint8 Buffer[sizeof(uint16)]; ///< Byte buffer.
	uint16 Value; ///< Interpreted value.
};

/** @brief uint32_t union for converting bytes to unsigned int. */
union UInt32Union {
	uint8 Buffer[sizeof(uint32)]; ///< Byte buffer.
	uint32 Value; ///< Interpreted value.
};

#pragma endregion

/** Communication module class. */
class CommunicationModule {

public:

#pragma region Methods

	/** @brief Send RAW request frame.
	 *  @param opcode uint8, Operation code.
	 *  @param frame uint8*, Command for this operation code.
	 *  @param length const uint8, Length of the payload.
	 *  @return Void.
	 */
	void sendRawRequest(uint8 opcode, uint8 * payload, const uint8 length);

	/** @brief Send RAW response frame.
	 *  @param opcode uint8, Operation code.
	 *  @param frame uint8*, Command for this operation code.
	 *  @param length const uint8, Length of the payload.
	 *  @return Void.
	 */
	void sendRawResponse(uint8 opcode, uint8 status, uint8 * payload, const uint8 length);


	void getResponsePayload(uint8* buffer);

	/** @brief Setup the communication module.
	 *  @return Void.
	 */
	void setup();

	/** @brief Handle incoming commands.
	 *  @return Void.
	 */
	void update();

	/** @brief Set callback for WiFi mode.
	 *  @param Callback function.
	 *  @return Void.
	 */
	void setCallbackWiFiMode(void(*callback)(uint8 mode));

	/** @brief Set callback for WEB server state.
	 *  @param Callback function.
	 *  @return Void.
	 */
	void setCallbackWebServerState(void(*callback)(uint8 state));

	/** @brief Set callback for ESP update check.
	 *  @param Callback function.
	 *  @return Void.
	 */
	void setCallbackCheckUpdateESP(void(*callback)(void));


	/** @brief Set callback for EFM32 update check.
     *  @param Callback function.
     *  @return Void.
     */
	void setCallbackCheckUpdateRobot(void(*callback)(void));



	bool _isValidPackage = false;

	/** @brief Read incoming commands.
     *  @return Void.
     */
	void readFrame(bool takeResponse = false, uint32 timeOut = 10000);

	uint8 isSuccesResponse(uint8 opCode);

#pragma endregion

private:

#pragma region Variables

	/** @brief Will store last time LED was updated. */
	unsigned long _PreviousMillis = 0;

	/** @brief Communication frame buffer. */
	uint8 _FrameBuffer[FRAME_MAX_LEN];

	uint8 _OutFrameBuffer[FRAME_MAX_LEN];

	uint8 ReadState = FrameState::fsBegin;

	uint8 TemporalDataLength = 0;

	uint8 * ptrFrameBuffer;

	/** @brief Callback WiFi mode. */
	void(*callbackWiFiMode)(uint8 mode);

	/** @brief Callback WEB server mode. */
	void(*callbackWebServerMode)(uint8 mode);

	/** @brief Callback check for ESP updates. */
	void(*callbackCheckUpdateESP)(void);

	/** @brief Callback check for EFM updates. */
	void(*callbackCheckUpdateRobot)(void);

#pragma endregion

#pragma region Methods

	/** @brief Parse and execute the incoming commands.
	 *  @param frame The frame string.
	 *  @return Void.
	 */
	void parseFrame(uint8 * buffer, uint8 length);

	/** @brief Validate the incoming commands.
	 *  @param buffer uint8*, The frame array.
	 *  @param length uint8, Length of the frame.
	 *  @return True if successful; or False if failed.
	 */
	bool validateFrame(uint8 * buffer, uint8 length);

	/** @brief Validate the CRC.
	 *  @param buffer uint8*, The frame array.
	 *  @param length uint8, Length of the frame.
	 *  @return True if successful; or False if failed.
	 */
	bool validateCRC(uint8 * buffer, uint8 length);

	/** @brief Calculate the CRC.
	 *  @param frame String, The frame string.
	 *  @return Void
	 */
	void calculateCRC(uint8 * buffer, uint8 length, uint8 * out);

	/** @brief Extract the payload of the frame.
	 *  @param buffer uint8*, The frame array.
	 *  @param length uint8, Length of the frame.
	 *  @param out uint8*, Length of the frame.
	 *  @return Payload of the frame.
	 */
	void getRequestPayload(uint8 * buffer, uint8 length, uint8 * out);

	/** @brief Extract the payload of the frame.
	 *  @param buffer uint8 *, Frame buffer.
	 *  @return Void.
	 */
	void clearFrame();

#pragma endregion

};

/* @brief Communicator instance. */
extern CommunicationModule Communicator;

#endif
