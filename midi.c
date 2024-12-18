#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>
#include "midi.h"

const char *midi_file_format_name(uint16_t n) {
	const char *format_names[] = {
		"single-track",
		"multiple tracks, synchronous",
		"multiple tracks, asynchronous"
	};
	if(n < 3) return format_names[n];
	return "unknown";
}

const char *midi_note_name(uint8_t n, uint8_t *octave) {
	const char *note_names[] = {
		"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
	};
	if(octave) *octave = n / 12 - 1;
	return note_names[n % 12];
}

const char *midi_cc_name(uint8_t cc) {
	const char *cc_names[] = {
		"Bank select",
		"Modulation",
		"Breath Controller",
		"Controller 3 (undefined)",
		"Foot Controller",
		"Portamento Time",
		"Data Entry MSB",
		"Channel Volume (formerly Main Volume)",
		"Balance",
		"Controller 9 (undefined)",
		"Pan",
		"Expression",
		"Effect Control 1",
		"Effect Control 2",
		"Controller 14 (undefined)",
		"Controller 15 (undefined)",
		"General Purpose 1",
		"General Purpose 2",
		"General Purpose 3",
		"General Purpose 4",
		"Controller 20 (undefined)",
		"Controller 21 (undefined)",
		"Controller 22 (undefined)",
		"Controller 23 (undefined)",
		"Controller 24 (undefined)",
		"Controller 25 (undefined)",
		"Controller 26 (undefined)",
		"Controller 27 (undefined)",
		"Controller 28 (undefined)",
		"Controller 29 (undefined)",
		"Controller 30 (undefined)",
		"Controller 31 (undefined)",
		"Bank select LSB",
		"Modulation LSB",
		"Breath Controller LSB",
		"Controller 35 (undefined)",
		"Foot Controller LSB",
		"Portamento Time LSB",
		"Data Entry LSB",
		"Channel Volume LSB (formerly Main Volume)",
		"Balance LSB",
		"Controller 41 (undefined)",
		"Pan LSB",
		"Expression LSB",
		"Effect Control 1 LSB",
		"Effect Control 2 LSB",
		"Controller 46 (undefined)",
		"Controller 47 (undefined)",
		"General Purpose 1 LSB",
		"General Purpose 2 LSB",
		"General Purpose 3 LSB",
		"General Purpose 4 LSB",
		"Controller 52 (undefined)",
		"Controller 53 (undefined)",
		"Controller 54 (undefined)",
		"Controller 55 (undefined)",
		"Controller 56 (undefined)",
		"Controller 57 (undefined)",
		"Controller 58 (undefined)",
		"Controller 59 (undefined)",
		"Controller 60 (undefined)",
		"Controller 61 (undefined)",
		"Controller 62 (undefined)",
		"Controller 63 (undefined)",
		"Damper Pedal (Sustain)",
		"Portamento On/Off",
		"Sostenuto On/Off",
		"Soft Pedal On/Off",
		"Legato Footswitch",
		"Hold 2",
		"Sound Controller 1 (Sound Variation)",
		"Sound Controller 2 (Resonance/Timbre)",
		"Sound Controller 3 (Release Time)",
		"Sound Controller 4 (Attack Time)",
		"Sound Controller 5 (Cut-off Frequency/Brightness)",
		"Sound Controller 6 (Decay Time)",
		"Sound Controller 7 (Vibrato Rate)",
		"Sound Controller 8 (Vibrato Depth)",
		"Sound Controller 9 (Vibrato Delay)",
		"Sound Controller 10 (undefined)",
		"General Purpose 5",
		"General Purpose 6",
		"General Purpose 7",
		"General Purpose 7",
		"Portamento Control",
		"Controller 85 (undefined)",
		"Controller 86 (undefined)",
		"Controller 87 (undefined)",
		"Controller 88 (undefined)",
		"Controller 89 (undefined)",
		"Controller 90 (undefined)",
		"Effects 1 Depth (Reverb)",
		"Effects 2 Depth (Tremolo)",
		"Effects 3 Depth (Chorus)",
		"Effects 4 Depth (Celeste/Detune)",
		"Effects 5 Depth (Phaser)",
		"Data Entry +1 (Increment)",
		"Data Entry -1(Decrement)",
		"NRPN LSB",
		"NRPN MSB",
		"RPN LSB",
		"RPN MSB",
		"Controller 102 (undefined)",
		"Controller 103 (undefined)",
		"Controller 104 (undefined)",
		"Controller 105 (undefined)",
		"Controller 106 (undefined)",
		"Controller 107 (undefined)",
		"Controller 108 (undefined)",
		"Controller 109 (undefined)",
		"Controller 110 (undefined)",
		"Controller 111 (undefined)",
		"Controller 112 (undefined)",
		"Controller 113 (undefined)",
		"Controller 114 (undefined)",
		"Controller 115 (undefined)",
		"Controller 116 (undefined)",
		"Controller 117 (undefined)",
		"Controller 118 (undefined)",
		"Controller 119 (undefined)",
		"All Sound Off",
		"Reset All Controllers",
		"Local Control On/Off",
		"All Notes Off",
		"Omni Mode Off",
		"Omni Mode On",
		"Poly Mode Off/Mono Mode On",
		"Poly Mode On/Mono Mode Off",
	};
	return cc_names[cc & 0x7f];
}

const char *midi_rpn_name(uint16_t rpn) {
	const char *names[] = {
		"Pitch Bend Sensitivity",
		"Fine Tuning",
		"Coarse Tuning",
		"Tuning Program Select",
		"Tuning Bank Select",
	};
	if(rpn < 5) return names[rpn];
	return "Unknown";
}

float midi_note_freq(int note, float fraction) {
	return 440.0 * pow(2, (note + fraction - 69.0) / 12.0);
}

int midi_pitch_to_note(float pitch_hz, float *fraction) {
	float f = 12 * log(pitch_hz / 220.0) / log(2.0);
	int note = round(f);
	if(note > 70) note = 70;
	if(note < -57) note = -57;
	if(fraction) *fraction = f - note;
	return (int)note + 57;
}

void midi_meta_event_string(int cmd, int len, uint8_t *data, char *buf, int buf_size) {
	const char *mlive_tags[] = {
		"genre", "artist", "composer",
		"duration (seconds)", "bpm (tempo)",
	};
	switch(cmd) {
		case 0x00: {
			if(len == 0)
				snprintf(buf, buf_size, "Sequence number (no data bytes)");
			else if(len == 2)
				snprintf(buf, buf_size, "Sequence number %d", data[0] | data[1] << 8);
			else
				snprintf(buf, buf_size, "Sequence number (invalid length %d)", len);
			break;
		}
		case 0x01: snprintf(buf, buf_size, "Text \"%.*s\"", len, data); break;
		case 0x02: snprintf(buf, buf_size, "Copyright \"%.*s\"", len, data); break;
		case 0x03: snprintf(buf, buf_size, "Track name \"%.*s\"", len, data); break;
		case 0x04: snprintf(buf, buf_size, "Instrument name \"%.*s\"", len, data); break;
		case 0x05: snprintf(buf, buf_size, "Lyric \"%.*s\"", len, data); break;
		case 0x06: snprintf(buf, buf_size, "Marker \"%.*s\"", len, data); break;
		case 0x07: snprintf(buf, buf_size, "Cue point \"%.*s\"", len, data); break;
		case 0x2f: snprintf(buf, buf_size, "End of Track"); break;
		case 0x4b: snprintf(buf, buf_size, "M-Live Tag tag=%d (%s) \"%.*s\"", data[0], data[0] < sizeof(mlive_tags) / sizeof(mlive_tags[0]) ? mlive_tags[data[0]] : "-", len - 1, data + 1); break;
		case 0x58: {
			if(len == 2) snprintf(buf, buf_size, "Time signature %d/%d", data[0], data[1]);
			else if(len == 4) snprintf(buf, buf_size, "Time signature %d/%d %d clocks/click %d 32nd notes per quarter note", data[0], data[1], data[2], data[3]);
			else snprintf(buf, buf_size, "Time signature (invalid length %d)", len);
			break;
		}
	}
}
