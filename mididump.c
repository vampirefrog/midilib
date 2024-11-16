#include "midi.h"
#include "midi_reader.h"

static void hex_dump(const uint8_t *data, size_t len) {
	for(size_t i = 0; i < len; i++) printf("%02x", data[i]);
}

void handle_header(struct midi_reader *r) {
	printf("MIDI header_len=%d file_format=%d num_tracks=%d ticks_per_quarter_note=%d\n", r->header_len, r->file_format, r->num_tracks, r->ticks_per_quarter_note);
}

void handle_byte(struct midi_reader *r, uint8_t b) {
//		printf("%02x ", b);
}

void handle_track(struct midi_reader *r, int number, int length) {
	printf("Track %d (%d bytes)\n", number, length);
}

void handle_note_on(struct midi_reader *r, uint8_t channel, int duration, int note, int vel) {
	uint8_t octave;
	const char *note_n = midi_note_name(note, &octave);
	printf("%08x %02u Note On %s%d (%d) %d\n", duration, channel, note_n, octave, note, vel);
}

void handle_note_off(struct midi_reader *r, uint8_t channel, int duration, int note, int vel) {
	uint8_t octave = 0;
	const char *note_n = midi_note_name(note, &octave);
	printf("%08x %02u Note Off %s%d (%d) %d\n", duration, channel, note_n, octave, note, vel);
}

void handle_key_after_touch(struct midi_reader *r, uint8_t channel, int duration, int note, int vel) {
	printf("%08x %02u After Touch %d %d);\n", duration, channel, note, vel);
}

void handle_control_change(struct midi_reader *r, uint8_t channel, int duration, int controller, int value) {
	printf("%08x %02u Control Change controller=%d (%s) value=%d\n", duration, channel, controller, midi_cc_name(controller), value);
}

void handle_program_change(struct midi_reader *r, uint8_t channel, int duration, int program) {
	printf("%08x %02u Program Change program=%d;\n", duration, channel, program);
}

void handle_channel_after_touch(struct midi_reader *r, uint8_t channel, int duration, int value) {
	printf("%08x %02u After Touch value=%d\n", duration, channel, value);
}

void handle_pitch_wheel_change(struct midi_reader *r, uint8_t channel, uint32_t duration, uint16_t value) {
	printf("%08x %02u Pitch Wheel value=%d\n", duration, channel, value);
}

void handle_meta_event(struct midi_reader *r, int duration, int cmd, int len, uint8_t *data) {
	char buf[256];
	midi_meta_event_string(cmd, len, data, buf, sizeof(buf));
	printf("%08x Meta Event 0x%02x len=%d %s ", duration, cmd, len, buf);
	hex_dump(data, len);
	printf("\n");
}

void handle_meta_sequence_number(struct midi_reader *r, int duration, uint16_t seq) {
	printf("%08x Sequence Number %d\n", duration, seq);
}

void handle_text_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Text \"%s\"\n", duration, txt);
}

void handle_copyright_info_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Copyright \"%s\"\n", duration, txt);
}

void handle_track_name_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Track Name \"%s\"\n", duration, txt);
}

void handle_instrument_name_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Instrument Name \"%s\"\n", duration, txt);
}

void handle_lyric_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Lyric \"%s\"\n", duration, txt);
}

void handle_marker_text_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Marker Text \"%s\"\n", duration, txt);
}

void handle_program_name_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Program Name \"%s\"\n", duration, txt);
}

void handle_device_name_event(struct midi_reader *r, int duration, const char *txt) {
	printf("%08x Device Name \"%s\"\n", duration, txt);
}

void handle_tempo(struct midi_reader *r, int duration, uint32_t tempo) {
	printf("%08x Tempo %d BPM (%d)\n", duration, tempo == 0 ? 0 : 60000000 / tempo, tempo);
}

void handle_time_signature(struct midi_reader *r, int duration, uint8_t numerator, uint8_t denominator, uint8_t ticks_per_click, uint8_t quarter_note32nd_notes) {
	printf("%08x Time Signature %d %d %d %d\n", duration, numerator, denominator, ticks_per_click, quarter_note32nd_notes);
}

void handle_track_end(struct midi_reader *r, int duration) {
	printf("%08x Track End\n", duration);
}

void handle_rpn(struct midi_reader *r, uint8_t channel, int duration, uint16_t number, uint8_t msb) {
	printf("%08x %02d RPN %d (%s) = %d\n", channel, duration, number, midi_rpn_name(number), msb);
}
void handle_rpn_lsb(struct midi_reader *r, uint8_t channel, int duration, uint16_t number, uint8_t lsb) {
	printf("%08x %02d RPN %d (%s) LSB = %d\n", channel, duration, number, midi_rpn_name(number), lsb);
}
void handle_nrpn(struct midi_reader *r, uint8_t channel, int duration, uint16_t number, uint8_t msb) {
	printf("%08x %02d NRPN %d = %d\n", channel, duration, number, msb);
}
void handle_nrpn_lsb(struct midi_reader *r, uint8_t channel, int duration, uint16_t number, uint8_t lsb) {
	printf("%08x %02d NRPN %d LSB = %d\n", channel, duration, number, lsb);
}

void handle_sysex_byte(struct midi_reader *r, uint8_t b) {
	printf("SysEx %02x\n", b);
}

int main(int argc, char **argv) {
	struct midi_reader reader;

	midi_reader_init(&reader);
	struct file_stream stream;
	file_stream_init(&stream, argv[1], "rb");

	reader.handle_header = handle_header;
	reader.handle_byte = handle_byte;
	reader.handle_track = handle_track;
	reader.handle_note_on = handle_note_on;
	reader.handle_note_off = handle_note_off;
	reader.handle_key_after_touch = handle_key_after_touch;
	reader.handle_control_change = handle_control_change;
	reader.handle_program_change = handle_program_change;
	reader.handle_channel_after_touch = handle_channel_after_touch;
	reader.handle_pitch_wheel_change = handle_pitch_wheel_change;
	reader.handle_meta_event = handle_meta_event;
	reader.handle_meta_sequence_number = handle_meta_sequence_number;
	reader.handle_text_event = handle_text_event;
	reader.handle_copyright_info_event = handle_copyright_info_event;
	reader.handle_track_name_event = handle_track_name_event;
	reader.handle_instrument_name_event = handle_instrument_name_event;
	reader.handle_lyric_event = handle_lyric_event;
	reader.handle_marker_text_event = handle_marker_text_event;
	reader.handle_program_name_event = handle_program_name_event;
	reader.handle_device_name_event = handle_device_name_event;
	reader.handle_tempo = handle_tempo;
	reader.handle_time_signature = handle_time_signature;
	reader.handle_track_end = handle_track_end;
	reader.handle_rpn = handle_rpn;
	reader.handle_rpn_lsb = handle_rpn_lsb;
	reader.handle_nrpn = handle_nrpn;
	reader.handle_nrpn_lsb = handle_nrpn_lsb;
	reader.handle_sysex_byte = handle_sysex_byte;

	midi_reader_load(&reader, (struct stream *)&stream);

	return 0;
}
