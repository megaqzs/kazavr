/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zmk/split/transport/types.h>

#define ZMK_SPLIT_WIRED_ENVELOPE_MAGIC_PREFIX {'Z', 'm', 'K', 'w'}
#define ZMK_SPLIT_WIRED_ENVELOPE_MAGIC_PREFIX_SIZE 4

struct msg_prefix {
    uint8_t magic_prefix[ZMK_SPLIT_WIRED_ENVELOPE_MAGIC_PREFIX_SIZE];
    uint8_t payload_size;
} __packed;

struct command_payload {
    uint8_t source;
    struct zmk_split_transport_central_command cmd;
} __packed;

struct command_envelope {
    struct msg_prefix prefix;
    struct command_payload payload;
} __packed;

struct event_payload {
    uint8_t source;
    struct zmk_split_transport_peripheral_event event;
} __packed;

struct event_envelope {
    struct msg_prefix prefix;
    struct event_payload payload;
} __packed;

struct msg_postfix {
    uint32_t crc;
} __packed;

#define MSG_EXTRA_SIZE (sizeof(struct msg_prefix) + sizeof(struct msg_postfix))
