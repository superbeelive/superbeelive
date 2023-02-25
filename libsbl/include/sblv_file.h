#ifndef SBLV_FILE_H
#define SBLV_FILE_H

// Headers et outils pour les
// fichiers video "bruts"

typedef enum {
	MONO8,
	MONO16,
	RGB24
} encoding_t ;


typedef struct {
	char cam_serial[50]; // Serial of sensor
	unsigned int rows;   // Image height
	unsigned int cols;   // Image width
	double fps;          // Frame per second
	encoding_t encoding; // Pixel format
	time_t timestamp;    // Start of capture
	unsigned int hive;            // Hive ID
	unsigned int module;          // Module ID
	unsigned int cam;             // Camera ID
} sblv_header;

#endif
