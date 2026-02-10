#include <ErriezCRC32.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>
#include <zmk/split/wired.h>
#include <utils.h>
#include <stdio.h>

static const uint8_t peripheral_id = 0;

static size_t get_payload_data_size(const struct zmk_split_transport_peripheral_event *evt) {
    switch (evt->type) {
    case ZMK_SPLIT_TRANSPORT_PERIPHERAL_EVENT_TYPE_INPUT_EVENT:
        return sizeof(evt->data.input_event);
    case ZMK_SPLIT_TRANSPORT_PERIPHERAL_EVENT_TYPE_KEY_POSITION_EVENT:
        return sizeof(evt->data.key_position_event);
    case ZMK_SPLIT_TRANSPORT_PERIPHERAL_EVENT_TYPE_BATTERY_EVENT:
        return sizeof(evt->data.battery_event);
    default:
        return -1;
    }
}


static int split_peripheral_wired_report_event(const struct zmk_split_transport_peripheral_event *event) {
    size_t data_size = get_payload_data_size(event);
    if (data_size < 0) {
        //LOG_WRN("Failed to determine payload data size %d", data_size);
        return data_size;
    }

    // Data + type + source
    uint8_t payload_size =
        data_size + sizeof(peripheral_id) + sizeof(enum zmk_split_transport_peripheral_event_type);

    //if (ring_buf_space_get(&chosen_tx_buf) < MSG_EXTRA_SIZE + payload_size) {
    //    LOG_WRN("No room to send peripheral to the central (have %d but only space for %d)",
    //            MSG_EXTRA_SIZE + payload_size, ring_buf_space_get(&chosen_tx_buf));
    //    return -ENOSPC;
    //}


    struct event_envelope env = {
    .prefix = {
        .magic_prefix = ZMK_SPLIT_WIRED_ENVELOPE_MAGIC_PREFIX,
        .payload_size = payload_size
    },
    .payload = {
        .source = peripheral_id,
        .event = *event,
    }};

    struct msg_postfix postfix = {.crc =
                                      crc32Buffer((uint8_t *)&env, sizeof(env.prefix) + payload_size)};

    //LOG_HEXDUMP_DBG(&env, sizeof(env.prefix) + payload_size, "Payload");

    payload_size += sizeof(env.prefix);
    uint8_t i;

    for (i = 0; i < payload_size; i++)
        putchar(((char*) &env)[i]);
    for (i = 0; i < sizeof(postfix); i++)
        putchar(((char*) &postfix)[i]);

//#if !IS_HALF_DUPLEX_MODE
//    begin_tx();
//#endif

    return 0;
}

int wpos_upd(uint8_t row, uint8_t column, bool pressed) {
    struct zmk_split_transport_peripheral_event evt = {
        .type = ZMK_SPLIT_TRANSPORT_PERIPHERAL_EVENT_TYPE_KEY_POSITION_EVENT,
        .data = {
            .key_position_event = {
                .position = get_keycode(row, column),
                .pressed = pressed
            }
        }
    };

    if (evt.data.key_position_event.position >= 0)
        return split_peripheral_wired_report_event(&evt);
    return -1;
}
