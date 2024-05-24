#define _CRT_SECURE_NO_WARNINGS

#include <inttypes.h> // Include this header for PRIx16
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cctype>
#include <string>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
/**
 * @brief Convert RGB values to grayscale and pad to 16 bits in binary format.
 *
 * @param r The red component.
 * @param g The green component.
 * @param b The blue component.
 * @return uint16_t Grayscale value padded to 16 bits in binary format.
 */
uint16_t rgbToGrayscale(uint8_t r, uint8_t g, uint8_t b) {
	// Calculate grayscale value using the formula: 0.299*R + 0.587*G + 0.114*B
	uint16_t grayscale = (uint16_t)((0.299 * r) + (0.587 * g) + (0.114 * b));

	// Pad to 16 bits in binary format (0bXXXXXXXXXXXXXXXX)
	return grayscale;
}

/**
 * @brief Load a PNG file and convert pixel data to padded 16-bit binary format.
 *
 * @param filename The name of the PNG file to load.
 * @param width Pointer to store the image width.
 * @param height Pointer to store the image height.
 * @return uint16_t* A dynamically allocated array of padded 16-bit binary pixel values.
 */
uint16_t* loadAndConvertImage(const char* filename, int* width, int* height) {
	int comp;
	uint8_t* image_data = stbi_load(filename, width, height, &comp, STBI_rgb);

	if (image_data == NULL) {
		printf("Error: Could not load the image file.\n");
		return NULL;
	}

	// Calculate the size of the output array
	int num_pixels = (*width) * (*height);
	uint16_t* output_data = (uint16_t*)malloc(sizeof(uint16_t) * num_pixels);

	if (output_data == NULL) {
		printf("Error: Memory allocation failed.\n");
		stbi_image_free(image_data);
		return NULL;
	}

	// Convert and pad each pixel to 16 bits in binary format
	for (int i = 0; i < num_pixels; i++) {
		// Extract 12-bit RGB values
		uint8_t r = image_data[i * 3];
		uint8_t g = image_data[i * 3 + 1];
		uint8_t b = image_data[i * 3 + 2];

		// Pad to 16 bits in binary format (0bRRRRRRGGGGGGBBBB)
		output_data[i] = rgbToGrayscale(r, g, b);
	}

	// Free the loaded image data
	stbi_image_free(image_data);

	return output_data;
}
/**
 * @brief Save the 16-bit binary pixel data as a C header file.
 *
 * @param filename The name of the output header file.
 * @param data Pointer to the 16-bit binary pixel data array.
 * @param width The image width.
 * @param height The image height.
 */
void saveAsCHeader(const char* filename, const uint16_t* data, const char* name, int width, int height) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create the output file." << std::endl;
        return;
    }

    // convert name to uppercase for include guard
    std::string nameUpper(name);
    for (auto& c : nameUpper) c = std::toupper(static_cast<unsigned char>(c));

    // include guard and includes
    file << "#ifndef " << nameUpper << "_H\n";
    file << "#define " << nameUpper << "_H\n\n";
    file << "#include <stdint.h>\n\n";

    // convert name to lowercase for the array name
    std::string nameLower(name);
    for (auto& c : nameLower) c = std::tolower(static_cast<unsigned char>(c));

    file << "const uint16_t " << nameLower << "[] PROGMEM = {\n0b";

    // write the pixel data
    for (int i = 0; i < (width * height) - 1; ++i) {
        if (data[i] > 10) {
            file << "1";
        } else {
            file << "0";
        }

        if ((i + 1) % width == 0) {
            file << ",\n0b";
        }
    }

    file << "};\n\n";
    file << "#endif // " << nameUpper << "_H\n";

    file.close();
    std::cout << "Image data saved as " << filename << std::endl;
}
int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Usage: %s <input_filename> <output_filename>  <output_array_name>\n", argv[0]);
		return 1;
	}

	const char* input_filename = argv[1];
	const char* output_filename = argv[2];
	char* output_name = argv[3];
	int width, height;

	uint16_t* image_data = loadAndConvertImage(input_filename, &width, &height);

	if (image_data != NULL) {
		saveAsCHeader(output_filename, image_data, output_name, width, height);
		free(image_data);
	}

	return 0;
}