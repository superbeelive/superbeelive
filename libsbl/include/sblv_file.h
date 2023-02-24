#ifndef SBLV_FILE_H
#define SBLV_FILE_H

// Headers et outils pour les
// fichiers video "bruts"

typedef enum {
	MONO8,
	RGB24,
	YUV422
} encoding_t ;


typedef struct {
	char cam_serial[50]; // Serial of sensor
	unsigned int rows;   // Image height
	unsigned int cols;   // Image width
	double fps;          // Frame per second
	encoding_t encoding; // Pixel format
	time_t timestamp;    // Start of capture
	int hive;            // Hive serial
	int module;          // Module
	int cam;             // Camera
} sblv_header;

#endif
