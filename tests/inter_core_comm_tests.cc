#define printf_ printf
#define __DMB()

// static inline unsigned HAL_GetTick() {
// 	static unsigned tm = 0;
// 	tm++;
// 	return tm;
// }

#include "doctest.h"
#include "drivers/inter_core_comm.hh"

enum class Volume { USB, SDCard, NorFlash, RamDisk, MaxVolumes };
struct InterCoreCommMessage {
	enum MessageType : uint32_t {
		None,

		RequestRefreshPatchList,
		PatchListChanged,
		PatchListUnchanged,

		RequestPatchData,
		PatchDataLoadFail,
		PatchDataLoaded,

		NumRequests,
	};
	MessageType message_type;
	uint32_t bytes_read;
	uint32_t patch_id;
	Volume vol_id;

	// Note: return type is void because gcc gives a warning for `volatile T&operator=(const T&){...}`
	// warning: implicit dereference will not access object of type 'volatile T' in statement
	void operator=(const InterCoreCommMessage &msg) volatile { //NOLINT
		message_type = msg.message_type;
		bytes_read = msg.bytes_read;
		patch_id = msg.patch_id;
		vol_id = msg.vol_id;
	}

	InterCoreCommMessage &operator=(volatile InterCoreCommMessage &msg) { //NOLINT
		this->message_type = msg.message_type;
		this->bytes_read = msg.bytes_read;
		this->patch_id = msg.patch_id;
		this->vol_id = msg.vol_id;
		return *this;
	}
};

static volatile InterCoreCommMessage sharedmsg;
static mdrivlib::InterCoreComm<mdrivlib::ICCCoreType::Initiator, InterCoreCommMessage> tx{sharedmsg};
static mdrivlib::InterCoreComm<mdrivlib::ICCCoreType::Responder, InterCoreCommMessage> rx{sharedmsg};

TEST_CASE("ICC") {

	SUBCASE("Tx can send right away, but only once") {
		mdrivlib::IPCC_<1>::reset();
		mdrivlib::IPCC_<2>::reset();

		InterCoreCommMessage msg;
		msg.message_type = InterCoreCommMessage::RequestRefreshPatchList;
		bool ok = tx.send_message(msg);
		CHECK(ok);

		ok = tx.send_message(msg);
		CHECK_FALSE(ok);
	}

	SUBCASE("RX cannot send right away") {
		mdrivlib::IPCC_<1>::reset();
		mdrivlib::IPCC_<2>::reset();

		InterCoreCommMessage msg;
		msg.message_type = InterCoreCommMessage::RequestRefreshPatchList;
		bool ok = rx.send_message(msg);
		CHECK_FALSE(ok);
	}

	SUBCASE("Tx sends, RX receives it, sends back, TX receives it, repeat") {
		mdrivlib::IPCC_<1>::reset();
		mdrivlib::IPCC_<2>::reset();

		InterCoreCommMessage msg;

		// First round:
		msg.message_type = InterCoreCommMessage::RequestRefreshPatchList;
		bool ok = tx.send_message(msg);
		CHECK(ok);

		auto rxmsg = rx.get_new_message();
		CHECK(rxmsg.message_type == InterCoreCommMessage::RequestRefreshPatchList);

		msg.message_type = InterCoreCommMessage::PatchListChanged;
		ok = rx.send_message(msg);
		CHECK(ok);

		auto txmsg = tx.get_new_message();
		CHECK(txmsg.message_type == InterCoreCommMessage::PatchListChanged);

		// Second round:
		msg.message_type = InterCoreCommMessage::RequestPatchData;
		ok = tx.send_message(msg);
		CHECK(ok);

		rxmsg = rx.get_new_message();
		CHECK(rxmsg.message_type == InterCoreCommMessage::RequestPatchData);

		msg.message_type = InterCoreCommMessage::PatchDataLoaded;
		ok = rx.send_message(msg);
		CHECK(ok);

		txmsg = tx.get_new_message();
		CHECK(txmsg.message_type == InterCoreCommMessage::PatchDataLoaded);
	}

	SUBCASE("Checking get_new_message repeatedly doesn't break anything") {
		mdrivlib::IPCC_<1>::reset();
		mdrivlib::IPCC_<2>::reset();

		InterCoreCommMessage msg;

		// First round:
		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);

		msg.message_type = InterCoreCommMessage::RequestRefreshPatchList;
		bool ok = tx.send_message(msg);
		CHECK(ok);

		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);

		auto rxmsg = rx.get_new_message();
		CHECK(rxmsg.message_type == InterCoreCommMessage::RequestRefreshPatchList);

		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);

		msg.message_type = InterCoreCommMessage::PatchListChanged;
		ok = rx.send_message(msg);
		CHECK(ok);

		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);

		auto txmsg = tx.get_new_message();
		CHECK(txmsg.message_type == InterCoreCommMessage::PatchListChanged);

		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(rx.get_new_message().message_type == InterCoreCommMessage::None);

		// Second round:
		msg.message_type = InterCoreCommMessage::RequestPatchData;
		ok = tx.send_message(msg);
		CHECK(ok);

		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);
		CHECK(tx.get_new_message().message_type == InterCoreCommMessage::None);

		rxmsg = rx.get_new_message();
		CHECK(rxmsg.message_type == InterCoreCommMessage::RequestPatchData);
	}
}
