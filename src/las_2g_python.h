#ifndef LAS_2G_PYTHON_H
#define LAS_2G_PYTHON_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define WITHHELD_CLASSIFICATION 0x80
#define ENTRY_SIZE 28
#define HEADER_SIZE 0xE3 // the header size
#define HEADER_STRING_SIZE 32 // the size of the strings for the system identifier and generating software

#pragma pack (push)
#pragma pack(1)

/**
 * @brief Structure for containing LAS header information.
 * 
 */
//typedef struct __attribute__((__packed__)) {
typedef struct {
    char file_signature[4];
    uint16_t file_source_id;
    uint16_t global_encoding;
    uint32_t guid_data_1;
    uint16_t guid_data_2;
    uint16_t guid_data_3;
    uint64_t guid_data_4;
    uint8_t version_major;
    uint8_t version_minor;
    char system_identifier[HEADER_STRING_SIZE];
    char generating_software[HEADER_STRING_SIZE];
    uint16_t file_creation_day_of_year;
    uint16_t file_creation_year ;
    uint16_t header_size;
    uint32_t offset_to_point_data;
    uint32_t number_of_variable_length_records;
    uint8_t point_data_format_id;
    uint16_t point_data_record_length;
    uint32_t number_of_point_records;
    uint32_t number_of_points_by_return[5];
    double x_scale_factor;
    double y_scale_factor;
    double z_scale_factor;
    double x_offset;
    double y_offset;
    double z_offset;
    double max_x;
    double min_x;
    double max_y;
    double min_y;
    double max_z;
    double min_z;
} LASHeader;


/**
 * @brief structure for the point entries. 2G uses Point Data Record Format 1
 * 
 */
//typedef struct __attribute__((__packed__)) {
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
    uint16_t intensity;
    uint8_t bit_field; /// represents the byte that has sub fields defined
    uint8_t classification;
    uint8_t scan_angle; // always 0 relative to scanner
    uint8_t user_data;
    uint16_t point_source_id;
    double gps_time;
} LASEntry;

#pragma pack (pop)

/**
 * @brief structure for a single LAS file. 
 * 
 */
typedef struct {
    LASHeader * header;
    LASEntry * entries;
} LASFile;

/**
 * @brief Create an empty LAS header
 * 
 * @param utc_seconds time in us since unix epoch
 * @param number of entries.
 * @param 
 * @return LASHeader* created on the heap, ownership passed to the caller. 
 */
LASHeader * initLASHeader (uint64_t utc_time, uint32_t number_of_points);

/**
 * @brief Create an empty LASEntry.
 * 
 * @param utc_seconds Time in us from the Unix epoch (converted to adjusted GPS time internally)
 * @param x double in m
 * @param y double in m
 * @param z double in m
 * @param intensity
 * @param quality
 * @return LASEntry .
 */
LASEntry  initLASEntry (uint64_t utc_time, double x, double y, double z, uint16_t intensity, uint8_t quality);

/**
 * @brief Write the LAS file consisting of a header and an array of entries onto the hard drive.
 * 
 * @param filename 
 * @param array of pointers to las file types.
 * @param number_of_file_entries number of entries
 * @return int Number of files written, or -1 if failure
 */
int write_las( const char * filename, LASFile las_files[], size_t number_of_file_entries);

/**
 * @brief Read a header from a file
 * @param open fid (does no checks)
 * @param pointer to header.
 * @return error code 0 success.
 */
size_t read_header(FILE * fid, LASHeader * header);

/**
 * @brief Read all the entries from a file
 * @param open fid (does no checks)
 * @param pointer to array of entries.
 * @param number of entries
 * @return error code 0 success.
 */
size_t read_entry(FILE * fid, LASEntry * entries, size_t number_of_entries);

/**
 * @brief Read a header from a file
 * @param open fid (does no checks)
 * @param pointer to header.
 * @return error code 0 success.
 */
size_t write_header(FILE * fid, LASHeader * header);

/**
 * @brief Read all the entries from a file
 * @param open fid (does no checks)
 * @param pointer to array of entries.
 * @param number of entries
 * @return error code 0 success.
 */
size_t write_entries(FILE * fid, LASEntry * entries, size_t number_of_entries);

/**
 * @brief Read a LAS file from the hard drive
 * 
 * @param filename 
 * @param las_files Array to pointers to las files. Caller gets ownership of every file pointer and their contained headers and entries.
 * @return int number of las files read, or -1 for error.
 */
int read_las( const char * filename, LASFile *** las_files);

/**
 * @brief Convert Adjusted GPS to UTC time.
 * 
 * @param adj_pps_time in us
 * @return uint64_t in us
 */
uint64_t AdjustedGPSTimeusToUTCTimeus(uint64_t adj_pps_time);

/**
 * Convert UTC time to adjusted GPS time (GPS time - 1x10^9)
 * @param utc_time - UTC time in microseconds
 * @return adjusted GPS time in seconds
 */
double UTCTimeusToAdjustedGPSTime(uint64_t utc_time);

#endif